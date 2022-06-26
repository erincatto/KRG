#pragma once
#include "EngineTools/_Module/API.h"
#include "EngineTools/RawAssets/RawSkeleton.h"
#include "System/FileSystem/FileSystemPath.h"
#include "System/Memory/Pointers.h"

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    class FbxSceneContext;

    //-------------------------------------------------------------------------

    KRG_ENGINETOOLS_API TUniquePtr<RawAssets::RawSkeleton> ReadSkeleton( FileSystem::Path const& sourceFilePath, String const& skeletonRootBoneName );

    // Temp HACK
    KRG_ENGINETOOLS_API void ReadSkeleton( FbxSceneContext const& sceneCtx, String const& skeletonRootBoneName, RawAssets::RawSkeleton& skeleton );
}