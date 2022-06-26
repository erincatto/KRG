#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    class GraphCompilationContext;

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API GraphResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( GraphResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return GraphDefinition::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API GraphVariationResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( GraphVariationResourceDescriptor );

    public:

        KRG_EXPOSE ResourcePath                 m_graphPath;
        KRG_EXPOSE StringID                     m_variationID; // Optional: if not set, will use the default variation
    };
}