#include "ReflectorSettingsAndUtils.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Reflection
{
    static char const* g_macroNames[] =
    {
        "KRG_REFLECTION_IGNORE_HEADER",
        "KRG_REGISTER_MODULE",
        "KRG_REGISTER_ENUM",
        "KRG_REGISTER_TYPE",
        "KRG_REGISTER_RESOURCE",
        "KRG_REGISTER_TYPE_RESOURCE",
        "KRG_REGISTER_VIRTUAL_RESOURCE",
        "KRG_REGISTER_ENTITY_COMPONENT",
        "KRG_REGISTER_SINGLETON_ENTITY_COMPONENT",
        "KRG_REGISTER_ENTITY_SYSTEM",
        "KRG_REGISTER",
        "KRG_EXPOSE",
    };

    //-------------------------------------------------------------------------

    char const* GetReflectionMacroText( ReflectionMacro macro )
    {
        uint32_t const macroIdx = (uint32_t) macro;
        KRG_ASSERT( macroIdx < (uint32_t) ReflectionMacro::NumMacros );
        return g_macroNames[macroIdx];
    }
}