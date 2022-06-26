#pragma once
#include "EngineTools/_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityEditorContext;

    //-------------------------------------------------------------------------

    class KRG_ENGINETOOLS_API EntityEditorOutliner
    {
    public:

        EntityEditorOutliner( EntityEditorContext& ctx );

        void Draw( UpdateContext const& context );

    private:

        EntityEditorContext&                m_context;
    };
}