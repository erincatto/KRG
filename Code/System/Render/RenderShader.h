#pragma once

#include "RenderBuffer.h"
#include "System/Resource/IResource.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_API Shader : public Resource::IResource
        {
            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

            KRG_SERIALIZE_MEMBERS( m_cbuffers, m_resourceBindings, m_byteCode );
            KRG_REGISTER_RESOURCE( 'shdr', "Render Shader" );

        public:

            struct ResourceBinding
            {
                KRG_SERIALIZE_MEMBERS( m_ID, m_slot );

                ResourceBinding() : m_slot( 0 ) {}
                ResourceBinding( uint32_t ID, uint32_t slot ) : m_ID( ID ), m_slot( slot ) {}

                uint32_t                 m_ID;
                uint32_t                 m_slot;
            };

        public:

            virtual ~Shader() { KRG_ASSERT( !m_shaderHandle.IsValid() ); }

            inline PipelineStage GetPipelineStage() const { return m_pipelineStage; }

            inline ShaderHandle const& GetShaderHandle() const { return m_shaderHandle; }
            inline uint32_t GetNumConstBuffers() const { return (uint32_t) m_cbuffers.size(); }
            inline RenderBuffer const& GetConstBuffer( uint32_t i ) const { KRG_ASSERT( i < m_cbuffers.size() ); return m_cbuffers[i]; }

            inline bool operator==( Shader const& rhs ) const { return m_shaderHandle.m_pData == rhs.m_shaderHandle.m_pData; }
            inline bool operator!=( Shader const& rhs ) const { return m_shaderHandle.m_pData != rhs.m_shaderHandle.m_pData; }

        protected:

            Shader( PipelineStage stage );
            Shader( PipelineStage stage, uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

        protected:

            ShaderHandle                        m_shaderHandle;
            TVector<uint8_t>                      m_byteCode;
            TVector<RenderBuffer>               m_cbuffers;
            TVector<ResourceBinding>            m_resourceBindings;
            PipelineStage                       m_pipelineStage;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_API PixelShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'psdr', "Pixel Shader" );

        public:

            PixelShader() : Shader( PipelineStage::Pixel ) {}
            PixelShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_API GeometryShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'gsdr', "Geometry Shader");

        public:

            GeometryShader() : Shader( PipelineStage::Pixel ) {}
            GeometryShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_API VertexShader : public Shader
        {
            KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Shader ), m_vertexLayoutDesc );
            KRG_REGISTER_RESOURCE( 'vsdr', "Vertex Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

        public:

            VertexShader() : Shader( PipelineStage::Vertex ) {}
            VertexShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers, VertexLayoutDescriptor const& vertexLayoutDesc );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
            inline VertexLayoutDescriptor const& GetVertexLayoutDesc() const { return m_vertexLayoutDesc; }

        private:

            VertexLayoutDescriptor m_vertexLayoutDesc;
        };

        class KRG_SYSTEM_API ComputeShader : public Shader
        {
            KRG_REGISTER_RESOURCE( 'csdr', "Compute Shader" );

            friend class RenderDevice;
            friend class ShaderCompiler;
            friend class ShaderLoader;

        public:

            ComputeShader() : Shader( PipelineStage::Compute ) {}
            ComputeShader( uint8_t const* pByteCode, size_t const byteCodeSize, TVector<RenderBuffer> const& constBuffers );

            virtual bool IsValid() const override { return m_shaderHandle.IsValid(); }
        };
    }
}