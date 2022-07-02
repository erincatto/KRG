#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/ResourceDescriptor.h"
#include "System/Render/RenderTexture.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    enum class TextureType
    {
        KRG_REGISTER_ENUM

        Default,
        AmbientOcclusion,
        TangentSpaceNormals,
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API TextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( TextureResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return Texture::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath     m_path;
        KRG_EXPOSE TextureType      m_type = TextureType::Default;
        KRG_EXPOSE String           m_name; // Optional property needed for extracting textures out of container files (e.g. glb, fbx)
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API CubemapTextureResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( CubemapTextureResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return CubemapTexture::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath     m_path;
    };
}