#pragma once

#include "../_Module/API.h"
#include "System/Render/RenderShader.h"
#include "System/Render/RenderStates.h"
#include "System/Render/RenderPipelineState.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API Material : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'mtrl', "Render Material" );
        friend class MaterialCompiler;
        friend class MaterialLoader;

        KRG_SERIALIZE_MEMBERS( m_pAlbedoTexture, m_pMetalnessTexture, m_pRoughnessTexture, m_pNormalMapTexture, m_pAOTexture, m_albedo, m_metalness, m_roughness, m_normalScaler );

    public:

        virtual bool IsValid() const override { return true; }

        inline Texture const* GetAlbedoTexture() const { KRG_ASSERT( IsValid() ); return m_pAlbedoTexture.GetPtr(); }
        inline Texture const* GetMetalnessTexture() const { KRG_ASSERT( IsValid() ); return m_pMetalnessTexture.GetPtr(); }
        inline Texture const* GetRoughnessTexture() const { KRG_ASSERT( IsValid() ); return m_pRoughnessTexture.GetPtr(); }
        inline Texture const* GetNormalMapTexture() const { KRG_ASSERT( IsValid() ); return m_pNormalMapTexture.GetPtr(); }
        inline Texture const* GetAOTexture() const { KRG_ASSERT( IsValid() ); return m_pAOTexture.GetPtr(); }

        KRG_FORCE_INLINE bool HasAlbedoTexture() const { return m_pAlbedoTexture.IsValid(); }
        KRG_FORCE_INLINE bool HasMetalnessTexture() const { return m_pMetalnessTexture.IsValid(); }
        KRG_FORCE_INLINE bool HasRoughnessTexture() const { return m_pRoughnessTexture.IsValid(); }
        KRG_FORCE_INLINE bool HasNormalMapTexture() const { return m_pNormalMapTexture.IsValid(); }
        KRG_FORCE_INLINE bool HasAOTexture() const { return m_pAOTexture.IsValid(); }

        KRG_FORCE_INLINE Color GetAlbedoValue() const { return m_albedo; }
        KRG_FORCE_INLINE float GetMetalnessValue() const { return m_metalness; }
        KRG_FORCE_INLINE float GetRoughnessValue() const { return m_roughness; }
        KRG_FORCE_INLINE float GetNormalScalerValue() const { return m_normalScaler; }

    private:

        TResourcePtr<Texture>   m_pAlbedoTexture;
        TResourcePtr<Texture>   m_pMetalnessTexture;
        TResourcePtr<Texture>   m_pRoughnessTexture;
        TResourcePtr<Texture>   m_pNormalMapTexture;
        TResourcePtr<Texture>   m_pAOTexture;
        Color                   m_albedo = Colors::Black;
        float                   m_metalness = 0.0f;
        float                   m_roughness = 0.0f;
        float                   m_normalScaler = 1.0f;
    };
}