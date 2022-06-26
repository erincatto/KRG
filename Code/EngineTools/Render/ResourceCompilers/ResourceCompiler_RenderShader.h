#pragma once

#include "EngineTools/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderShader.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class ShaderCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( ShaderCompiler );
        static const int32_t s_version = 1;

    public:

        ShaderCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}