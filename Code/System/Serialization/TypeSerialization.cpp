#include "TypeSerialization.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/Log.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------
// Descriptors
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Serialization
{
    // Type descriptor reader needs to support both nested and unnested formats as it needs to read the outputs from both descriptor serialization and type model serialization
    struct TypeDescriptorReader
    {
        static bool ReadArrayDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pRootTypeInfo, PropertyInfo const* pArrayPropertyInfo, RapidJsonValue const& arrayValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix )
        {
            KRG_ASSERT( pArrayPropertyInfo != nullptr && arrayValue.IsArray() );

            int32_t const numElements = (int32_t) arrayValue.Size();
            for ( int32_t i = 0; i < numElements; i++ )
            {
                if ( arrayValue[i].IsArray() )
                {
                    // We dont support arrays of arrays
                    KRG_LOG_ERROR( "TypeSystem", "We dont support arrays of arrays" );
                    return false;
                }
                else if ( arrayValue[i].IsObject() )
                {
                    if ( CoreTypeRegistry::IsCoreType( pArrayPropertyInfo->m_typeID ) )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, object declared for core type property: %s", pArrayPropertyInfo->m_ID.c_str() );
                        return false;
                    }

                    auto pArrayPropertyTypeInfo = typeRegistry.GetTypeInfo( pArrayPropertyInfo->m_typeID );
                    String const newPrefix = String( String::CtorSprintf(), "%s%d/", propertyPathPrefix.c_str(), i );
                    if ( !ReadTypeDescriptor( typeRegistry, pRootTypeInfo, pArrayPropertyTypeInfo, arrayValue[i], outPropertyValues, newPrefix ) )
                    {
                        return false;
                    }
                }
                else // Add regular property value
                {
                    if ( !CoreTypeRegistry::IsCoreType( pArrayPropertyInfo->m_typeID ) )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, only core type properties are allowed to be directly declared: %s", pArrayPropertyInfo->m_ID.c_str() );
                        return false;
                    }

                    if ( !arrayValue[i].IsString() )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, Core type values must be strings, property (%s) has invalid value: %s", pArrayPropertyInfo->m_ID.c_str(), arrayValue[i].GetString() );
                        return false;
                    }

                    auto const propertyPath = PropertyPath( String( String::CtorSprintf(), "%s%d", propertyPathPrefix.c_str(), i ) );
                    auto pPropertyInfo = typeRegistry.ResolvePropertyPath( pRootTypeInfo, propertyPath );
                    if ( pPropertyInfo != nullptr )
                    {
                        outPropertyValues.push_back( PropertyDescriptor( typeRegistry, propertyPath, *pPropertyInfo, arrayValue[i].GetString() ) );
                    }
                }
            }

            return true;
        }

        static bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pRootTypeInfo, TypeInfo const* pTypeInfo, RapidJsonValue const& typeObjectValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix = String() )
        {
            // Read properties
            //-------------------------------------------------------------------------

            for ( auto itr = typeObjectValue.MemberBegin(); itr != typeObjectValue.MemberEnd(); ++itr )
            {
                StringID const propertyID( itr->name.GetString() );
                PropertyInfo const* pPropertyInfo = pTypeInfo->GetPropertyInfo( propertyID );
                if ( pPropertyInfo == nullptr )
                {
                    continue;
                }

                if ( itr->value.IsArray() )
                {
                    String const newPrefix = String( String::CtorSprintf(), "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                    ReadArrayDescriptor( typeRegistry, pRootTypeInfo, pPropertyInfo, itr->value, outPropertyValues, newPrefix );
                }
                else if ( itr->value.IsObject() )
                {
                    if ( CoreTypeRegistry::IsCoreType( pPropertyInfo->m_typeID ) )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, object declared for core type property: %s", itr->name.GetString() );
                        return false;
                    }
                    
                    auto pPropertyTypeInfo = typeRegistry.GetTypeInfo( pPropertyInfo->m_typeID );
                    String const newPrefix = String( String::CtorSprintf(), "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                    if ( !ReadTypeDescriptor( typeRegistry, pRootTypeInfo, pPropertyTypeInfo, itr->value, outPropertyValues, newPrefix ) )
                    {
                        return false;
                    }
                }
                else // Regular core type property
                {
                    if ( !CoreTypeRegistry::IsCoreType( pPropertyInfo->m_typeID ) && !pPropertyInfo->IsEnumProperty() )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, only core type properties are allowed to be directly declared: %s", itr->name.GetString() );
                        return false;
                    }

                    if ( !itr->value.IsString() )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Malformed json detected, Core type values must be strings, invalid value detected: %s", itr->name.GetString() );
                        return false;
                    }

                    PropertyPath const propertyPath( String( String::CtorSprintf(), "%s%s", propertyPathPrefix.c_str(), itr->name.GetString() ) );

                    auto pResolvedPropertyInfo = typeRegistry.ResolvePropertyPath( pRootTypeInfo, propertyPath );
                    if ( pResolvedPropertyInfo != nullptr )
                    {
                        outPropertyValues.push_back( PropertyDescriptor( typeRegistry, propertyPath, *pResolvedPropertyInfo, itr->value.GetString() ) );
                    }
                }
            }

            return true;
        }
    };

    bool ReadTypeDescriptorFromJSON( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, TypeDescriptor& outDesc )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        // Get type info
        //-------------------------------------------------------------------------

        auto const typeIDIter = typeObjectValue.FindMember( s_typeIDKey );
        if ( typeIDIter == typeObjectValue.MemberEnd() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        TypeID const typeID( typeIDIter->value.GetString() );
        auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
        if ( pTypeInfo == nullptr )
        {
            KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
            return false;
        }

        // Read descriptor
        //-------------------------------------------------------------------------

        outDesc.m_typeID = TypeID( typeIDIter->value.GetString() );
        return TypeDescriptorReader::ReadTypeDescriptor( typeRegistry, pTypeInfo, pTypeInfo, typeObjectValue, outDesc.m_properties );
    }

    //-------------------------------------------------------------------------

    // Type descriptor serialization will collapse all properties into a single list per type, using the property paths as property names 
    struct TypeDescriptorWriter
    {
        static void WriteProperty( RapidJsonWriter& writer, PropertyDescriptor const& propertyDesc )
        {
            writer.Key( propertyDesc.m_path.ToString().c_str() );
            writer.Key( propertyDesc.m_stringValue.c_str() );
        }

        static void WriteStructure( RapidJsonWriter& writer, TypeDescriptor const& typeDesc )
        {
            writer.StartObject();

            // Every type has to have a type ID
            writer.Key( s_typeIDKey );
            writer.String( typeDesc.m_typeID.c_str() );

            // Write all property values
            for ( auto const& propertyValue : typeDesc.m_properties )
            {
                WriteProperty( writer, propertyValue );
            }

            writer.EndObject();
        }
    };

    void WriteTypeDescriptorToJSON( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& typeDesc )
    {
        KRG_ASSERT( typeDesc.IsValid() );
        TypeDescriptorWriter::WriteStructure( writer, typeDesc );
    }
}

//-------------------------------------------------------------------------
// Native
//-------------------------------------------------------------------------

namespace KRG::Serialization
{
    struct NativeTypeReader
    {
        template<typename T>
        static void SetPropertyValue( void* pAddress, T value )
        {
            *( (T*) pAddress ) = value;
        }

        static bool ReadCoreType( TypeRegistry const& typeRegistry, PropertyInfo const& propInfo, RapidJsonValue const& typeValue, void* pPropertyDataAddress )
        {
            KRG_ASSERT( pPropertyDataAddress != nullptr );

            if ( typeValue.IsString() )
            {
                String const valueString = String( typeValue.GetString() );
                Conversion::ConvertStringToNativeType( typeRegistry, propInfo, valueString, pPropertyDataAddress );
            }
            else if ( typeValue.IsBool() )
            {
                KRG_ASSERT( propInfo.m_typeID == CoreTypeID::Bool );
                SetPropertyValue( pPropertyDataAddress, typeValue.GetBool() );
            }
            else if ( typeValue.IsInt64() || typeValue.IsUint64() )
            {
                if ( propInfo.m_typeID == CoreTypeID::Uint8 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint8_t) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Int8 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int8_t) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Uint16 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint16_t) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Int16 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int16_t) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Uint32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint32_t) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Int32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int32_t) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Uint64 )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Int64 )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetInt64() );
                }
                else // Invalid JSON data encountered
                {
                    KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                    return false;
                }
            }
            else if ( typeValue.IsDouble() )
            {
                if ( propInfo.m_typeID == CoreTypeID::Float )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetFloat() );
                }
                else if ( propInfo.m_typeID == CoreTypeID::Double )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetDouble() );
                }
                else // Invalid JSON data encountered
                {
                    KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                    return false;
                }
            }
            else // Invalid JSON data encountered
            {
                KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                return false;
            }

            return true;
        }

        //-------------------------------------------------------------------------

        static bool ReadType( TypeRegistry const& typeRegistry, RapidJsonValue const& currentJsonValue, TypeID typeID, IRegisteredType* pTypeData )
        {
            KRG_ASSERT( !IsCoreType( typeID ) );

            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            if ( pTypeInfo == nullptr )
            {
                KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
                return false;
            }

            KRG_ASSERT( currentJsonValue.IsObject() );
            auto const typeIDValueIter = currentJsonValue.FindMember( s_typeIDKey );
            KRG_ASSERT( typeIDValueIter != currentJsonValue.MemberEnd() );
            TypeID const actualTypeID( typeIDValueIter->value.GetString() );

            // If you hit this the type in the JSON file and the type you are trying to deserialize do not match
            if ( typeID != actualTypeID && !typeRegistry.IsTypeDerivedFrom( actualTypeID, typeID ) )
            {
                KRG_LOG_ERROR( "TypeSystem", "Type mismatch, expected %s, encountered %s", typeID.c_str(), actualTypeID.c_str() );
                return false;
            }

            //-------------------------------------------------------------------------

            for ( auto const& propInfo : pTypeInfo->m_properties )
            {
                // Read Arrays
                auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeData );
                if ( propInfo.IsArrayProperty() )
                {
                    // Try get serialized value
                    const char* pPropertyName = propInfo.m_ID.c_str();
                    auto const propertyNameIter = currentJsonValue.FindMember( pPropertyName );
                    if ( propertyNameIter == currentJsonValue.MemberEnd() )
                    {
                        continue;
                    }

                    KRG_ASSERT( propertyNameIter->value.IsArray() );
                    auto jsonArrayValue = propertyNameIter->value.GetArray();
                    size_t const numJSONArrayElements = jsonArrayValue.Size();

                    // Static array
                    if ( propInfo.IsStaticArrayProperty() )
                    {
                        if ( propInfo.m_arraySize < numJSONArrayElements )
                        {
                            KRG_LOG_ERROR( "TypeSystem", "Static array size mismatch for %s, expected maximum %d elements, encountered %d elements", propInfo.m_size, propInfo.m_size, (int32_t) numJSONArrayElements );
                            return false;
                        }

                        uint8_t* pArrayElementAddress = reinterpret_cast<uint8_t*>( pPropertyDataAddress );
                        for ( auto i = 0; i < numJSONArrayElements; i++ )
                        {
                            if ( !ReadProperty( typeRegistry, jsonArrayValue[i], propInfo, pArrayElementAddress ) )
                            {
                                return false;
                            }
                            pArrayElementAddress += propInfo.m_arrayElementSize;
                        }
                    }
                    else // Dynamic array
                    {
                        // If we have less elements in the json array than in the current type, clear the array as we will resize the array appropriately as part of reading the values
                        size_t const currentArraySize = pTypeInfo->m_pTypeHelper->GetArraySize( pTypeData, propInfo.m_ID );
                        if ( numJSONArrayElements < currentArraySize )
                        {
                            pTypeInfo->m_pTypeHelper->ClearArray( pTypeData, propInfo.m_ID );
                        }

                        // Do the traversal backwards to only allocate once
                        for ( int32_t i = (int32_t) ( numJSONArrayElements - 1 ); i >= 0; i-- )
                        {
                            auto pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetArrayElementDataPtr( pTypeData, propInfo.m_ID, i );
                            if ( !ReadProperty( typeRegistry, jsonArrayValue[i], propInfo, pArrayElementAddress ) )
                            {
                                return false;
                            }
                        }
                    }
                }
                else // Non-array type
                {
                    // Try get serialized value
                    const char* pPropertyName = propInfo.m_ID.c_str();
                    auto const propertyValueIter = currentJsonValue.FindMember( pPropertyName );
                    if ( propertyValueIter == currentJsonValue.MemberEnd() )
                    {
                        continue;
                    }

                    if ( !ReadProperty( typeRegistry, propertyValueIter->value, propInfo, pPropertyDataAddress ) )
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        static bool ReadProperty( TypeRegistry const& typeRegistry, RapidJsonValue const& currentJsonValue, PropertyInfo const& propertyInfo, void* pPropertyInstance )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                String const typeName = propertyInfo.m_typeID.c_str();
                return ReadCoreType( typeRegistry, propertyInfo, currentJsonValue, pPropertyInstance );
            }
            else // Complex Type
            {
                return ReadType( typeRegistry, currentJsonValue, propertyInfo.m_typeID, (IRegisteredType*) pPropertyInstance );
            }
        }
    };

    bool ReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, IRegisteredType* pTypeInstance )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        if ( !typeObjectValue.HasMember( s_typeIDKey ) )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        return NativeTypeReader::ReadType( typeRegistry, typeObjectValue, pTypeInstance->GetTypeID(), pTypeInstance );
    }

    bool ReadNativeTypeFromString( TypeRegistry const& typeRegistry, String const& jsonString, IRegisteredType* pTypeInstance )
    {
        KRG_ASSERT( !jsonString.empty() && pTypeInstance != nullptr );

        JsonReader reader;
        reader.ReadFromString( jsonString.c_str() );
        return ReadNativeType( typeRegistry, reader.GetDocument(), pTypeInstance );
    }

    //-------------------------------------------------------------------------

    struct NativeTypeWriter
    {
        static void WriteType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, TypeID typeID, IRegisteredType const* pTypeInstance, bool createJsonObject = true )
        {
            KRG_ASSERT( !IsCoreType( typeID ) );
            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            if ( createJsonObject )
            {
                writer.StartObject();
            }

            // Every type has to have a type ID
            writer.Key( s_typeIDKey );
            writer.String( typeID.c_str() );

            //-------------------------------------------------------------------------

            for ( auto const& propInfo : pTypeInfo->m_properties )
            {
                // Write Key
                const char* pPropertyName = propInfo.m_ID.c_str();
                KRG_ASSERT( pPropertyName != nullptr );
                writer.Key( pPropertyName );

                // Write Value
                auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeInstance );
                if ( propInfo.IsArrayProperty() )
                {
                    size_t const elementByteSize = typeRegistry.GetTypeByteSize( propInfo.m_typeID );
                    KRG_ASSERT( elementByteSize > 0 );

                    writer.StartArray();

                    size_t numArrayElements = 0;
                    uint8_t const* pElementAddress = nullptr;

                    // Static array
                    if ( propInfo.IsStaticArrayProperty() )
                    {
                        numArrayElements = propInfo.m_size / elementByteSize;
                        pElementAddress = propInfo.GetPropertyAddress<uint8_t>( pTypeInstance );
                    }
                    else // Dynamic array
                    {
                        TVector<uint8_t> const& dynamicArray = *propInfo.GetPropertyAddress< TVector<uint8_t> >( pTypeInstance );
                        size_t const arrayByteSize = dynamicArray.size();
                        numArrayElements = arrayByteSize / elementByteSize;
                        pElementAddress = dynamicArray.data();
                    }

                    // Write array elements
                    for ( auto i = 0; i < numArrayElements; i++ )
                    {
                        WriteProperty( typeRegistry, writer, scratchBuffer, propInfo, pElementAddress );
                        pElementAddress += elementByteSize;
                    }

                    writer.EndArray();
                }
                else
                {
                    WriteProperty( typeRegistry, writer, scratchBuffer, propInfo, pPropertyDataAddress );
                }
            }

            if ( createJsonObject )
            {
                writer.EndObject();
            }
        }

        static void WriteProperty( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, PropertyInfo const& propertyInfo, void const* pPropertyInstance )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                Conversion::ConvertNativeTypeToString( typeRegistry, propertyInfo, pPropertyInstance, scratchBuffer );
                writer.String( scratchBuffer.c_str() );
            }
            else
            {
                WriteType( typeRegistry, writer, scratchBuffer, propertyInfo.m_typeID, (IRegisteredType*) pPropertyInstance );
            }
        }
    };

    void WriteNativeType( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, RapidJsonWriter& writer )
    {
        String scratchBuffer;
        scratchBuffer.reserve( 255 );
        NativeTypeWriter::WriteType( typeRegistry, writer, scratchBuffer, pTypeInstance->GetTypeID(), pTypeInstance );
    }

    void WriteNativeTypeContents( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, RapidJsonWriter& writer )
    {
        String scratchBuffer;
        scratchBuffer.reserve( 255 );
        NativeTypeWriter::WriteType( typeRegistry, writer, scratchBuffer, pTypeInstance->GetTypeID(), pTypeInstance, false );
    }

    void WriteNativeTypeToString( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, String& outString )
    {
        JsonWriter writer;
        WriteNativeType( typeRegistry, pTypeInstance, *writer.GetWriter() );
        outString = writer.GetStringBuffer().GetString();
    }

    IRegisteredType* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue )
    {
        auto const typeIDIter = typeObjectValue.FindMember( s_typeIDKey );
        if ( typeIDIter == typeObjectValue.MemberEnd() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return nullptr;
        }

        TypeID const typeID( typeIDIter->value.GetString() );
        auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
        if ( pTypeInfo == nullptr )
        {
            KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
            return nullptr;
        }

        IRegisteredType* pTypeInstance = pTypeInfo->m_pTypeHelper->CreateType();

        if ( !ReadNativeType( typeRegistry, typeObjectValue, pTypeInstance ) )
        {
            KRG::Delete( pTypeInstance );
            return nullptr;
        }

        return pTypeInstance;
    }
}

//-------------------------------------------------------------------------
// Reader / Writer
//-------------------------------------------------------------------------

namespace KRG::Serialization
{
    TypeReader::TypeReader( TypeSystem::TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void TypeReader::OnFileReadSuccess()
    {
        if ( m_document.IsArray() )
        {
            m_numSerializedTypes = m_document.Size();
        }
        else
        {
            m_numSerializedTypes = m_document.IsObject() ? 1 : 0;
        }
    }

    void TypeReader::Reset()
    {
        JsonReader::Reset();
        m_numSerializedTypes = 0;
        m_deserializedTypeIdx = 0;
    }

    RapidJsonValue const& TypeReader::GetObjectValueToBeDeserialized()
    {
        KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

        if ( m_document.IsArray() )
        {
            KRG_ASSERT( m_document.IsArray() );
            return m_document[m_deserializedTypeIdx++];
        }
        else
        {
            return m_document;
        }
    }

    //-------------------------------------------------------------------------

    TypeWriter::TypeWriter( TypeSystem::TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    void TypeWriter::Reset()
    {
        JsonWriter::Reset();
        m_numTypesSerialized = 0;
    }

    void TypeWriter::PreSerializeType()
    {
        if ( m_numTypesSerialized == 1 )
        {
            String const firstValueSerialized = m_stringBuffer.GetString();

            //-------------------------------------------------------------------------

            m_stringBuffer.Clear();
            m_writer.StartArray();
            m_writer.RawValue( firstValueSerialized.c_str(), firstValueSerialized.length(), rapidjson::Type::kObjectType );
        }
    }

    void TypeWriter::FinalizeSerializedData()
    {
        if ( m_numTypesSerialized > 1 )
        {
            m_writer.EndArray();
        }
    }
}
#endif