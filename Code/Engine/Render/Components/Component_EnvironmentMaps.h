#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Entity/EntitySpatialComponent.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------
namespace KRG::Render
{
    class KRG_ENGINE_API LocalEnvironmentMapComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( LocalEnvironmentMapComponent );

    public:

        inline CubemapTexture const* GetEnvironmentMapTexture() const { return m_environmentMapTexture.GetPtr(); }

    private:

        KRG_EXPOSE TResourcePtr<CubemapTexture> m_environmentMapTexture;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_API GlobalEnvironmentMapComponent : public EntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( GlobalEnvironmentMapComponent );

    public:

        inline bool HasSkyboxTexture() const { return m_skyboxTexture.IsLoaded(); }
        inline CubemapTexture const* GetSkyboxTexture() const { return m_skyboxTexture.GetPtr(); }

        inline bool HasSkyboxRadianceTexture() const { return m_skyboxRadianceTexture.IsLoaded(); }
        inline CubemapTexture const* GetSkyboxRadianceTexture() const { return m_skyboxRadianceTexture.GetPtr(); }

        inline float GetSkyboxIntensity() const { return m_skyboxIntensity; }

        //TODO: lighting hack
        inline float GetExposure() const { return m_exposure; }

    private:

        KRG_EXPOSE TResourcePtr<CubemapTexture> m_skyboxTexture;
        KRG_EXPOSE TResourcePtr<CubemapTexture> m_skyboxRadianceTexture;
        KRG_EXPOSE float m_skyboxIntensity = 1.0;
        KRG_EXPOSE float m_exposure = -1.0;
    };
}