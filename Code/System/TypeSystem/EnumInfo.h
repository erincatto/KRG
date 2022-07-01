#pragma once

#include "TypeID.h"
#include "CoreTypeIDs.h"
#include "System/Log.h"
#include "System/Types/HashMap.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct EnumInfo
    {
        inline size_t GetNumValues() const
        {
            return m_constants.size();
        }

        inline bool IsValidValue( StringID label ) const
        {
            auto const iter = m_constants.find( label );
            return iter != m_constants.end();
        }

        inline int64_t GetConstantValue( StringID label ) const
        {
            auto const iter = m_constants.find( label );
            if ( iter != m_constants.end() )
            {
                return iter->second;
            }
            else // Flag error and return first valid value
            {
                KRG_LOG_ERROR( "Serialization", "Invalid enum constant value (%s) for enum (%s)", label.c_str(), m_ID.c_str() );
                return m_constants.begin()->second;
            }
        }

        inline bool TryGetConstantValue( StringID label, int64_t& outValue ) const
        {
            auto const iter = m_constants.find( label );
            if ( iter != m_constants.end() )
            {
                outValue = iter->second;
                return true;
            }

            return false;
        }

        inline StringID GetConstantLabel( int64_t value ) const
        {
            for ( auto const& pair : m_constants )
            {
                if ( pair.second == value )
                {
                    return pair.first;
                }
            }

            KRG_UNREACHABLE_CODE();
            return StringID();
        }

        inline bool TryGetConstantLabel( int64_t value, StringID& outValue ) const
        {
            for ( auto const& pair : m_constants )
            {
                if ( pair.second == value )
                {
                    outValue = pair.first;
                    return true;
                }
            }

            return false;
        }

    public:

        TypeID                                              m_ID;
        CoreTypeID                                          m_underlyingType;
        THashMap<StringID, int64_t>                         m_constants;
    };
}
