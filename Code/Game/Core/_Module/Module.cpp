#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Game
{
    bool GameModule::Initialize( ModuleContext& context, IniFile const& iniFile )
    {
        return true;
    }

    void GameModule::Shutdown( ModuleContext& context )
    {
    }
}