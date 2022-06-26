#pragma  once

#include "EngineTools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class RagdollCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( RagdollCompiler );
        static const int32_t s_version = 3;

    public:

        RagdollCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}