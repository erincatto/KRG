#include "Workspace_GamePreviewer.h"
#include "Engine/Entity/EntityWorld.h"
#include "System/ThirdParty/iniparser/krg_ini.h"

//-------------------------------------------------------------------------

namespace KRG
{
    GamePreviewer::GamePreviewer( ToolsContext const* pToolsContext, EntityWorld* pWorld )
        : EditorWorkspace( pToolsContext, pWorld )
    {}

    void GamePreviewer::LoadMapToPreview( ResourceID mapResourceID )
    {
        if ( mapResourceID != m_loadedMap )
        {
            // Unload current map
            if ( m_loadedMap.IsValid() && m_pWorld->IsMapActive( m_loadedMap ) )
            {
                m_pWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_loadedMap = mapResourceID;
            m_pWorld->LoadMap( m_loadedMap );
            SetDisplayName( m_loadedMap.GetResourcePath().GetFileNameWithoutExtension() );
        }
    }

    void GamePreviewer::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );
        m_engineToolsUI.Initialize( context );
        m_engineToolsUI.LockToWindow( GetViewportWindowID() );
    }

    void GamePreviewer::Shutdown( UpdateContext const& context )
    {
        KRG_ASSERT( m_loadedMap.IsValid() );
        m_pWorld->UnloadMap( m_loadedMap );

        m_engineToolsUI.Shutdown( context );
        EditorWorkspace::Shutdown( context );
    }

    bool GamePreviewer::HasWorkspaceToolbar() const
    {
         return m_engineToolsUI.m_debugOverlayEnabled;
    }

    //-------------------------------------------------------------------------

    void GamePreviewer::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );
    }

    void GamePreviewer::UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineToolsUI.HandleUserInput( context, m_pWorld );
        m_engineToolsUI.DrawWindows( context, m_pWorld, pWindowClass );
    }

    void GamePreviewer::DrawViewportOverlayElements( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineToolsUI.DrawOverlayElements( context, pViewport );
    }

    void GamePreviewer::DrawWorkspaceToolbarItems( UpdateContext const& context )
    {
        KRG_ASSERT( context.GetUpdateStage() == UpdateStage::FrameEnd );
        m_engineToolsUI.DrawMenu( context, m_pWorld );
    }
}