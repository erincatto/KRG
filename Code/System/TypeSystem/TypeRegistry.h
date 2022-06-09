#pragma once

#include "ResourceInfo.h"
#include "TypeInfo.h"
#include "CoreTypeIDs.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct EnumInfo;
    struct PropertyInfo;
    class PropertyPath;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_TYPESYSTEM_API TypeRegistry : public ISystem
    {
    public:

        KRG_SYSTEM_ID( TypeRegistry );

    public:

        TypeRegistry();
        ~TypeRegistry();

        //-------------------------------------------------------------------------
        // Type Registration
        //-------------------------------------------------------------------------

        TypeInfo const* RegisterType( TypeInfo const& type );
        void UnregisterType( TypeID typeID );

        //-------------------------------------------------------------------------
        // Type Info
        //-------------------------------------------------------------------------

        // Returns the type information for a given type ID
        TypeInfo const* GetTypeInfo( TypeID typeID ) const;

        // Returns the size of a given type
        size_t GetTypeByteSize( TypeID typeID ) const;

        // Returns the resolved property info for a given path
        PropertyInfo const* ResolvePropertyPath( TypeInfo const* pTypeInfo, PropertyPath const& pathID ) const;

        // Does a given type derive from a given parent type
        bool IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const;

        // Return all types matching specified type metadata
        TVector<TypeInfo const*> GetAllTypesWithMatchingMetadata( TBitFlags<TypeInfoMetaData> metadataFlags ) const;

        // Return all known types
        TVector<TypeInfo const*> GetAllTypes( bool includeAbstractTypes = true, bool sortAlphabetically = false ) const;

        // Return all types that derived from a specified type
        TVector<TypeInfo const*> GetAllDerivedTypes( TypeID parentTypeID, bool includeParentTypeInResults = false, bool includeAbstractTypes = true, bool sortAlphabetically = false ) const;

        // Get all the types that this type is allowed to be cast to
        TInlineVector<TypeID, 5> GetAllCastableTypes( IRegisteredType const* pType ) const;

        // Are these two types in the same derivation chain (i.e. does either derive from the other )
        bool AreTypesInTheSameHierarchy( TypeID typeA, TypeID typeB ) const;

        // Are these two types in the same derivation chain (i.e. does either derive from the other )
        bool AreTypesInTheSameHierarchy( TypeInfo const* pTypeInfoA, TypeInfo const* pTypeInfoB ) const;

        //-------------------------------------------------------------------------
        // Enums
        //-------------------------------------------------------------------------

        EnumInfo const* RegisterEnum( EnumInfo const& type );
        void UnregisterEnum( TypeID typeID );
        EnumInfo const* GetEnumInfo( TypeID enumID ) const;

        //-------------------------------------------------------------------------
        // Resources
        //-------------------------------------------------------------------------

        inline THashMap<TypeID, ResourceInfo> const& GetRegisteredResourceTypes() const { return m_registeredResourceTypes; }
        void RegisterResourceTypeID( ResourceInfo const& resourceInfo );
        void UnregisterResourceTypeID( TypeID typeID );
        bool IsRegisteredResourceType( ResourceTypeID resourceTypeID ) const;
        ResourceInfo const* GetResourceInfoForType( TypeID typeID ) const;
        ResourceInfo const* GetResourceInfoForResourceType( ResourceTypeID resourceTypeID ) const;

    private:

        THashMap<TypeID, TypeInfo*>             m_registeredTypes;
        THashMap<TypeID, EnumInfo*>             m_registeredEnums;
        THashMap<TypeID, ResourceInfo>          m_registeredResourceTypes;
    };
}