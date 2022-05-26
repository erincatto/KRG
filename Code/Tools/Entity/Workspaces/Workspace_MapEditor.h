#pragma once

#include "Tools/Entity/EntityEditor/EntityEditor_BaseWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API EntityMapEditor final : public EntityEditorBaseWorkspace
    {
    public:

        EntityMapEditor( ToolsContext const* pToolsContext, EntityWorld* pWorld );

        inline bool HasLoadedMap() const { return m_loadedMap.IsValid(); }
        inline ResourceID GetLoadedMap() const { return m_loadedMap; }

        void CreateNewMap();
        void SelectAndLoadMap();
        void LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad );
        void SaveMap();
        void SaveMapAs();

        // Game Preview
        //-------------------------------------------------------------------------

        inline TEventHandle<UpdateContext const&> OnGamePreviewStartRequested() { return m_gamePreviewStartRequested; }
        inline TEventHandle<UpdateContext const&> OnGamePreviewStopRequested() { return m_gamePreviewStopRequested; }

        void NotifyGamePreviewStarted();
        void NotifyGamePreviewEnded();

    private:

        virtual uint32 GetID() const override { return 0xFFFFFFFF; }
        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool Save() override;
        virtual void DrawWorkspaceToolbarItems( UpdateContext const& context ) override;

    private:

        ResourceID                                      m_loadedMap;
        bool                                            m_isGamePreviewRunning = false;
        TEvent<UpdateContext const&>                    m_gamePreviewStartRequested;
        TEvent<UpdateContext const&>                    m_gamePreviewStopRequested;
    };
}