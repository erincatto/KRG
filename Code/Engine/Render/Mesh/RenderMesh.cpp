#include "RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    Mesh::GeometrySection::GeometrySection( StringID ID, uint32_t startIndex, uint32_t numIndices )
        : m_ID( ID )
        , m_startIndex( startIndex )
        , m_numIndices( numIndices )
    {}

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void Mesh::DrawNormals( Drawing::DrawContext& drawingContext, Transform const& worldTransform ) const
    {
        KRG_UNIMPLEMENTED_FUNCTION();
    }
    #endif
}