#pragma  once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMaterialDatabaseCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( PhysicsMaterialDatabaseCompiler );
        static const int32_t s_version = 0;

    public:

        PhysicsMaterialDatabaseCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}