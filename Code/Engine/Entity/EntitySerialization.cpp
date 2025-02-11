#include "EntitySerialization.h"
#include "EntityMap.h"
#include "Engine/Entity/EntityDescriptors.h"
#include "Engine/Entity/Entity.h"
#include "System/Serialization/TypeSerialization.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/FileSystem/FileSystem.h"
#include "System/Log.h"
#include <eastl/sort.h>

#if KRG_DEVELOPMENT_TOOLS

using namespace KRG::Serialization;

//-------------------------------------------------------------------------

namespace KRG::EntityModel::Serializer
{
    static bool Error( char const* pFormat, ... )
    {
        va_list args;
        va_start( args, pFormat );
        Log::AddEntryVarArgs( Log::Severity::Error, "Entity Collection Reader", __FILE__, __LINE__, pFormat, args );
        va_end( args );
        return false;
    }
}

//-------------------------------------------------------------------------
// Reading
//-------------------------------------------------------------------------

namespace KRG::EntityModel::Serializer
{
    namespace
    {
        struct ParsingContext
        {
            ParsingContext( TypeSystem::TypeRegistry const& typeRegistry ) : m_typeRegistry( typeRegistry ) {}

            inline void ClearComponentNames()
            {
                m_componentNames.clear();
            }

            inline bool DoesComponentExist( StringID componentName ) const
            {
                return m_componentNames.find( componentName ) != m_componentNames.end();
            }

            inline bool DoesEntityExist( StringID entityName ) const
            {
                return m_entityNames.find( entityName ) != m_entityNames.end();
            }

        public:

            TypeSystem::TypeRegistry const&             m_typeRegistry;

            // Parsing context ID - Entity/Component/etc...
            StringID                                    m_parsingContextName;

            // Maps to allow for fast lookups of entity/component names for validation
            THashMap<StringID, bool>                    m_entityNames;
            THashMap<StringID, bool>                    m_componentNames;
        };

        //-------------------------------------------------------------------------

        static bool ReadAndConvertPropertyValue( ParsingContext& ctx, TypeSystem::TypeInfo const* pTypeInfo, Serialization::JsonValue::ConstMemberIterator memberIter, TypeSystem::PropertyDescriptor& outPropertyDesc )
        {
            if ( !memberIter->value.IsString() )
            {
                return Error( "Property value for (%s) must be a string value.", memberIter->name.GetString() );
            }

            //-------------------------------------------------------------------------

            KRG_ASSERT( !memberIter->value.IsArray() ); // TODO: arrays not supported yet
            outPropertyDesc = TypeSystem::PropertyDescriptor( TypeSystem::PropertyPath( memberIter->name.GetString() ), memberIter->value.GetString(), TypeSystem::TypeID() );

            //-------------------------------------------------------------------------

            auto const pPropertyInfo = ctx.m_typeRegistry.ResolvePropertyPath( pTypeInfo, outPropertyDesc.m_path );
            if ( pPropertyInfo == nullptr )
            {
                return Error( "Failed to resolve property path: %s, for type (%s)", outPropertyDesc.m_path.ToString().c_str(), pTypeInfo->m_ID.c_str() );
            }

            if ( TypeSystem::IsCoreType( pPropertyInfo->m_typeID ) || pPropertyInfo->IsEnumProperty() || pPropertyInfo->IsBitFlagsProperty() )
            {
                if ( !TypeSystem::Conversion::ConvertStringToBinary( ctx.m_typeRegistry, *pPropertyInfo, outPropertyDesc.m_stringValue, outPropertyDesc.m_byteValue ) )
                {
                    return Error( "Failed to convert string value (%s) to binary for property: %s for type (%s)", outPropertyDesc.m_stringValue.c_str(), outPropertyDesc.m_path.ToString().c_str(), pTypeInfo->m_ID.c_str() );
                }
            }

            outPropertyDesc.m_typeID = pPropertyInfo->m_typeID;
            outPropertyDesc.m_templatedArgumentTypeID = pPropertyInfo->m_templateArgumentTypeID;

            return true;
        }

        static bool ReadComponent( ParsingContext& ctx, Serialization::JsonValue const& componentObject, ComponentDescriptor& outComponentDesc )
        {
            // Read name and ID
            //-------------------------------------------------------------------------

            auto nameIter = componentObject.FindMember( "Name" );
            if ( nameIter == componentObject.MemberEnd() || !nameIter->value.IsString() || nameIter->value.GetStringLength() == 0 )
            {
                return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextName.c_str() );
            }

            outComponentDesc.m_name = StringID( nameIter->value.GetString() );

            // Validate type ID
            //-------------------------------------------------------------------------

            auto typeDataIter = componentObject.FindMember( "TypeData" );
            if ( typeDataIter == componentObject.MemberEnd() || !typeDataIter->value.IsObject() )
            {
                return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and Type Data values set", ctx.m_parsingContextName.c_str() );
            }

            Serialization::JsonValue const& componentTypeDataObject = typeDataIter->value;

            auto typeIDIter = componentTypeDataObject.FindMember( Serialization::s_typeIDKey );
            if ( typeIDIter == componentTypeDataObject.MemberEnd() || !typeIDIter->value.IsString() )
            {
                Error( "Invalid type data found for component: '%s' on entity %s!", nameIter->value.GetString(), ctx.m_parsingContextName.c_str() );
                return false;
            }

            outComponentDesc.m_typeID = StringID( typeIDIter->value.GetString() );

            // Spatial component info
            //-------------------------------------------------------------------------

            auto pTypeInfo = ctx.m_typeRegistry.GetTypeInfo( outComponentDesc.m_typeID );
            if ( pTypeInfo == nullptr )
            {
                return Error( "Invalid entity component type ID detected for entity (%s): %s", ctx.m_parsingContextName.c_str(), outComponentDesc.m_typeID.c_str() );
            }

            outComponentDesc.m_isSpatialComponent = pTypeInfo->IsDerivedFrom<SpatialEntityComponent>();

            if ( outComponentDesc.m_isSpatialComponent )
            {
                auto spatialParentIter = componentObject.FindMember( "SpatialParent" );
                if ( spatialParentIter != componentObject.MemberEnd() && spatialParentIter->value.IsString() )
                {
                    outComponentDesc.m_spatialParentName = StringID( spatialParentIter->value.GetString() );
                }

                auto attachmentSocketIter = componentObject.FindMember( "AttachmentSocketID" );
                if ( attachmentSocketIter != componentObject.MemberEnd() && attachmentSocketIter->value.IsString() )
                {
                    outComponentDesc.m_attachmentSocketID = StringID( attachmentSocketIter->value.GetString() );
                }
            }

            // Read property overrides
            //-------------------------------------------------------------------------

            // Reserve memory for all property (+1 extra slot) and create an empty desc
            outComponentDesc.m_properties.reserve( componentTypeDataObject.Size() );
            outComponentDesc.m_properties.push_back( TypeSystem::PropertyDescriptor() );

            // Read all properties
            for ( auto itr = componentTypeDataObject.MemberBegin(); itr != componentTypeDataObject.MemberEnd(); ++itr )
            {
                // Skip Type ID
                if ( strcmp( itr->name.GetString(), Serialization::s_typeIDKey ) == 0 )
                {
                    continue;
                }

                // If we successfully read the property value add a new property value
                if ( ReadAndConvertPropertyValue( ctx, pTypeInfo, itr, outComponentDesc.m_properties.back() ) )
                {
                    outComponentDesc.m_properties.push_back( TypeSystem::PropertyDescriptor() );
                }
                else
                {
                    return false;
                }
            }

            // Remove last entry as it will always be empty
            outComponentDesc.m_properties.pop_back();

            //-------------------------------------------------------------------------

            if ( ctx.DoesComponentExist( outComponentDesc.m_name ) )
            {
                Error( "Duplicate component UUID detected: '%s' on entity %s!", outComponentDesc.m_name.c_str(), ctx.m_parsingContextName.c_str() );
                return false;
            }
            else
            {
                ctx.m_componentNames.insert( TPair<StringID, bool>( outComponentDesc.m_name, true ) );
                return true;
            }
        }

        //-------------------------------------------------------------------------

        static bool ReadSystemData( ParsingContext& ctx, Serialization::JsonValue const& systemObject, SystemDescriptor& outSystemDesc )
        {
            auto typeIDIter = systemObject.FindMember( Serialization::s_typeIDKey );
            if ( typeIDIter == systemObject.MemberEnd() || !typeIDIter->value.IsString() )
            {
                return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", ctx.m_parsingContextName.c_str() );
            }

            if ( typeIDIter->value.GetStringLength() == 0 )
            {
                return Error( "Invalid entity system format (systems must have a TypeID string value set) on entity %s", ctx.m_parsingContextName.c_str() );
            }

            outSystemDesc.m_typeID = StringID( typeIDIter->value.GetString() );
            return true;
        }

        //-------------------------------------------------------------------------

        static bool ReadEntityData( ParsingContext& ctx, Serialization::JsonValue const& entityObject, EntityDescriptor& outEntityDesc )
        {
            // Read name and ID
            //-------------------------------------------------------------------------

            auto nameIter = entityObject.FindMember( "Name" );
            if ( nameIter == entityObject.MemberEnd() || !nameIter->value.IsString() || nameIter->value.GetStringLength() == 0 )
            {
                return Error( "Invalid entity component format detected for entity (%s): components must have ID, Name and TypeID string values set", ctx.m_parsingContextName.c_str() );
            }

            outEntityDesc.m_name = StringID( nameIter->value.GetString() );

            // Read spatial info
            //-------------------------------------------------------------------------

            auto spatialParentIter = entityObject.FindMember( "SpatialParent" );
            if ( spatialParentIter != entityObject.MemberEnd() && spatialParentIter->value.IsString() )
            {
                outEntityDesc.m_spatialParentName = StringID( spatialParentIter->value.GetString() );
            }

            auto attachmentSocketIter = entityObject.FindMember( "AttachmentSocketID" );
            if ( attachmentSocketIter != entityObject.MemberEnd() && attachmentSocketIter->value.IsString() )
            {
                outEntityDesc.m_attachmentSocketID = StringID( attachmentSocketIter->value.GetString() );
            }

            // Set parsing ctx ID
            //-------------------------------------------------------------------------

            ctx.m_parsingContextName = outEntityDesc.m_name;

            //-------------------------------------------------------------------------
            // Read components
            //-------------------------------------------------------------------------

            ctx.ClearComponentNames();

            auto componentsArrayIter = entityObject.FindMember( "Components" );
            if ( componentsArrayIter != entityObject.MemberEnd() && componentsArrayIter->value.IsArray() )
            {
                // Read component data
                //-------------------------------------------------------------------------

                int32_t const numComponents = (int32_t) componentsArrayIter->value.Size();
                KRG_ASSERT( outEntityDesc.m_components.empty() && outEntityDesc.m_numSpatialComponents == 0 );
                outEntityDesc.m_components.resize( numComponents );

                bool wasRootComponentFound = false;

                for ( int32_t i = 0; i < numComponents; i++ )
                {
                    if ( !ReadComponent( ctx, componentsArrayIter->value[i], outEntityDesc.m_components[i] ) )
                    {
                        return Error( "Failed to read component definition %u for entity (%s)", i, outEntityDesc.m_name.c_str() );
                    }

                    if ( outEntityDesc.m_components[i].IsSpatialComponent() )
                    {
                        if ( outEntityDesc.m_components[i].IsRootComponent() )
                        {
                            if ( wasRootComponentFound )
                            {
                                return Error( "Multiple root components found on entity (%s)", outEntityDesc.m_name.c_str() );
                            }
                            else
                            {
                                wasRootComponentFound = true;
                            }
                        }

                        outEntityDesc.m_numSpatialComponents++;
                    }
                }

                // Validate spatial components
                //-------------------------------------------------------------------------

                for ( auto const& componentDesc : outEntityDesc.m_components )
                {
                    if ( componentDesc.IsSpatialComponent() && componentDesc.HasSpatialParent() )
                    {
                        if ( !ctx.DoesComponentExist( componentDesc.m_spatialParentName ) )
                        {
                            return Error( "Couldn't find spatial parent (%s) for component (%s) on entity (%s)", componentDesc.m_spatialParentName.c_str(), componentDesc.m_name.c_str(), outEntityDesc.m_name.c_str() );
                        }
                    }
                }

                // Validate singleton components
                //-------------------------------------------------------------------------
                // As soon as a given component is a singleton all components derived from it are singleton components

                for ( int32_t i = 0; i < numComponents; i++ )
                {
                    auto pComponentTypeInfo = ctx.m_typeRegistry.GetTypeInfo( outEntityDesc.m_components[i].m_typeID );
                    if ( pComponentTypeInfo->IsAbstractType() )
                    {
                        return Error( "Abstract component type detected (%s) found on entity (%s)", pComponentTypeInfo->GetTypeName(), outEntityDesc.m_name.c_str() );
                    }

                    auto pDefaultComponentInstance = Cast<EntityComponent>( pComponentTypeInfo->GetDefaultInstance() );
                    if ( !pDefaultComponentInstance->IsSingletonComponent() )
                    {
                        continue;
                    }

                    for ( int32_t j = 0; j < numComponents; j++ )
                    {
                        if ( i == j )
                        {
                            continue;
                        }

                        if ( ctx.m_typeRegistry.IsTypeDerivedFrom( outEntityDesc.m_components[j].m_typeID, outEntityDesc.m_components[i].m_typeID ) )
                        {
                            return Error( "Multiple singleton components of type (%s) found on the same entity (%s)", pComponentTypeInfo->GetTypeName(), outEntityDesc.m_name.c_str() );
                        }
                    }
                }

                // Sort Components
                //-------------------------------------------------------------------------

                auto comparator = [] ( ComponentDescriptor const& componentDescA, ComponentDescriptor const& componentDescB )
                {
                    // Spatial components have precedence
                    if ( componentDescA.IsSpatialComponent() && !componentDescB.IsSpatialComponent() )
                    {
                        return true;
                    }

                    if ( !componentDescA.IsSpatialComponent() && componentDescB.IsSpatialComponent() )
                    {
                        return false;
                    }

                    // Handle spatial component compare - root component takes precedence
                    if ( componentDescA.IsSpatialComponent() && componentDescB.IsSpatialComponent() )
                    {
                        if ( componentDescA.IsRootComponent() )
                        {
                            return true;
                        }
                        else if ( componentDescB.IsRootComponent() )
                        {
                            return false;
                        }
                    }

                    // Arbitrary sort based on name ID
                    return strcmp( componentDescA.m_name.c_str(), componentDescB.m_name.c_str() ) <= 0;
                };

                eastl::sort( outEntityDesc.m_components.begin(), outEntityDesc.m_components.end(), comparator );
            }

            //-------------------------------------------------------------------------
            // Read systems
            //-------------------------------------------------------------------------

            auto systemsArrayIter = entityObject.FindMember( "Systems" );
            if ( systemsArrayIter != entityObject.MemberEnd() && systemsArrayIter->value.IsArray() )
            {
                KRG_ASSERT( outEntityDesc.m_systems.empty() );

                outEntityDesc.m_systems.resize( systemsArrayIter->value.Size() );

                for ( rapidjson::SizeType i = 0; i < systemsArrayIter->value.Size(); i++ )
                {
                    if ( !ReadSystemData( ctx, systemsArrayIter->value[i], outEntityDesc.m_systems[i] ) )
                    {
                        return Error( "Failed to read system definition %u on entity (%s)", i, outEntityDesc.m_name.c_str() );
                    }
                }
            }

            //-------------------------------------------------------------------------

            ctx.m_parsingContextName.Clear();

            //-------------------------------------------------------------------------

            if ( ctx.DoesEntityExist( outEntityDesc.m_name ) )
            {
                return Error( "Duplicate entity ID detected: %s", outEntityDesc.m_name.c_str() );
            }
            else
            {
                ctx.m_entityNames.insert( TPair<StringID, bool>( outEntityDesc.m_name, true ) );
                return true;
            }
        }

        static bool ReadEntityArray( ParsingContext& ctx, Serialization::JsonValue const& entitiesArrayValue, EntityCollectionDescriptor& outCollection )
        {
            int32_t const numEntities = (int32_t) entitiesArrayValue.Size();
            outCollection.Reserve( numEntities );
            for ( int32_t i = 0; i < numEntities; i++ )
            {
                if ( !entitiesArrayValue[i].IsObject() )
                {
                    return Error( "Malformed collection file, entities array can only contain objects" );
                }

                EntityDescriptor entityDesc;
                if ( !ReadEntityData( ctx, entitiesArrayValue[i], entityDesc ) )
                {
                    return false;
                }

                outCollection.AddEntity( entityDesc );
            }

            return true;
        }
    }

    //-------------------------------------------------------------------------
    // Serializer
    //-------------------------------------------------------------------------

    bool ReadEntityDescriptor( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& entitiesObjectValue, EntityDescriptor& outEntityDesc )
    {
        if ( !entitiesObjectValue.IsObject() )
        {
            return Error( "Supplied entity json value is not an object" );
        }

        ParsingContext ctx( typeRegistry );
        return ReadEntityData( ctx, entitiesObjectValue, outEntityDesc );
    }

    bool ReadEntityCollectionFromJson( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& entitiesArrayValue, EntityCollectionDescriptor& outCollectionDesc )
    {
        if ( !entitiesArrayValue.IsArray() )
        {
            return Error( "Failed to read entity collection, json value is not an array" );
        }

        ParsingContext ctx( typeRegistry );

        if ( !ReadEntityArray( ctx, entitiesArrayValue, outCollectionDesc ) )
        {
            return false;
        }

        outCollectionDesc.GenerateSpatialAttachmentInfo();

        //-------------------------------------------------------------------------

        return true;
    }

    bool ReadEntityCollectionFromFile( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& filePath, EntityCollectionDescriptor& outCollectionDesc )
    {
        KRG_ASSERT( filePath.IsValid() );

        //-------------------------------------------------------------------------

        if ( !FileSystem::Exists( filePath ) )
        {
            return Error( "Cant read source file %s", filePath.GetFullPath().c_str() );
        }

        //-------------------------------------------------------------------------

        outCollectionDesc.Clear();

        //-------------------------------------------------------------------------

        // Load file into memory buffer
        FILE* fp = fopen( filePath.c_str(), "r" );
        fseek( fp, 0, SEEK_END );
        size_t filesize = (size_t) ftell( fp );
        fseek( fp, 0, SEEK_SET );

        Blob fileBuffer;
        fileBuffer.resize( filesize + 1 );
        size_t readLength = fread( fileBuffer.data(), 1, filesize, fp );
        fileBuffer[readLength] = '\0';
        fclose( fp );

        // Parse JSON
        //-------------------------------------------------------------------------

        rapidjson::Document entityCollectionDocument;
        rapidjson::ParseResult result = entityCollectionDocument.ParseInsitu( (char*) fileBuffer.data() );
        if ( result.IsError() )
        {
            return Error( "Failed to parse JSON: %s", GetJsonErrorMessage( result.Code() ) );
        }

        if ( !entityCollectionDocument.HasMember( "Entities" ) || !entityCollectionDocument["Entities"].IsArray() )
        {
            return Error( "Invalid format for entity collection file, missing root entities array" );
        }

        // Read Entities
        //-------------------------------------------------------------------------

        return ReadEntityCollectionFromJson( typeRegistry, entityCollectionDocument["Entities"], outCollectionDesc );
    }
}

//-------------------------------------------------------------------------
// Writing
//-------------------------------------------------------------------------

namespace KRG::EntityModel::Serializer
{
    namespace
    {
        static bool WriteEntityComponentToJson( Serialization::JsonWriter& writer, TypeSystem::TypeRegistry const& typeRegistry, ComponentDescriptor const& componentDesc )
        {
            writer.StartObject();

            writer.Key( "Name" );
            writer.String( componentDesc.m_name.c_str() );

            if ( componentDesc.m_spatialParentName.IsValid() )
            {
                writer.Key( "SpatialParent" );
                writer.String( componentDesc.m_spatialParentName.c_str() );
            }

            if ( componentDesc.m_attachmentSocketID.IsValid() )
            {
                writer.Key( "AttachmentSocketID" );
                writer.String( componentDesc.m_attachmentSocketID.c_str() );
            }

            //-------------------------------------------------------------------------

            writer.Key( "TypeData" );
            writer.StartObject();

            writer.Key( "TypeID" );
            writer.String( componentDesc.m_typeID.c_str() );

            for ( auto const& propertyDesc : componentDesc.m_properties )
            {
                writer.Key( propertyDesc.m_path.ToString().c_str() );
                writer.String( propertyDesc.m_stringValue.c_str() );
            }

            writer.EndObject();

            //-------------------------------------------------------------------------

            writer.EndObject();
            return true;
        }

        static bool WriteEntitySystemToJson( Serialization::JsonWriter& writer, TypeSystem::TypeRegistry const& typeRegistry, SystemDescriptor const& systemDesc )
        {
            if ( !systemDesc.m_typeID.IsValid() )
            {
                return Error( "Failed to write entity system desc since the system type ID was invalid." );
            }

            writer.StartObject();
            writer.Key( "TypeID" );
            writer.String( systemDesc.m_typeID.c_str() );
            writer.EndObject();
            return true;
        }
    }

    //-------------------------------------------------------------------------

    bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc, Serialization::JsonWriter& writer )
    {
        writer.StartObject();

        writer.Key( "Name" );
        writer.String( entityDesc.m_name.c_str() );

        if ( entityDesc.m_attachmentSocketID.IsValid() )
        {
            writer.Key( "AttachmentSocketID" );
            writer.String( entityDesc.m_attachmentSocketID.c_str() );
        }

        //-------------------------------------------------------------------------

        if ( !entityDesc.m_components.empty() )
        {
            writer.Key( "Components" );
            writer.StartArray();
            for ( auto const& component : entityDesc.m_components )
            {
                if ( !WriteEntityComponentToJson( writer, typeRegistry, component ) )
                {
                    return false;
                }
            }
            writer.EndArray();
        }

        //-------------------------------------------------------------------------

        if ( !entityDesc.m_systems.empty() )
        {
            writer.Key( "Systems" );
            writer.StartArray();
            for ( auto const& system : entityDesc.m_systems )
            {
                if ( !WriteEntitySystemToJson( writer, typeRegistry, system ) )
                {
                    return false;
                }
            }
            writer.EndArray();
        }

        //-------------------------------------------------------------------------

        writer.EndObject();
        return true;
    }

    bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, Entity const* pEntity, Serialization::JsonWriter& writer )
    {
        EntityDescriptor entityDesc;
        if ( !pEntity->CreateDescriptor( typeRegistry, entityDesc ) )
        {
            return false;
        }

        return WriteEntityToJson( typeRegistry, entityDesc, writer );
    }

    bool WriteEntityCollectionToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, Serialization::JsonWriter& writer )
    {
        // Write collection to document
        //-------------------------------------------------------------------------

        writer.StartObject();
        writer.Key( "Entities" );
        writer.StartArray();

        for ( auto const& entity : collection.GetEntityDescriptors() )
        {
            if ( !WriteEntityToJson( typeRegistry, entity, writer ) )
            {
                return false;
            }
        }

        writer.EndArray();
        writer.EndObject();

        return true;
    }

    bool WriteMapToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, Serialization::JsonWriter& writer )
    {
        EntityCollectionDescriptor ecd;
        if ( !map.CreateDescriptor( typeRegistry, ecd ) )
        {
            return false;
        }

        return WriteEntityCollectionToJson( typeRegistry, ecd, writer );
    }

    bool WriteEntityCollectionToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, FileSystem::Path const& outFilePath )
    {
        KRG_ASSERT( outFilePath.IsValid() );
        JsonArchiveWriter archive;
        WriteEntityCollectionToJson( typeRegistry, collection, *archive.GetWriter() );
        return archive.WriteToFile( outFilePath );
    }

    bool WriteMapToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, FileSystem::Path const& outFilePath )
    {
        EntityCollectionDescriptor ecd;
        if ( !map.CreateDescriptor( typeRegistry, ecd ) )
        {
            return false;
        }

        return WriteEntityCollectionToFile( typeRegistry, ecd, outFilePath );
    }
}
#endif