#pragma once

#include "System/_Module/API.h"
#include "RenderAPI.h"
#include "System/Resource/IResource.h"
#include "System/Serialization/BinarySerialization.h"
#include "System/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    enum class TextureFormat : uint8_t
    {
        Raw,
        DDS,
    };

    //-------------------------------------------------------------------------

    struct SamplerState
    {
        friend class RenderDevice;

        inline bool IsValid() const { return m_resourceHandle.IsValid(); }

        SamplerStateHandle const& GetResourceHandle() const { return m_resourceHandle; }

    public:

        TextureFiltering        m_filtering = TextureFiltering::MinMagMipLinear;
        TextureAddressMode      m_addressModeU = TextureAddressMode::Wrap;
        TextureAddressMode      m_addressModeV = TextureAddressMode::Wrap;
        TextureAddressMode      m_addressModeW = TextureAddressMode::Wrap;
        Float4                  m_borderColor = Float4(0.0f);
        uint32_t                  m_maxAnisotropyValue = 1;
        float                   m_LODBias = 0;
        float                   m_minLOD = -FLT_MAX;
        float                   m_maxLOD = FLT_MAX;

    private:

        SamplerStateHandle      m_resourceHandle;
    };

    //-------------------------------------------------------------------------
    // Texture
    //-------------------------------------------------------------------------
    // Abstraction for a render texture resource

    class KRG_SYSTEM_API Texture : public Resource::IResource
    {
        friend class RenderDevice;
        friend class TextureCompiler;
        friend class TextureLoader;

        KRG_REGISTER_RESOURCE( 'txtr', "Render Texture" );
        KRG_SERIALIZE( m_format, m_rawData );

    public:

        Texture() = default;
        Texture( Int2 const& dimensions ) : m_dimensions( dimensions ) {}

        virtual bool IsValid() const override { return m_textureHandle.IsValid(); }
        inline Int2 const& GetDimensions() const { return m_dimensions; }

        inline bool operator==( Texture const& rhs ) const { return m_shaderResourceView == m_shaderResourceView; }
        inline bool operator!=( Texture const& rhs ) const { return m_shaderResourceView != m_shaderResourceView; }

        // Resource Views
        //-------------------------------------------------------------------------

        inline bool HasShaderResourceView() const { return m_shaderResourceView.IsValid(); }
        inline ViewSRVHandle const& GetShaderResourceView() const { return m_shaderResourceView; }

        inline bool HasUnorderedAccessView() const { return m_unorderedAccessView.IsValid(); }
        inline ViewUAVHandle const& GetUnorderedAccessView() const { return m_unorderedAccessView; }
        
        inline bool HasRenderTargetView() const { return m_renderTargetView.IsValid(); }
        inline ViewRTHandle const& GetRenderTargetView() const { return m_renderTargetView; }
        
        inline bool HasDepthStencilView() const { return m_depthStencilView.IsValid(); }
        inline ViewDSHandle const& GetDepthStencilView() const { return m_depthStencilView; }

    protected:

        TextureHandle           m_textureHandle;
        ViewSRVHandle           m_shaderResourceView;
        ViewUAVHandle           m_unorderedAccessView;
        ViewRTHandle            m_renderTargetView;
        ViewDSHandle            m_depthStencilView;
        Int2                    m_dimensions = Int2(0, 0);
        TextureFormat           m_format;
        Blob                    m_rawData; // Temporary storage for the raw data used during installation, cleared when installation completes
    };

    //-------------------------------------------------------------------------
    // Cubemap Texture
    //-------------------------------------------------------------------------
    // Abstraction for a cubemap render texture resource
    // Needed to define a new resource type

    class KRG_SYSTEM_API CubemapTexture : public Texture
    {
        friend class RenderDevice;
        friend class TextureCompiler;
        friend class TextureLoader;

        KRG_REGISTER_RESOURCE( 'cbmp', "Render Cubemap Texture" );
        KRG_SERIALIZE( KRG_SERIALIZE_BASE( Texture ) );

    public:

        CubemapTexture() = default;
    };
}