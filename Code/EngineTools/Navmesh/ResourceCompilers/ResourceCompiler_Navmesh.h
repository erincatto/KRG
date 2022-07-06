#pragma  once
#if KRG_ENABLE_NAVPOWER

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshCompiler : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( NavmeshCompiler );

    public:

        NavmeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        virtual bool IsInputFileRequired() const override { return false; }

    private:

        Resource::CompilationResult GenerateNavmesh( Resource::CompileContext const& ctx ) const;
    };
}

#endif
