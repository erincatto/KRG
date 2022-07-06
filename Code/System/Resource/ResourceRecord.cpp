#include "ResourceRecord.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceRecord::~ResourceRecord()
    {
        KRG_ASSERT( m_pResource == nullptr && !HasReferences() );
    }
}