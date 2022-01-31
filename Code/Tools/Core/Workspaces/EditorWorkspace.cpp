#include "EditorWorkspace.h"
#include "Engine/Render/Debug/DebugView_Render.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "Engine/Core/DebugViews/DebugView_Camera.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/ToolsUI/OrientationGuide.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Drawing::DrawContext EditorWorkspace::GetDrawingContext()
    {
        return m_pWorld->GetDebugDrawingSystem()->GetDrawingContext();
    }

    //-------------------------------------------------------------------------

    EditorWorkspace::EditorWorkspace( ToolsContext const* pToolsContext, EntityWorld* pWorld, String const& displayName )
        : m_pToolsContext( pToolsContext )
        , m_pWorld( pWorld )
        , m_displayName( displayName )
    {
        KRG_ASSERT( m_pWorld != nullptr );
        KRG_ASSERT( m_pToolsContext != nullptr && m_pToolsContext->IsValid() );
    }

    EditorWorkspace::~EditorWorkspace()
    {
        KRG_ASSERT( m_requestedResources.empty() );
        KRG_ASSERT( m_reloadingResources.empty() );
    }

    void EditorWorkspace::Initialize( UpdateContext const& context )
    {
        SetDisplayName( m_displayName );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::SetDisplayName( String const& name )
    {
        m_displayName = name;
        m_workspaceWindowID.sprintf( "%s###window%u", m_displayName.c_str(), GetID() );
    }

    void EditorWorkspace::DrawDefaultToolbarItems()
    {
        bool const isSavingAllowed = AlwaysAllowSaving() || IsDirty();

        ImGui::BeginDisabled( !isSavingAllowed );
        if ( ImGui::MenuItem( KRG_ICON_SAVE"Save" ) )
        {
            Save();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled( !CanUndo() );
        if ( ImGui::MenuItem( KRG_ICON_UNDO"Undo" ) )
        {
            Undo();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled( !CanRedo() );
        if ( ImGui::MenuItem( KRG_ICON_REDO"Redo" ) )
        {
            Redo();
        }
        ImGui::EndDisabled();
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::DrawDefaultViewportToolbarItems()
    {
        ImGui::SetNextItemWidth( 46 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 5, 5 ) );
        if ( ImGui::BeginCombo( "##RenderingOptions", KRG_ICON_EYE, ImGuiComboFlags_HeightLarge ) )
        {
            Render::RenderDebugView::DrawRenderVisualizationModesMenu( GetWorld() );
            ImGui::EndCombo();
        }
        ImGuiX::ItemTooltip( "Render Modes" );
        ImGui::PopStyleVar();
        ImGui::SameLine();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( 46 );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 5, 5 ) );
        if ( ImGui::BeginCombo( "##CameraOptions", KRG_ICON_CAMERA, ImGuiComboFlags_HeightLarge ) )
        {
            CameraDebugView::DrawDebugCameraOptions( GetWorld() );

            ImGui::EndCombo();
        }
        ImGuiX::ItemTooltip( "Camera Options" );
        ImGui::PopStyleVar();
        ImGui::SameLine();

        //-------------------------------------------------------------------------

        if ( m_allowWorldTimeControl )
        {
            if ( BeginViewportToolbarGroup( "TimeControls", ImVec2( 174, 0 ), ImVec2( 2, 1 ) ) )
            {
                ImGuiX::ScopedFont sf( ImGuiX::Font::Tiny );

                // Play/Pause
                if ( m_pWorld->IsPaused() )
                {
                    if ( ImGui::Button( KRG_ICON_PLAY"##ResumeWorld" ) )
                    {
                        SetWorldPaused( false );
                    }
                    ImGuiX::ItemTooltip( "Pause" );
                }
                else
                {
                    if ( ImGui::Button( KRG_ICON_PAUSE"##PauseWorld" ) )
                    {
                        SetWorldPaused( true );
                    }
                    ImGuiX::ItemTooltip( "Pause" );
                }

                // Step
                ImGui::SameLine( 0, 0 );
                ImGui::BeginDisabled( !m_pWorld->IsPaused() );
                if ( ImGui::Button( KRG_ICON_STEP_FORWARD"##StepFrame" ) )
                {
                    m_pWorld->RequestTimeStep();
                }
                ImGuiX::ItemTooltip( "Step Frame" );
                ImGui::EndDisabled();

                // Slider
                ImGui::SameLine( 0, 0 );
                ImGui::SetNextItemWidth( 100 );
                float newTimeScale = m_worldTimeScale;
                if ( ImGui::SliderFloat( "##TimeScale", &newTimeScale, 0.1f, 3.5f, "%.2f", ImGuiSliderFlags_NoInput ) )
                {
                    SetWorldTimeScale( newTimeScale );
                }
                ImGuiX::ItemTooltip( "Time Scale" );

                // Reset
                ImGui::SameLine( 0, 0 );
                if ( ImGui::Button( KRG_ICON_UNDO"##ResetTimeScale" ) )
                {
                    ResetWorldTimeScale();
                }
                ImGuiX::ItemTooltip( "Reset TimeScale" );
            }
            EndViewportToolbarGroup();
        }
    }

    bool EditorWorkspace::BeginViewportToolbarGroup( char const* pGroupID, ImVec2 groupSize, ImVec2 const& padding )
    {
        ImGui::SameLine();

        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Style::s_backgroundColorSemiLight.Value );
        ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorLight.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, ImGuiX::Style::s_itemColorDark.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBgActive, ImGuiX::Style::s_backgroundColorMedium.Value );
        ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, ImGuiX::Style::s_backgroundColorMedium.Value );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, padding );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 4.0f );

        // Adjust "use available" height to default toolbar height
        if ( groupSize.y <= 0 )
        {
            groupSize.y = ImGui::GetFrameHeight();
        }

        return ImGui::BeginChild( pGroupID, groupSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar );
    }

    void EditorWorkspace::EndViewportToolbarGroup()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
        ImGui::PopStyleColor( 5 );

        ImGui::SameLine();
    }

    //-------------------------------------------------------------------------

    bool EditorWorkspace::DrawViewport( UpdateContext const& context, ViewportInfo const& viewportInfo, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( viewportInfo.m_pViewportRenderTargetTexture != nullptr && viewportInfo.m_retrievePickingID != nullptr );

        m_isViewportFocused = false;
        m_isViewportHovered = false;

        auto pWorld = GetWorld();
        Render::Viewport* pViewport = pWorld->GetViewport();

        // Create viewport window
        ImGuiWindowFlags const viewportWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs;
        ImGui::SetNextWindowClass( pWindowClass );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        if ( ImGui::Begin( GetViewportWindowID(), nullptr, viewportWindowFlags ) )
        {
            m_isViewportFocused = ImGui::IsWindowFocused();
            m_isViewportHovered = ImGui::IsWindowHovered();

            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );

            ImVec2 const windowPos = ImGui::GetWindowPos();

            // Switch focus based on mouse input
            //-------------------------------------------------------------------------

            if ( m_isViewportHovered )
            {
                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) || ImGui::IsMouseClicked( ImGuiMouseButton_Middle ) )
                {
                    ImGui::SetWindowFocus();
                    m_isViewportFocused = true;
                }
            }

            // Update engine viewport dimensions
            //-------------------------------------------------------------------------

            Math::Rectangle const viewportRect( Float2::Zero, viewportSize );
            pViewport->Resize( viewportRect );

            // Draw 3D scene
            //-------------------------------------------------------------------------

            ImVec2 const viewportImageCursorPos = ImGui::GetCursorPos();
            ImGui::Image( viewportInfo.m_pViewportRenderTargetTexture, viewportSize );

            if ( ImGui::BeginDragDropTarget() )
            {
                OnDragAndDrop( pViewport );
                ImGui::EndDragDropTarget();
            }

            // Draw overlay elements
            //-------------------------------------------------------------------------

            ImGui::SetCursorPos( style.WindowPadding );
            DrawViewportOverlayElements( context, pViewport );

            if ( HasViewportOrientationGuide() )
            {
                ImGuiX::OrientationGuide::Draw( ImGui::GetWindowPos() + viewportSize - ImVec2( ImGuiX::OrientationGuide::GetWidth() + 4, ImGuiX::OrientationGuide::GetWidth() + 4 ), *pViewport );
            }

            // Draw viewport toolbar
            //-------------------------------------------------------------------------

            if ( HasViewportToolbar() )
            {
                ImGui::SetCursorPos( ImGui::GetWindowContentRegionMin() + ImGui::GetStyle().ItemSpacing );
                DrawDefaultViewportToolbarItems();
                DrawViewportToolbar( context, pViewport );
            }

            // Handle picking
            //-------------------------------------------------------------------------

            if ( m_isViewportHovered && !ImGui::IsAnyItemHovered() )
            {
                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
                {
                    ImVec2 const mousePos = ImGui::GetMousePos();
                    if ( mousePos.x != FLT_MAX && mousePos.y != FLT_MAX )
                    {
                        ImVec2 const mousePosWithinViewportImage = ( mousePos - windowPos ) - viewportImageCursorPos;
                        Int2 const pixelCoords = Int2( Math::RoundToInt( mousePosWithinViewportImage.x ), Math::RoundToInt( mousePosWithinViewportImage.y ) );
                        Render::PickingID const pickingID = viewportInfo.m_retrievePickingID( pixelCoords );
                        if ( pickingID.IsSet() )
                        {
                            OnMousePick( pickingID );
                        }
                    }
                }
            }

            // Handle being docked
            //-------------------------------------------------------------------------

            if ( auto pDockNode = ImGui::GetWindowDockNode() )
            {
                pDockNode->LocalFlags = 0;
                pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
                pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        return m_isViewportFocused;
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::SetViewportCameraSpeed( float cameraSpeed )
    {
        KRG_ASSERT( m_pWorld != nullptr );
        auto pPlayerManager = m_pWorld->GetWorldSystem<PlayerManager>();
        pPlayerManager->SetDebugCameraSpeed( cameraSpeed );
    }

    void EditorWorkspace::SetViewportCameraPosition( Transform const& cameraTransform )
    {
        KRG_ASSERT( m_pWorld != nullptr );
        auto pPlayerManager = m_pWorld->GetWorldSystem<PlayerManager>();
        pPlayerManager->SetDebugCameraView( cameraTransform );
    }

    void EditorWorkspace::SetWorldPaused( bool newPausedState )
    {
        bool const currentPausedState = m_pWorld->IsPaused();

        if ( currentPausedState == newPausedState )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pWorld->IsPaused() )
        {
            m_pWorld->SetTimeScale( m_worldTimeScale );
        }
        else // Pause
        {
            m_worldTimeScale = m_pWorld->GetTimeScale();
            m_pWorld->SetTimeScale( -1.0f );
        }
    }

    void EditorWorkspace::SetWorldTimeScale( float newTimeScale )
    {
        m_worldTimeScale = Math::Clamp( newTimeScale, 0.1f, 3.5f );
        m_pWorld->SetTimeScale( m_worldTimeScale );
    }

    void EditorWorkspace::ResetWorldTimeScale()
    {
        m_worldTimeScale = 1.0f;
        m_pWorld->SetTimeScale( 1.0f );
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::LoadResource( Resource::ResourcePtr* pResourcePtr )
    {
        KRG_ASSERT( pResourcePtr != nullptr && pResourcePtr->IsUnloaded() );
        KRG_ASSERT( !VectorContains( m_requestedResources, pResourcePtr ) );
        m_requestedResources.emplace_back( pResourcePtr );
        m_pToolsContext->m_pResourceSystem->LoadResource( *pResourcePtr );
    }

    void EditorWorkspace::UnloadResource( Resource::ResourcePtr* pResourcePtr )
    {
        KRG_ASSERT( !pResourcePtr->IsUnloaded() );
        KRG_ASSERT( VectorContains( m_requestedResources, pResourcePtr ) );
        m_pToolsContext->m_pResourceSystem->UnloadResource( *pResourcePtr );
        m_requestedResources.erase_first_unsorted( pResourcePtr );
    }

    void EditorWorkspace::AddEntityToWorld( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && !pEntity->IsAddedToMap() );
        KRG_ASSERT( !VectorContains( m_addedEntities, pEntity ) );
        m_addedEntities.emplace_back( pEntity );
        m_pWorld->GetPersistentMap()->AddEntity( pEntity );
    }

    void EditorWorkspace::RemoveEntityFromWorld( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->GetMapID() == m_pWorld->GetPersistentMap()->GetID() );
        KRG_ASSERT( VectorContains( m_addedEntities, pEntity ) );
        m_pWorld->GetPersistentMap()->RemoveEntity( pEntity );
        m_addedEntities.erase_first_unsorted( pEntity );
    }

    void EditorWorkspace::DestroyEntityInWorld( Entity*& pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->GetMapID() == m_pWorld->GetPersistentMap()->GetID() );
        KRG_ASSERT( VectorContains( m_addedEntities, pEntity ) );
        m_pWorld->GetPersistentMap()->DestroyEntity( pEntity );
        m_addedEntities.erase_first_unsorted( pEntity );
        pEntity = nullptr;
    }

    //-------------------------------------------------------------------------

    void EditorWorkspace::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded )
    {
        for ( auto& pLoadedResource : m_requestedResources )
        {
            if ( pLoadedResource->IsUnloaded() || pLoadedResource->HasLoadingFailed() )
            {
                continue;
            }

            // Check resource and install dependencies to see if we need to unload it
            bool shouldUnload = VectorContains( resourcesToBeReloaded, pLoadedResource->GetResourceID() );
            if ( !shouldUnload )
            {
                for ( ResourceID const& installDependency : pLoadedResource->GetInstallDependencies() )
                {
                    if ( VectorContains( resourcesToBeReloaded, installDependency ) )
                    {
                        shouldUnload = true;
                        break;
                    }
                }
            }

            // Request unload and track the resource we need to reload
            if ( shouldUnload )
            {
                m_pToolsContext->m_pResourceSystem->UnloadResource( *pLoadedResource );
                m_reloadingResources.emplace_back( pLoadedResource );
            }
        }
    }

    void EditorWorkspace::EndHotReload()
    {
        for( auto& pReloadedResource : m_reloadingResources )
        {
            m_pToolsContext->m_pResourceSystem->LoadResource( *pReloadedResource );
        }
        m_reloadingResources.clear();
    }
}