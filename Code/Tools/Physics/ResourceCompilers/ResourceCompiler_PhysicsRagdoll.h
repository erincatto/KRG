#pragma  once

#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class RagdollCompiler : public Resource::Compiler
    {
        static const int32_t s_version = 3;

    public:

        RagdollCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}