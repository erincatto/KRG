#pragma once

#include "EngineTools/Entity/EntityEditor/EntityEditor_BaseWorkspace.h"
#include "Engine/Entity/EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_ENGINETOOLS_API EntityCollectionEditor final : public EntityEditorBaseWorkspace
    {
    public:

        EntityCollectionEditor( ToolsContext const* pToolsContext, EntityWorld* pWorld, ResourceID const& collectionResourceID );

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        virtual uint32_t GetID() const override { return 0x00FFFFFF; }
        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool Save() override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

    private:

        TResourcePtr<EntityCollectionDescriptor>        m_collection;
        bool                                            m_collectionInstantiated = false;
    };
}