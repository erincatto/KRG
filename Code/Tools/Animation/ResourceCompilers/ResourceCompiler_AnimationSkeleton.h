#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class SkeletonCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( SkeletonCompiler );
        static const int32_t s_version = 2;

    public:

        SkeletonCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const final;
    };
}