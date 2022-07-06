#pragma once
#include "Engine/_Module/API.h"
#include "System/KRG.h"
#include "System/Serialization/JSONSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    namespace FileSystem { class Path; }
    namespace TypeSystem { class TypeRegistry; }
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityDescriptor;
    class EntityMap;
    class EntityCollectionDescriptor;
}

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::EntityModel::Serializer
{
    KRG_ENGINE_API bool ReadEntityDescriptor( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& entityValue, EntityDescriptor& outEntityDesc );
    KRG_ENGINE_API bool ReadEntityCollectionFromJson( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& entitiesArrayValue, EntityCollectionDescriptor& outCollectionDesc );
    KRG_ENGINE_API bool ReadEntityCollectionFromFile( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& filePath, EntityCollectionDescriptor& outCollectionDesc );

    //-------------------------------------------------------------------------

    KRG_ENGINE_API bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc, Serialization::JsonWriter& writer );
    KRG_ENGINE_API bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, Entity const* pEntity, Serialization::JsonWriter& writer );
    KRG_ENGINE_API bool WriteEntityCollectionToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, Serialization::JsonWriter& writer );
    KRG_ENGINE_API bool WriteMapToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, Serialization::JsonWriter& writer );
    KRG_ENGINE_API bool WriteEntityCollectionToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, FileSystem::Path const& outFilePath );
    KRG_ENGINE_API bool WriteMapToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, FileSystem::Path const& outFilePath );
}
#endif