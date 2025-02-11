#pragma once
#include "EngineTools/_Module/API.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_ENGINETOOLS_API FileSystem::Path SaveDialog( String const& extension, FileSystem::Path const& startingPath = FileSystem::Path(), String const& friendlyFilterName = "" );
    KRG_ENGINETOOLS_API FileSystem::Path SaveDialog( ResourceTypeID resourceTypeID, FileSystem::Path const& startingPath = FileSystem::Path(), String const& friendlyFilterName = "" );
}