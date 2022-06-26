#pragma once

#include "EngineTools/RawAssets/RawMesh.h"
#include "System/FileSystem/FileSystemPath.h"
#include "System/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::gltf
{
    KRG_ENGINETOOLS_API TUniquePtr<RawAssets::RawMesh> ReadStaticMesh( FileSystem::Path const& sourceFilePath, String const& nameOfMeshToCompile = String() );
    KRG_ENGINETOOLS_API TUniquePtr<RawAssets::RawMesh> ReadSkeletalMesh( FileSystem::Path const& sourceFilePath, int32_t maxBoneInfluences );
}