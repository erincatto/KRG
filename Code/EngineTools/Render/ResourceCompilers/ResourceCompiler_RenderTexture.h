#pragma once

#include "EngineTools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class TextureCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( TextureCompiler );
        static const int32_t s_version = 4;

    public:

        TextureCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    private:

        Resource::CompilationResult CompileTexture( Resource::CompileContext const& ctx ) const;
        Resource::CompilationResult CompileCubemapTexture( Resource::CompileContext const& ctx ) const;
    };
}