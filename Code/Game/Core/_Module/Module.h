#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"

//-------------------------------------------------------------------------

namespace KRG::Game
{
    class KRG_GAME_CORE_API GameModule : public IModule
    {
        KRG_REGISTER_MODULE;

    public:

        virtual bool Initialize( ModuleContext& context, IniFile const& iniFile ) override final;
        virtual void Shutdown( ModuleContext& context ) override final;
    };
}