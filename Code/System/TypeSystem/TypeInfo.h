#pragma once

#include "PropertyInfo.h"
#include "ITypeHelper.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    enum class TypeInfoMetaData
    {
        Abstract,
        Entity,
        EntitySystem,
        EntityComponent,
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_TYPESYSTEM_API TypeInfo
    {

    public:

        TypeInfo() = default;

        inline char const* GetTypeName() const { return m_ID.ToStringID().c_str(); }

        #if KRG_DEVELOPMENT_TOOLS
        inline char const* GetFriendlyTypeName() const { return m_friendlyName.c_str(); }
        inline char const* GetCategoryName() const { return m_category.c_str(); }
        #endif

        bool IsAbstractType() const { return m_metadata.IsFlagSet( TypeInfoMetaData::Abstract ); }

        bool IsDerivedFrom( TypeID const parentTypeID ) const;

        template<typename T>
        inline bool IsDerivedFrom() const { return IsDerivedFrom( T::GetStaticTypeID() ); }

        inline IRegisteredType const* GetDefaultInstance() const { return m_pTypeHelper->GetDefaultTypeInstancePtr(); }

        // Properties
        //-------------------------------------------------------------------------

        PropertyInfo const* GetPropertyInfo( StringID propertyID ) const;

        // Function declaration for generated property registration functions
        template<typename T>
        void RegisterProperties( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_UNIMPLEMENTED_FUNCTION(); // Default implementation should never be called
        }

    public:

        TypeID                                  m_ID;
        int32_t                                 m_size = -1;
        int32_t                                 m_alignment = -1;
        TBitFlags<TypeInfoMetaData>             m_metadata;
        ITypeHelper*                            m_pTypeHelper = nullptr;
        TVector<TypeInfo const*>                m_parentTypes;
        TVector<PropertyInfo>                   m_properties;
        THashMap<StringID, int32_t>             m_propertyMap;

        #if KRG_DEVELOPMENT_TOOLS
        bool                                    m_isForDevelopmentUseOnly = false;      // Whether this property only exists in development builds
        String                                  m_friendlyName;
        String                                  m_category;
        #endif
    };
}

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T>
    bool IsOfType( IRegisteredType const* pType )
    {
        KRG_ASSERT( pType != nullptr );
        return pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() );
    }

    // This is a assumed safe cast, it will validate the cast only in dev builds. Doesnt accept null arguments
    template<typename T>
    T* Cast( IRegisteredType* pType )
    {
        KRG_ASSERT( pType != nullptr );
        KRG_ASSERT( pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) );
        return static_cast<T*>( pType );
    }

    // This is a assumed safe cast, it will validate the cast only in dev builds. Doesnt accept null arguments
    template<typename T>
    T const* Cast( IRegisteredType const* pType )
    {
        KRG_ASSERT( pType != nullptr );
        KRG_ASSERT( pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) );
        return static_cast<T const*>( pType );
    }

    // This will try to cast to the specified type but can fail. Also accepts null arguments
    template<typename T>
    T* TryCast( IRegisteredType* pType )
    {
        if ( pType != nullptr && pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) )
        {
            return static_cast<T*>( pType );
        }

        return nullptr;
    }

    // This will try to cast to the specified type but can fail. Also accepts null arguments
    template<typename T>
    T const* TryCast( IRegisteredType const* pType )
    {
        if ( pType != nullptr && pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) )
        {
            return static_cast<T const*>( pType );
        }

        return nullptr;
    }
}