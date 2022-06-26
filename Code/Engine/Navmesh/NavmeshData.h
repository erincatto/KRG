#pragma once

#include "Engine/_Module/API.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_ENGINE_API NavmeshData : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'nav', "Navmesh");
        friend class NavmeshGenerator;
        friend class NavmeshLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_graphImage ) );

    public:

        virtual bool IsValid() const override { return !m_graphImage.empty(); }
        inline TVector<uint8_t> const& GetGraphImage() const { return m_graphImage; }

    private:

        TVector<uint8_t>   m_graphImage;
    };
}