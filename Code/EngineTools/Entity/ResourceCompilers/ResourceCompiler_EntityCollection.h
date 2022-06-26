#pragma once

#include "EngineTools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityMapDescriptor;

    //-------------------------------------------------------------------------

    class EntityCollectionCompiler final : public Resource::Compiler
    {
        KRG_REGISTER_TYPE( EntityCollectionCompiler );
        static const int32_t s_version = 7;

    public:

        EntityCollectionCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        virtual bool GetReferencedResources( ResourceID const& resourceID, TVector<ResourceID>& outReferencedResources ) const override;
    };
}
