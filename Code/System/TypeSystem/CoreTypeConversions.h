#pragma once

#include "System/_Module/API.h"
#include "PropertyInfo.h"
#include "System/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class TypeRegistry;
}

//-------------------------------------------------------------------------
// Core Type Serialization
//-------------------------------------------------------------------------
// Basic serialization of core types to/from string and binary representations
// Primarily used to serialize core type properties
// Each core type needs to implement a serializer function specializations
// This also handles enums and bitflags as core types

namespace KRG::TypeSystem::Conversion
{
    KRG_SYSTEM_API bool ConvertStringToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, void* pValue );
    KRG_SYSTEM_API bool ConvertNativeTypeToString( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, String& strValue );
    KRG_SYSTEM_API bool ConvertBinaryToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, TVector<uint8_t> const& byteArray, void* pValue );
    KRG_SYSTEM_API bool ConvertNativeTypeToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, TVector<uint8_t>& byteArray );
    KRG_SYSTEM_API bool ConvertStringToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, TVector<uint8_t>& byteArray );
    KRG_SYSTEM_API bool IsValidStringValueForType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue );

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bool ConvertStringToNativeType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, void* pValue )
    {
        return ConvertStringToNativeType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, pValue );
    }

    KRG_FORCE_INLINE bool ConvertNativeTypeToString( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, String& strValue )
    {
        return ConvertNativeTypeToString( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, strValue );
    }

    KRG_FORCE_INLINE bool ConvertBinaryToNativeType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, TVector<uint8_t> const& byteArray, void* pValue )
    {
        return ConvertBinaryToNativeType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, byteArray, pValue );
    }

    KRG_FORCE_INLINE bool ConvertNativeTypeToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, TVector<uint8_t>& byteArray )
    {
        return ConvertNativeTypeToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, byteArray );
    }

    KRG_FORCE_INLINE bool ConvertStringToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, TVector<uint8_t>& byteArray )
    {
        return ConvertStringToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, byteArray );
    }

    KRG_FORCE_INLINE bool IsValidStringValueForType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue )
    {
        return IsValidStringValueForType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue );
    }

    //-------------------------------------------------------------------------

    // Convert a comma separated string of floats into an array of floats
    KRG_SYSTEM_API void StringToFloatArray( String const& str, int32_t const numFloats, float* pFloats );

    // Convert an array of floats into a comma separated string of floats
    KRG_SYSTEM_API void FloatArrayToString( float const* pFloats, int32_t const numFloats, String& strValue );

    // Convert a comma separated string of ints into an array of ints
    KRG_SYSTEM_API void StringToIntArray( String const& str, int32_t const numInts, int32_t* pInts );

    // Convert an array of ints into a comma separated string of ints
    KRG_SYSTEM_API void IntArrayToString( int32_t const* pInts, int32_t const numInts, String& strValue );
}