#pragma once

#include "EngineTools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "System/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    // Create a DDS texture from the supplied texture
    bool ConvertTexture( FileSystem::Path const& texturePath, TextureType type, TVector<uint8_t>& rawData );
}