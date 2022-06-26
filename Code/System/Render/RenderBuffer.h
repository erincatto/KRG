#pragma once

#include "System/_Module/API.h"
#include "System/Render/RenderAPI.h"
#include "System/Render/RenderVertexFormats.h"
#include "System/Serialization/Serialization.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_SYSTEM_API RenderBuffer
    {
        friend class RenderDevice;

        KRG_SERIALIZE_MEMBERS( m_ID, m_slot, m_byteSize, m_byteStride, m_usage, m_type );

    public:

        enum class Type
        {
            Unknown = 0,
            Vertex,
            Index,
            Constant,
        };

        enum class Usage
        {
            GPU_only,
            CPU_and_GPU,
        };

    public:

        RenderBuffer() = default;
        ~RenderBuffer() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

        inline bool IsValid() const { return m_resourceHandle.IsValid(); }

        BufferHandle const& GetResourceHandle() const { return m_resourceHandle; }
        inline uint32_t GetNumElements() const { return m_byteSize / m_byteStride; }

    public:

        uint32_t              m_ID;
        uint32_t              m_slot = 0;
        uint32_t              m_byteSize = 0;
        uint32_t              m_byteStride = 0;
        Usage               m_usage = Usage::GPU_only;
        Type                m_type = Type::Unknown;

    protected:

        BufferHandle       m_resourceHandle;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_API VertexBuffer : public RenderBuffer
    {
        friend class RenderDevice;
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( RenderBuffer ), KRG_NVP( m_vertexFormat ) );

    public:

        VertexBuffer() : RenderBuffer() { m_type = Type::Vertex; }

    public:

        VertexFormat        m_vertexFormat = VertexFormat::StaticMesh;
    };
}