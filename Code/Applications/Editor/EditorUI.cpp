#include "EditorUI.h"
#include "Tools/Entity/Workspaces/Workspace_MapEditor.h"
#include "Tools/Entity/Workspaces/Workspace_GamePreviewer.h"
#include "Tools/Core/Workspaces/EditorWorkspace.h"
#include "Engine/Physics/Debug/DebugView_Physics.h"
#include "Engine/Core/ToolsUI/OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DebugViews/DebugView_Resource.h"
#include "Engine/Core/DebugViews/DebugView_RuntimeSettings.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Render/Debug/DebugView_Render.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EditorUI::~EditorUI()
    {
        KRG_ASSERT( m_pResourceBrowser == nullptr );
    }

    void EditorUI::SetStartupMap( ResourceID const& mapID )
    {
        KRG_ASSERT( mapID.IsValid() );

        if ( mapID.GetResourceTypeID() == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            m_startupMap = mapID;
        }
        else
        {
            KRG_LOG_ERROR( "Editor", "Invalid startup map resource supplied: %s", m_startupMap.c_str() );
        }
    }

    void EditorUI::Initialize( UpdateContext const& context )
    {
        m_context.Initialize( context );
        m_pResourceBrowser = KRG::New<ResourceBrowser>( m_context );
        m_resourceDatabaseUpdateEventBindingID = m_context.GetResourceDatabase()->OnDatabaseUpdated().Bind( [this] () { m_pResourceBrowser->RebuildBrowserTree(); } );

        //-------------------------------------------------------------------------

        if ( m_startupMap.IsValid() )
        {
            m_context.LoadMap( m_startupMap );
        }
    }

    void EditorUI::Shutdown( UpdateContext const& context )
    {
        if ( m_resourceDatabaseUpdateEventBindingID.IsValid() )
        {
            m_context.GetResourceDatabase()->OnDatabaseUpdated().Unbind( m_resourceDatabaseUpdateEventBindingID );
        }

        KRG::Delete( m_pResourceBrowser );
        m_context.Shutdown( context );
    }

    //-------------------------------------------------------------------------

    void EditorUI::StartFrame( UpdateContext const& context )
    {
        UpdateStage const updateStage = context.GetUpdateStage();
        KRG_ASSERT( updateStage == UpdateStage::FrameStart );

        // Update the model - this process all workspace lifetime requests
        m_context.Update( context );

        //-------------------------------------------------------------------------
        // Main Menu
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMainMenuBar() )
        {
            DrawMainMenu( context );
            ImGui::EndMainMenuBar();
        }

        //-------------------------------------------------------------------------
        // Create main dock window
        //-------------------------------------------------------------------------

        m_editorWindowClass.ClassId = ImGui::GetID( "EditorWindowClass" );
        m_editorWindowClass.DockingAllowUnclassed = false;

        ImGuiID const dockspaceID = ImGui::GetID( "EditorDockSpace" );

        ImGuiWindowFlags const windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        ImGui::PopStyleVar( 3 );
        {
            if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            {
                ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace );
                ImGui::DockBuilderSetNodeSize( dockspaceID, ImGui::GetContentRegionAvail() );
                ImGuiID leftDockID = 0, rightDockID = 0;
                ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.25f, &leftDockID, &rightDockID );
                ImGui::DockBuilderFinish( dockspaceID );

                ImGui::DockBuilderDockWindow( m_pResourceBrowser->GetWindowName(), leftDockID );
                ImGui::DockBuilderDockWindow( m_context.GetMapEditorWindowName(), rightDockID );
            }

            // Create the actual dock space
            ImGui::PushStyleVar( ImGuiStyleVar_TabRounding, 0 );
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &m_editorWindowClass );
            ImGui::PopStyleVar( 1 );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw editor windows
        //-------------------------------------------------------------------------

        if ( m_isResourceBrowserWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            m_isResourceBrowserWindowOpen = m_pResourceBrowser->Draw( context );
        }

        if ( m_isResourceLogWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            Resource::ResourceDebugView::DrawResourceLogWindow( m_context.GetResourceSystem(), &m_isResourceLogWindowOpen );
        }

        if ( m_isResourceReferenceTrackerWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            Resource::ResourceDebugView::DrawReferenceTrackerWindow( m_context.GetResourceSystem(), &m_isResourceReferenceTrackerWindowOpen );
        }

        if ( m_isSystemLogWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            m_isSystemLogWindowOpen = m_systemLogView.Draw( context );
        }

        if ( m_isRuntimeSettingsWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            m_isRuntimeSettingsWindowOpen = RuntimeSettingsDebugView::DrawRuntimeSettingsView( context );
        }

        if ( m_isPhysicsMaterialDatabaseWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            m_isPhysicsMaterialDatabaseWindowOpen = Physics::PhysicsDebugView::DrawMaterialDatabaseView( context );
        }

        if ( m_isImguiDemoWindowOpen )
        {
            ImGui::ShowDemoWindow( &m_isImguiDemoWindowOpen );
        }

        if ( m_isFontTestWindowOpen )
        {
            DrawFontTestWindow();
        }

        //-------------------------------------------------------------------------
        // Draw open workspaces
        //-------------------------------------------------------------------------

        // Reset mouse state, this is updated via the workspaces
        EditorWorkspace* pWorkspaceToClose = nullptr;

        // Draw all workspaces
        for ( auto pWorkspace : m_context.GetWorkspaces() )
        {
            if ( m_context.IsGamePreviewWorkspace( pWorkspace ) )
            {
                continue;
            }

            ImGui::SetNextWindowClass( &m_editorWindowClass );
            if ( !DrawWorkspaceWindow( context, pWorkspace ) )
            {
                pWorkspaceToClose = pWorkspace;
            }
        }

        // Did we get a close request?
        if ( pWorkspaceToClose != nullptr )
        {
            // We need to defer this to the start of the update since we may have references resources that we might unload (i.e. textures)
            m_context.QueueDestroyWorkspace( pWorkspaceToClose );
        }

        //-------------------------------------------------------------------------
        // Handle Warnings/Errors
        //-------------------------------------------------------------------------

        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        if ( !unhandledWarningsAndErrors.empty() )
        {
            m_isSystemLogWindowOpen = true;
        }
    }

    void EditorUI::EndFrame( UpdateContext const& context )
    {
        // Game previewer needs to be drawn at the end of the frames since then all the game simulation data will be correct and all the debug tools will be accurate
        if ( m_context.IsGamePreviewRunning() )
        {
            GamePreviewer* pGamePreviewer = m_context.GetGamePreviewWorkspace();
            if ( !DrawWorkspaceWindow( context, pGamePreviewer ) )
            {
                m_context.QueueDestroyWorkspace( pGamePreviewer );
            }
        }
    }

    void EditorUI::Update( UpdateContext const& context )
    {
        for ( auto pWorkspace : m_context.GetWorkspaces() )
        {
            EntityWorldUpdateContext updateContext( context, pWorkspace->GetWorld() );
            pWorkspace->UpdateWorld( updateContext );
        }
    }

    //-------------------------------------------------------------------------

    void EditorUI::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded )
    {
        for ( auto pWorkspace : m_context.GetWorkspaces() )
        {
            pWorkspace->BeginHotReload( usersToBeReloaded, resourcesToBeReloaded );
        }
    }

    void EditorUI::EndHotReload()
    {
        for ( auto pWorkspace : m_context.GetWorkspaces() )
        {
            pWorkspace->EndHotReload();
        }
    }

    //-------------------------------------------------------------------------

    void EditorUI::DrawFontTestWindow()
    {
        if ( ImGui::Begin( "Font Test", &m_isFontTestWindowOpen ) )
        {
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Small );
                ImGui::Text( KRG_ICON_FILE_CHECK"This is a test - Small" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::SmallBold );
                ImGui::Text( KRG_ICON_ALERT"This is a test - Small Bold" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Medium );
                ImGui::Text( KRG_ICON_FILE_CHECK"This is a test - Medium" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::MediumBold );
                ImGui::Text( KRG_ICON_ALERT"This is a test - Medium Bold" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Large );
                ImGui::Text( KRG_ICON_FILE_CHECK"This is a test - Large" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::LargeBold );
                ImGui::Text( KRG_ICON_CCTV_OFF"This is a test - Large Bold" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Huge );
                ImGui::Text( KRG_ICON_FILE_CHECK"This is a test - Huge" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::HugeBold );
                ImGui::Text( KRG_ICON_FILE_CHECK"This is a test - Huge Bold" );
            }

            //-------------------------------------------------------------------------

            ImGui::NewLine();

            //-------------------------------------------------------------------------

            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Small );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::SmallBold );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Medium );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::MediumBold );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Large );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::LargeBold );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Huge );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::HugeBold );
                ImGuiX::ColoredButton( Colors::Green, Colors::White, KRG_ICON_PLUS"ADD" );
            }

            //-------------------------------------------------------------------------

            ImGui::NewLine();

            //-------------------------------------------------------------------------
            
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Small );
                ImGui::Button( KRG_ICON_HAIR_DRYER );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_Z_WAVE );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_KANGAROO );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_YIN_YANG );
            }

            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Medium );
                ImGui::Button( KRG_ICON_HAIR_DRYER );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_Z_WAVE );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_KANGAROO );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_YIN_YANG );
            }

            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Large );
                ImGui::Button( KRG_ICON_HAIR_DRYER );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_Z_WAVE );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_KANGAROO );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_YIN_YANG );
            }

            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Huge );
                ImGui::Button( KRG_ICON_HAIR_DRYER );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_Z_WAVE );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_KANGAROO );
                ImGui::SameLine();
                ImGui::Button( KRG_ICON_YIN_YANG );
            }

            //-------------------------------------------------------------------------

            ImGuiX::IconButton( KRG_ICON_KANGAROO, "Test", Colors::PaleGreen, ImVec2( 100, 0 ) );

            ImGuiX::IconButton( KRG_ICON_HOME, "Home", Colors::RoyalBlue, ImVec2( 100, 0 ) );

            ImGuiX::IconButton( KRG_ICON_MOVIE_PLAY, "Play", Colors::LightPink, ImVec2( 100, 0 ) );

            ImGuiX::ColoredIconButton( Colors::Green, Colors::White, Colors::Yellow, KRG_ICON_KANGAROO, "Test", ImVec2( 100, 0 ) );

            ImGuiX::FlatIconButton( KRG_ICON_HOME, "Home", Colors::RoyalBlue, ImVec2( 100, 0 ) );

        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void EditorUI::DrawMainMenu( UpdateContext const& context )
    {
        ImVec2 const menuDimensions = ImGui::GetContentRegionMax();

        //-------------------------------------------------------------------------
        // Engine
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Resource" ) )
        {
            ImGui::MenuItem( "Resource Browser", nullptr, &m_isResourceBrowserWindowOpen );
            ImGui::MenuItem( "Resource Log", nullptr, &m_isResourceLogWindowOpen );
            ImGui::MenuItem( "Resource Reference Tracker", nullptr, &m_isResourceReferenceTrackerWindowOpen );
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Physics" ) )
        {
            ImGui::MenuItem( "Physics Material DB", nullptr, &m_isPhysicsMaterialDatabaseWindowOpen );
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "System" ) )
        {
            ImGui::MenuItem( "Debug Settings", nullptr, &m_isRuntimeSettingsWindowOpen );
            ImGui::MenuItem( "System Log", nullptr, &m_isSystemLogWindowOpen );

            ImGui::Separator();

            ImGui::MenuItem( "Imgui Font Test Window", nullptr, &m_isFontTestWindowOpen );
            ImGui::MenuItem( "Imgui Demo Window", nullptr, &m_isImguiDemoWindowOpen );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------
        // Draw Frame Limiter and Performance Stats
        //-------------------------------------------------------------------------

        float const currentFPS = 1.0f / context.GetDeltaTime();
        float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;

        TInlineString<100> const perfStats( TInlineString<100>::CtorSprintf(), "FPS: %3.0f", currentFPS );
        TInlineString<100> const memStats( TInlineString<100>::CtorSprintf(), "MEM: %.2fMB", allocatedMemory );

        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float const frameLimiterSize = 30;
        float const perfStatsSize = 64;
        float const memStatsSize = ImGui::CalcTextSize( memStats.c_str() ).x;

        float const memStatsOffset = menuDimensions.x - ( itemSpacing * 2 ) - memStatsSize;
        float const perfStatsOffset = memStatsOffset - perfStatsSize;
        float const frameLimiterOffset = perfStatsOffset - frameLimiterSize;

        ImGui::SameLine( frameLimiterOffset, 0 );

        if ( ImGui::BeginMenu( KRG_ICON_CAR_SPEED_LIMITER ) )
        {
            bool noLimit = !context.HasFrameRateLimit();
            if ( ImGui::MenuItem( "None", nullptr, &noLimit ) )
            {
                const_cast<UpdateContext&>( context ).SetFrameRateLimit( 0.0f );
            }

            bool is30FPS = context.HasFrameRateLimit() && context.GetFrameRateLimit() == 30.0f;
            if ( ImGui::MenuItem( "30 FPS", nullptr, &is30FPS ) )
            {
                const_cast<UpdateContext&>( context ).SetFrameRateLimit( 30.0f );
            }

            bool is60FPS = context.HasFrameRateLimit() && context.GetFrameRateLimit() == 60.0f;
            if ( ImGui::MenuItem( "60 FPS", nullptr, &is60FPS ) )
            {
                const_cast<UpdateContext&>( context ).SetFrameRateLimit( 60.0f );
            }

            ImGui::EndMenu();
        }

        ImGui::SameLine( perfStatsOffset );
        ImGui::Text( perfStats.c_str() );

        ImGui::SameLine( memStatsOffset );
        ImGui::Text( memStats.c_str() );
    }

    bool EditorUI::DrawWorkspaceWindow( UpdateContext const& context, EditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        //-------------------------------------------------------------------------
        // Create Workspace Window
        //-------------------------------------------------------------------------
        // This is an empty window that just contains the dockspace for the workspace

        bool isTabOpen = true;
        bool* pIsTabOpen = m_context.IsMapEditorWorkspace( pWorkspace ) ? nullptr : &isTabOpen;

        ImGuiWindowFlags windowFlags = 0;

        if ( pWorkspace->HasWorkspaceToolbar() )
        {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }

        if ( pWorkspace->IsDirty() )
        {
            windowFlags |= ImGuiWindowFlags_UnsavedDocument;
        }

        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::SetNextWindowSize( ImVec2( 1024, 768 ), ImGuiCond_FirstUseEver );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        bool const shouldDrawWindowContents = ImGui::Begin( pWorkspace->GetWorkspaceWindowID(), pIsTabOpen, windowFlags );
        bool const isFocused = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy );
        ImGui::PopStyleVar();

        // Draw Workspace Menu
        //-------------------------------------------------------------------------

        if ( pWorkspace->HasWorkspaceToolbar() )
        {
            if ( ImGui::BeginMenuBar() )
            {
                pWorkspace->DrawWorkspaceToolbar( context );
                ImGui::EndMenuBar();
            }
        }

        // Create dockspace
        //-------------------------------------------------------------------------

        ImGuiID const dockspaceID = ImGui::GetID( pWorkspace->GetDockspaceID() );
        ImGuiWindowClass workspaceWindowClass;
        workspaceWindowClass.ClassId = dockspaceID;
        workspaceWindowClass.DockingAllowUnclassed = false;

        if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
        {
            ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );
            ImGui::DockBuilderSetNodeSize( dockspaceID, ImGui::GetContentRegionAvail() );
            pWorkspace->InitializeDockingLayout( dockspaceID );
            ImGui::DockBuilderFinish( dockspaceID );
        }

        ImGuiDockNodeFlags const dockFlags = shouldDrawWindowContents ? ImGuiDockNodeFlags_None : ImGuiDockNodeFlags_KeepAliveOnly;
        ImGui::DockSpace( dockspaceID, ImGui::GetContentRegionAvail(), dockFlags, &workspaceWindowClass );

        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw workspace contents
        //-------------------------------------------------------------------------

        bool enableInputForWorld = false;
        auto pWorldManager = context.GetSystem<EntityWorldManager>();
        auto pWorld = pWorkspace->GetWorld();

        if ( shouldDrawWindowContents )
        {
            if ( !m_context.IsMapEditorWorkspace( pWorkspace ) || !m_context.IsGamePreviewRunning() )
            {
                pWorld->ResumeUpdates();
            }

            if ( pWorkspace->HasViewportWindow() )
            {
                EditorWorkspace::ViewportInfo viewportInfo;
                viewportInfo.m_pViewportRenderTargetTexture = m_context.GetViewportTextureForWorkspace( pWorkspace );
                viewportInfo.m_retrievePickingID = [this, pWorkspace] ( Int2 const& pixelCoords ) { return m_context.GetViewportPickingID( pWorkspace, pixelCoords ); };
                enableInputForWorld = pWorkspace->DrawViewport( context, viewportInfo, &workspaceWindowClass );
            }

            pWorkspace->UpdateWorkspace( context, &workspaceWindowClass );
        }
        else // If the workspace window is hidden suspend world updates
        {
            pWorld->SuspendUpdates();
        }

        pWorldManager->SetPlayerEnabled( pWorld, enableInputForWorld );

        // Handle input
        //-------------------------------------------------------------------------

        if ( isFocused )
        {
            auto& IO = ImGui::GetIO();
            if ( IO.KeyCtrl && ImGui::IsKeyPressed( ImGuiKey_Z ) )
            {
                if ( pWorkspace->CanUndo() )
                {
                    pWorkspace->Undo();
                }
            }

            if ( IO.KeyCtrl && ImGui::IsKeyPressed( ImGuiKey_Y ) )
            {
                if ( pWorkspace->CanRedo() )
                {
                    pWorkspace->Redo();
                }
            }
        }

        return isTabOpen;
    }
}