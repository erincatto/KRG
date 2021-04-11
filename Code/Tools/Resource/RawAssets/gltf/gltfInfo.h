#ifdef _WIN32
#pragma once

//-------------------------------------------------------------------------

namespace KRG::RawAssets { struct RawAssetInfo; }
namespace KRG::FileSystem { class Path; }

//-------------------------------------------------------------------------

namespace KRG::gltf
{
    bool ReadFileInfo( FileSystem::Path const& sourceFilePath, RawAssets::RawAssetInfo& outInfo );
}

#endif