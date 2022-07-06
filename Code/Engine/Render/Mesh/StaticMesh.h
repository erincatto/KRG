#pragma once
#include "RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_API StaticMesh : public Mesh
    {
        KRG_REGISTER_RESOURCE( 'msh', "Static Mesh" );
        KRG_SERIALIZE( KRG_SERIALIZE_BASE( Mesh ) );
    };
}