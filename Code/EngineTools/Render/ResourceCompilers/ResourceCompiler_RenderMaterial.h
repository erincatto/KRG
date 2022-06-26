#pragma once

#include "EngineTools/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MaterialCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( MaterialCompiler );
        static const int32_t s_version = 1;

    public:

        MaterialCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        virtual bool GetReferencedResources( ResourceID const& resourceID, TVector<ResourceID>& outReferencedResources ) const override;
    };
}