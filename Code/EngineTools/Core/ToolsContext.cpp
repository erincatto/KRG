#include "ToolsContext.h"
#include "EngineTools/Resource/ResourceDatabase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    FileSystem::Path const& ToolsContext::GetRawResourceDirectory() const { return m_pResourceDatabase->GetRawResourceDirectoryPath(); }
    FileSystem::Path const& ToolsContext::GetCompiledResourceDirectory() const { return m_pResourceDatabase->GetCompiledResourceDirectoryPath(); }
}