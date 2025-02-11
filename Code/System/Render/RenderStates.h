#pragma once

#include "System/Math/Math.h"
#include "RenderAPI.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        //-------------------------------------------------------------------------

        struct RasterizerState
        {
            friend class RenderDevice;

            ~RasterizerState() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

            inline bool IsValid() const { return m_resourceHandle.IsValid(); }

            RasterizerStateHandle const& GetResourceHandle() const { return m_resourceHandle; }

        public:

            CullMode                m_cullMode = CullMode::BackFace;
            FillMode                m_fillMode = FillMode::Solid;
            WindingMode             m_windingMode = WindingMode::CounterClockwise;
            bool                    m_scissorCulling = false;

        private:

            RasterizerStateHandle          m_resourceHandle;
        };

        //-------------------------------------------------------------------------

        struct BlendState
        {
            friend class RenderDevice;

            ~BlendState() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

            inline bool IsValid() const { return m_resourceHandle.IsValid(); }

            BlendStateHandle const& GetResourceHandle() const { return m_resourceHandle; }

        public:

            BlendValue              m_srcValue = BlendValue::One;
            BlendValue              m_dstValue = BlendValue::Zero;
            BlendOp                 m_blendOp = BlendOp::Add;
            BlendValue              m_srcAlphaValue = BlendValue::One;
            BlendValue              m_dstAlphaValue = BlendValue::Zero;
            BlendOp                 m_blendOpAlpha = BlendOp::Add;
            bool                    m_blendEnable = false;

        private:

            BlendStateHandle          m_resourceHandle;
        };
    }
}