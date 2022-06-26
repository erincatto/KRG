#pragma once

#include "EngineTools/RawAssets/RawSkeleton.h"
#include "System/FileSystem/FileSystemPath.h"
#include "System/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::gltf
{
    KRG_ENGINETOOLS_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );
}