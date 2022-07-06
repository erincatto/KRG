#pragma once

#include "EngineTools/Resource/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapCompiler final : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( EntityMapCompiler );
        static const int32_t s_version = 2;

    public:

        EntityMapCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        virtual bool GetReferencedResources( ResourceID const& resourceID, TVector<ResourceID>& outReferencedResources ) const override;
    };
}
