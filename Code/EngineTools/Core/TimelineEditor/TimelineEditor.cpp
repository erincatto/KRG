#include "TimelineEditor.h"
#include "System/Imgui/ImguiX.h" 
#include "System/Imgui/ImguiStyle.h"

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    static float g_headerHeight = 24;
    static float g_trackHeaderWidth = 200;

    static ImColor const g_headerBackgroundColor( 0xFF3D3837 );
    static ImColor const g_headerLabelColor( 0xFFBBBBBB );
    static ImColor const g_timelineLargeLineColor( 0xFF606060 );
    static ImColor const g_timelineMediumLineColor( 0xFF606060 );
    static ImColor const g_timelineSmallLineColor( 0xFF333333 );
    static ImColor const g_timelineRangeEndLineColor( 0x990000FF );

    static float g_timelineMinimumWidthForLargeInterval = 100;
    static float g_timelineLabelLeftPadding = 4.0f;
    static float g_timelineLargeLineOffset = 4;
    static float g_timelineMediumLineOffset = 10;
    static float g_timelineSmallLineOffset = 16;

    //-------------------------------------------------------------------------

    static ImVec4 const g_playheadDefaultColor = ImColor( 0xFF32CD32 );
    static ImVec4 const g_playheadHoveredColor = Float4( g_playheadDefaultColor ) * 1.20f;
    static ImVec4 const g_playheadShadowColor = ImColor( 0x44000000 );
    static ImVec4 const g_playheadBorderColor = Float4( g_playheadDefaultColor ) * 1.25f;

    //-------------------------------------------------------------------------

    static float g_horizontalScrollbarHeight = 16;
    static float g_verticalScrollbarHeight = 16;

    //-------------------------------------------------------------------------

    static float g_trackHeight = 30;
    static ImColor const g_trackSeparatorColor( 0xFF808080 );
    static ImColor const g_selectedTrackColor = ImGuiX::ConvertColor( Color( 0x00AA0021 ) );
    static float const g_itemMarginY = 2;
    static float const g_itemHandleWidth = 4;
    static float const g_immediateItemHalfWidth = 5;

    //-------------------------------------------------------------------------

    void TimelineEditor::MouseState::Reset()
    {
        m_isHoveredOverTrackEditor = false;
        m_pHoveredTrack = nullptr;
        m_pHoveredItem = nullptr;
        m_hoveredItemMode = ItemEditMode::None;
        m_playheadTimeForMouse = -1.0f;
        m_snappedPlayheadTimeForMouse = -1.0f;
    }

    inline void TimelineEditor::ItemEditState::Reset()
    {
        m_isEditing = false;
        m_pTrackForEditedItem = nullptr;
        m_pEditedItem = nullptr;
        m_mode = ItemEditMode::None;
        m_originalTimeRange.Reset();
    }

    char const* TimelineEditor::ContextMenuState::GetContextMenuName() const
    {
        if ( m_pItem != nullptr )
        {
            return "ItemContextMenu";
        }

        if ( m_pTrack != nullptr )
        {
            return "TrackContextMenu";
        }

        return "EditorContextMenu";
    }

    void TimelineEditor::ContextMenuState::Reset()
    {
        m_pTrack = nullptr;
        m_pItem = nullptr;
        m_isOpen = false;
        m_playheadTimeForMouse = -1.0f;
    }

    //-------------------------------------------------------------------------

    TimelineEditor::TimelineEditor( IntRange const& inTimeRange )
        : m_timeRange( inTimeRange )
        , m_viewRange( inTimeRange )
    {
        KRG_ASSERT( inTimeRange.IsSetAndValid() );
    }

    TimelineEditor::~TimelineEditor()
    {
        m_trackContainer.Reset();
    }

    //-------------------------------------------------------------------------

    bool TimelineEditor::Serialize( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& objectValue )
    {
        ClearSelection();
        m_trackContainer.Reset();

        //-------------------------------------------------------------------------

        auto trackDataIter = objectValue.FindMember( TrackContainer::s_trackContainerKey );
        if ( trackDataIter == objectValue.MemberEnd() )
        {
            return false;
        }

        auto const& eventDataValueObject = trackDataIter->value;
        if ( !eventDataValueObject.IsArray() )
        {
            KRG_LOG_ERROR( "Timeline Editor", "Malformed track data" );
            return false;
        }

        if ( !m_trackContainer.Serialize( typeRegistry, eventDataValueObject ) )
        {
            KRG_LOG_ERROR( "Timeline Editor", "Failed to read track data" );
            return false;
        }

        return true;
    }

    void TimelineEditor::Serialize( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonWriter& writer )
    {
        writer.Key( TrackContainer::s_trackContainerKey );
        m_trackContainer.Serialize( typeRegistry, writer );
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::SetPlayState( PlayState newPlayState )
    {
        if ( m_playState == newPlayState )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( newPlayState == PlayState::Playing )
        {
            if ( m_playheadTime >= m_timeRange.m_end )
            {
                m_playheadTime = (float) m_timeRange.m_begin;
            }

            m_viewUpdateMode = ViewUpdateMode::TrackPlayhead;
            m_playState = PlayState::Playing;
        }
        else
        {
            m_viewUpdateMode = ViewUpdateMode::None;
            m_playState = PlayState::Paused;
        }

        OnPlayStateChanged();
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::HandleUserInput()
    {
        if ( m_mouseState.m_pHoveredItem != nullptr && m_mouseState.m_hoveredItemMode != ItemEditMode::Move )
        {
            ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );
        }

        // Context Menu
        //-------------------------------------------------------------------------

        bool const isMouseRightButtonReleased = ImGui::IsMouseReleased( ImGuiMouseButton_Right );
        if ( isMouseRightButtonReleased )
        {
            if ( m_mouseState.m_isHoveredOverTrackEditor )
            {
                m_contextMenuState.m_pItem = m_mouseState.m_pHoveredItem;
                m_contextMenuState.m_pTrack = m_mouseState.m_pHoveredTrack;
                m_contextMenuState.m_playheadTimeForMouse = m_mouseState.m_playheadTimeForMouse;
                ImGui::OpenPopupEx( GImGui->CurrentWindow->GetID( m_contextMenuState.GetContextMenuName() ) );
            }
        }

        DrawContextMenu();

        // Handle Selection
        //-------------------------------------------------------------------------

        bool const isMouseClicked = ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right );
        if ( isMouseClicked )
        {
            if ( m_mouseState.m_pHoveredItem != nullptr )
            {
                m_itemEditState.m_pEditedItem = m_mouseState.m_pHoveredItem;
                m_itemEditState.m_pTrackForEditedItem = m_mouseState.m_pHoveredTrack;
                m_itemEditState.m_originalTimeRange = m_mouseState.m_pHoveredItem->GetTimeRange();
                m_itemEditState.m_mode = m_mouseState.m_hoveredItemMode;

                SetSelection( m_mouseState.m_pHoveredItem );
            }
            else if( m_mouseState.m_pHoveredTrack != nullptr )
            {
                SetSelection( m_mouseState.m_pHoveredTrack );
            }
        }

        // Keyboard
        //-------------------------------------------------------------------------

        if ( ImGui::IsKeyReleased( ImGuiKey_Space ) )
        {
            SetPlayState( m_playState == PlayState::Playing ? PlayState::Paused : PlayState::Playing );
        }
        else if ( ImGui::IsKeyReleased( ImGuiKey_Enter ) )
        {
            for ( auto pTrack : m_selectedTracks )
            {
                m_trackContainer.CreateItem( pTrack, m_playheadTime );
            }
        }
        else  if ( ImGui::IsKeyReleased( ImGuiKey_Delete ) )
        {
            TVector<TrackItem*> copiedSelectedItems = m_selectedItems;
            ClearSelection();

            m_trackContainer.BeginModification();
            for ( auto pItem : copiedSelectedItems )
            {
                m_trackContainer.DeleteItem( pItem );
            }
            m_trackContainer.EndModification();
        }

        // Item Edition
        //-------------------------------------------------------------------------

        if ( m_itemEditState.m_mode != ItemEditMode::None )
        {
            KRG_ASSERT( m_itemEditState.m_pEditedItem != nullptr && m_itemEditState.m_pTrackForEditedItem != nullptr );

            if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
            {
                auto pEditedItem = m_itemEditState.m_pEditedItem;

                if ( !m_itemEditState.m_isEditing )
                {
                    m_trackContainer.BeginModification();
                    m_itemEditState.m_isEditing = true;
                }

                // Calculate valid range for modifications
                //-------------------------------------------------------------------------

                auto const floatViewRange = GetViewRangeAsFloatRange();
                FloatRange validEventRange = GetViewRangeAsFloatRange();
                for ( auto const pOtherItem : m_itemEditState.m_pTrackForEditedItem->GetItems() )
                {
                    FloatRange const otherItemTimeRange = pOtherItem->GetTimeRange();

                    if ( pOtherItem == pEditedItem )
                    {
                        continue;
                    }

                    if ( otherItemTimeRange.m_end < m_itemEditState.m_originalTimeRange.m_begin && otherItemTimeRange.m_end > validEventRange.m_begin )
                    {
                        validEventRange.m_begin = otherItemTimeRange.m_end;
                    }

                    if ( otherItemTimeRange.m_begin > m_itemEditState.m_originalTimeRange.m_end && otherItemTimeRange.m_begin < validEventRange.m_end )
                    {
                        validEventRange.m_end = otherItemTimeRange.m_begin;
                    }
                }

                // Prevent immediate items ending up on top of other items or outside the range
                if ( pEditedItem->IsImmediateItem() )
                {
                    validEventRange.m_end -= 1;
                }

                // Apply mouse delta to item
                //-------------------------------------------------------------------------

                if ( validEventRange.IsSetAndValid() )
                {
                    float const pixelOffset = ImGui::GetMouseDragDelta().x;
                    float const timeOffset = pixelOffset / m_pixelsPerFrame;

                    FloatRange editedItemTimeRange = pEditedItem->GetTimeRange();

                    if ( m_itemEditState.m_mode == ItemEditMode::Move )
                    {
                        // Create a new range to clamp the event start time to
                        FloatRange validEventStartRange = validEventRange;
                        if ( pEditedItem->IsDurationItem() )
                        {
                            validEventStartRange.m_end = validEventStartRange.m_end - m_itemEditState.m_originalTimeRange.GetLength();
                        }

                        float newTime = m_itemEditState.m_originalTimeRange.m_begin + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_begin = validEventStartRange.GetClampedValue( newTime );
                        editedItemTimeRange.m_end = editedItemTimeRange.m_begin + m_itemEditState.m_originalTimeRange.GetLength();
                        SetPlayheadPosition( editedItemTimeRange.m_begin );
                    }
                    else if ( m_itemEditState.m_mode == ItemEditMode::ResizeLeft )
                    {
                        float newTime = m_itemEditState.m_originalTimeRange.m_begin + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_begin = Math::Min( m_itemEditState.m_originalTimeRange.m_end - 1, newTime );
                        editedItemTimeRange.m_begin = Math::Max( validEventRange.m_begin, editedItemTimeRange.m_begin );
                        SetPlayheadPosition( editedItemTimeRange.m_begin );
                    }
                    else if ( m_itemEditState.m_mode == ItemEditMode::ResizeRight )
                    {
                        float newTime = m_itemEditState.m_originalTimeRange.m_end + timeOffset;
                        if ( m_isFrameSnappingEnabled )
                        {
                            newTime = Math::Round( newTime );
                        }

                        editedItemTimeRange.m_end = Math::Max( m_itemEditState.m_originalTimeRange.m_begin + 1, newTime );
                        editedItemTimeRange.m_end = Math::Min( validEventRange.m_end, editedItemTimeRange.m_end );
                        SetPlayheadPosition( editedItemTimeRange.m_end );
                    }

                    m_trackContainer.UpdateItemTimeRange( pEditedItem, editedItemTimeRange );
                }
            }
            else if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
            {
                if ( m_itemEditState.m_isEditing )
                {
                    m_trackContainer.EndModification();
                }

                m_itemEditState.Reset();
            }
        }

        // Zoom
        //-------------------------------------------------------------------------

        auto const& IO = ImGui::GetIO();
        float const mouseWheelDelta = IO.MouseWheel;
        if ( IO.KeyCtrl && mouseWheelDelta != 0 )
        {
            m_pixelsPerFrame = Math::Max( 1.0f, m_pixelsPerFrame + mouseWheelDelta );
        }

        //-------------------------------------------------------------------------

        m_mouseState.Reset();
    }

    void TimelineEditor::UpdateViewRange()
    {
        ImVec2 const canvasSize = ImGui::GetContentRegionAvail();
        float const trackAreaWidth = ( canvasSize.x - g_trackHeaderWidth );
        int32_t const maxVisibleFrames = Math::Max( 0, Math::FloorToInt( ( canvasSize.x - g_trackHeaderWidth ) / m_pixelsPerFrame ) );

        // Adjust visible range based on the canvas size
        if ( m_viewRange.GetLength() != maxVisibleFrames )
        {
            m_viewRange.m_end = m_viewRange.m_begin + maxVisibleFrames;
        }

        // Process any update requests
        //-------------------------------------------------------------------------

        switch ( m_viewUpdateMode )
        {
            case ViewUpdateMode::ShowFullTimeRange:
            {
                int32_t const timeRangeLength = m_timeRange.GetLength();
                m_pixelsPerFrame = Math::Max( 1.0f, Math::Floor( trackAreaWidth / timeRangeLength ) );
                m_viewRange = m_timeRange;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::GoToStart:
            {
                m_viewRange.m_begin = m_timeRange.m_begin;
                m_viewRange.m_end = maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_begin;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::GoToEnd:
            {
                m_viewRange.m_begin = Math::Max( m_timeRange.m_begin, m_timeRange.m_end - maxVisibleFrames );
                m_viewRange.m_end = m_viewRange.m_begin + maxVisibleFrames;
                m_playheadTime = (float) m_timeRange.m_end;
                m_viewUpdateMode = ViewUpdateMode::None;
            }
            break;

            case ViewUpdateMode::TrackPlayhead:
            {
                if ( !m_viewRange.ContainsInclusive( (int32_t) m_playheadTime ) )
                {
                    // If the playhead is in the last visible range
                    if ( m_playheadTime + maxVisibleFrames >= m_timeRange.m_end )
                    {
                        m_viewRange.m_begin = m_timeRange.m_end - maxVisibleFrames;
                        m_viewRange.m_end = m_timeRange.m_end;
                    }
                    else
                    {
                        m_viewRange.m_begin = (int) m_playheadTime;
                        m_viewRange.m_end = m_viewRange.m_begin + maxVisibleFrames;
                    }
                }
            }
            break;
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::SetPlayheadPosition( float inPosition )
    {
        m_playheadTime = GetTimeRangeAsFloatRange().GetClampedValue( inPosition );

        if ( m_isFrameSnappingEnabled )
        {
            m_playheadTime = Math::Round( m_playheadTime );
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::ClearSelection()
    {
        m_selectedItems.clear();
        m_itemEditState.Reset();
    }

    void TimelineEditor::SetSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedTracks.clear();
        m_selectedItems.clear();
        m_selectedItems.emplace_back( pItem );
    }

    void TimelineEditor::SetSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedItems.clear();
        m_selectedTracks.clear();
        m_selectedTracks.emplace_back( pTrack );
    }

    void TimelineEditor::AddToSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedTracks.clear();
        if ( !VectorContains( m_selectedItems, pItem ) )
        {
            m_selectedItems.emplace_back( pItem );
        }
    }

    void TimelineEditor::AddToSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedItems.clear();
        if ( !VectorContains( m_selectedTracks, pTrack ) )
        {
            m_selectedTracks.emplace_back( pTrack );
        }
    }

    void TimelineEditor::RemoveFromSelection( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        m_selectedItems.erase_first( pItem );
    }

    void TimelineEditor::RemoveFromSelection( Track* pTrack )
    {
        KRG_ASSERT( pTrack != nullptr );
        m_selectedTracks.erase_first( pTrack );
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::EnsureValidSelection()
    {
        for ( int32_t i = (int32_t) m_selectedItems.size() - 1; i >= 0; i-- )
        {
            if ( !m_trackContainer.Contains( m_selectedItems[i] ) )
            {
                m_selectedItems.erase_unsorted( m_selectedItems.begin() + i );
            }
        }

        //-------------------------------------------------------------------------

        for ( int32_t i = (int32_t) m_selectedTracks.size() - 1; i >= 0; i-- )
        {
            if ( !m_trackContainer.Contains( m_selectedTracks[i] ) )
            {
                m_selectedTracks.erase_unsorted( m_selectedTracks.begin() + i );
            }
        }
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::DrawTimelineControls( ImRect const& controlsRect )
    {
        auto const& style = ImGui::GetStyle();

        ImVec2 const controlsChildSize = controlsRect.GetSize() - ImVec2( 2 * style.ChildBorderSize, 2 * style.ChildBorderSize );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, (int) g_headerBackgroundColor );
        ImGui::BeginChild( "TimelineControls", controlsChildSize, false );

        ImVec2 buttonSize = ImVec2( 20, controlsChildSize.y - style.ChildBorderSize );
        constexpr static float const buttonSeperation = 2;

        // Playback controls
        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

        if ( ImGui::Button( KRG_ICON_REWIND "##GoToStart", buttonSize ) )
        {
            SetViewToStart();
        }

        ImGuiX::ItemTooltip( "Rewind to start" );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

        if ( m_playState == PlayState::Playing )
        {
            if ( ImGui::Button( KRG_ICON_PAUSE "##Pause", buttonSize ) )
            {
                SetPlayState( PlayState::Paused );
            }

            ImGuiX::ItemTooltip( "Pause" );
        }
        else // Paused
        {
            if ( ImGui::Button( KRG_ICON_PLAY "##Play", buttonSize ) )
            {
                SetPlayState( PlayState::Playing );
            }

            ImGuiX::ItemTooltip( "Play" );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

        if ( ImGui::Button( KRG_ICON_FAST_FORWARD "##GoToEnd", buttonSize ) )
        {
            SetViewToEnd();
        }

        ImGuiX::ItemTooltip( "Fast-forward to end" );

        // Options
        //-------------------------------------------------------------------------

        buttonSize = ImVec2( 22, controlsChildSize.y - style.ChildBorderSize );

        ImGuiX::VerticalSeparator( ImVec2( 9, -1 ) );

        if ( ImGuiX::ColoredButton( ImVec4( 0, 0, 0, 0 ), m_isFrameSnappingEnabled ? ImGuiX::Style::s_colorText : ImGuiX::Style::s_colorTextDisabled, KRG_ICON_CURSOR_DEFAULT_CLICK"##Snap", buttonSize) )
        {
            m_isFrameSnappingEnabled = !m_isFrameSnappingEnabled;
        }

        ImGuiX::ItemTooltip( m_isFrameSnappingEnabled ? "Disable frame snapping" : "Enable frame snapping" );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

        if ( IsLoopingEnabled() )
        {
            if ( ImGui::Button( KRG_ICON_INFINITY"##PlayOnce", buttonSize ) )
            {
                m_isLoopingEnabled = false;
            }

            ImGuiX::ItemTooltip( "Disable looping" );
        }
        else // Playing Once
        {
            if ( ImGui::Button( KRG_ICON_NUMERIC_1_CIRCLE"##Loop", buttonSize ) )
            {
                m_isLoopingEnabled = true;
            }

            ImGuiX::ItemTooltip( "Enable looping" );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, buttonSeperation );

        if ( ImGui::Button( KRG_ICON_FIT_TO_PAGE_OUTLINE"##ResetView", buttonSize ) )
        {
            ResetViewRange();
        }

        ImGuiX::ItemTooltip( "Reset View" );

        //-------------------------------------------------------------------------
        // Add tracks button
        //-------------------------------------------------------------------------

        ImVec2 const addTracksButtonSize( 26, -1 );

        ImGui::SameLine( 0, 0 );
        float const spacerWidth = ImGui::GetContentRegionAvail().x - addTracksButtonSize.x - 4;
        ImGui::SameLine( 0, spacerWidth );

        ImGui::PushStyleColor( ImGuiCol_Text, ImGuiX::ConvertColor( Colors::LimeGreen ).Value );
        bool const showAddTracksMenu = ImGuiX::FlatButton( KRG_ICON_MOVIE_PLUS"##AddTrack", addTracksButtonSize );
        ImGui::PopStyleColor();
        ImGuiX::ItemTooltip( "Add Track" );

        if( showAddTracksMenu )
        {
            ImGui::OpenPopup( "AddTracksPopup" );
        }

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::BeginPopup( "AddTracksPopup" ) )
        {
            DrawAddTracksMenu();
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void TimelineEditor::DrawTimeline( ImRect const& timelineRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        // Draw timeline
        //-------------------------------------------------------------------------

        int32_t const visibleRangeLength = m_viewRange.GetLength();

        int32_t numFramesForLargeInterval = 10;
        int32_t numFramesForSmallInterval = 1;
        while ( ( numFramesForLargeInterval * m_pixelsPerFrame ) < g_timelineMinimumWidthForLargeInterval )
        {
            numFramesForLargeInterval *= 2;
            numFramesForSmallInterval *= 2;
        };

        int32_t const NumFramesForMediumInterval = numFramesForLargeInterval / 2;

        //-------------------------------------------------------------------------

        float startPosX = timelineRect.GetTL().x;
        float startPosY = timelineRect.GetTL().y;
        float endPosX = timelineRect.GetBR().x;
        float endPosY = timelineRect.GetBR().y;

        for ( int32_t i = 0; i <= visibleRangeLength; i += numFramesForSmallInterval )
        {
            float const lineOffsetX = startPosX + Math::Round( i * m_pixelsPerFrame );
            if ( lineOffsetX < startPosX || lineOffsetX > endPosX )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            bool const isRangeEndLine = ( ( m_viewRange.m_begin + i ) == m_timeRange.m_end );
            bool const isLargeLine = ( ( i % numFramesForLargeInterval ) == 0 ) || ( i == m_viewRange.GetLength() || i == 0 ) || isRangeEndLine;
            bool const isMediumLine = ( i % NumFramesForMediumInterval ) == 0;

            //-------------------------------------------------------------------------

            if ( isLargeLine )
            {
                float lineOffsetY = g_timelineLargeLineOffset;
                ImColor lineColor = isRangeEndLine ? g_timelineRangeEndLineColor :  g_timelineLargeLineColor;

                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), lineColor, 1 );

                // Draw text label
                if ( !isRangeEndLine )
                {
                    InlineString label;
                    label.sprintf( "%d", m_viewRange.m_begin + i );
                    pDrawList->AddText( ImVec2( lineOffsetX + g_timelineLabelLeftPadding, startPosY ), g_headerLabelColor, label.c_str() );
                }
            }
            else if( isMediumLine )
            {
                float const lineOffsetY = g_timelineMediumLineOffset;
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, endPosY ), g_timelineMediumLineColor, 1 );
            }
            else // Small lines
            {
                float const lineOffsetY = g_timelineSmallLineOffset;
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + lineOffsetY ), ImVec2( lineOffsetX, startPosY + g_headerHeight ), g_timelineLargeLineColor, 1 );
                pDrawList->AddLine( ImVec2( lineOffsetX, startPosY + g_headerHeight ), ImVec2( lineOffsetX, endPosY ), g_timelineSmallLineColor, 1 );
            }
        }
    }

    void TimelineEditor::DrawPlayhead( ImRect const& playheadRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        float playheadPosX = playheadRect.GetTL().x;
        float playheadPosY = playheadRect.GetTL().y + g_headerHeight;

        //-------------------------------------------------------------------------

        constexpr static float const playHeadVerticalPadding = 3.0f;
        constexpr static float const playheadHalfWidth = 7.0f;

        float const playheadStartOffsetX = ConvertFramesToPixels( m_playheadTime - m_viewRange.m_begin );
        float const playheadHeight = g_headerHeight - ( playHeadVerticalPadding * 2 );
        ImVec2 const playheadPosition( playheadPosX + playheadStartOffsetX, playheadPosY - playHeadVerticalPadding );

        ImGui::SetCursorPos( playheadPosition - ImVec2{ playheadHalfWidth, playheadHeight } - ImGui::GetWindowPos() );
        ImGui::SetItemAllowOverlap();
        ImGui::InvisibleButton( "##Playhead", ImVec2( playheadHalfWidth * 2.0f, playheadHeight ) );

        // Handle Mouse
        //-------------------------------------------------------------------------

        ImVec2 const mousePos = ImGui::GetMousePos();

        // Draw tooltip
        bool const isMouseWithinPlayhead = playheadRect.Contains( mousePos ) && mousePos.y < ( playheadRect.Min.y + g_headerHeight );
        if ( isMouseWithinPlayhead )
        {
            ImGui::SetTooltip( "%.2f", m_playheadTime );
        }

        // If the mouse is clicked over the header, start dragging operation
        if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
        {
            if ( isMouseWithinPlayhead )
            {
                m_isDraggingPlayhead = true;
            }
        }
        else if ( m_isDraggingPlayhead && ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            // Do Nothing
        }
        else
        {
            m_isDraggingPlayhead = false;
        }

        // Dragging
        //-------------------------------------------------------------------------

        if ( m_isDraggingPlayhead )
        {
            // Any play head manipulation, switches back to paused
            if ( IsPlaying() )
            {
                SetPlayState( PlayState::Paused );
            }

            //-------------------------------------------------------------------------

            // The valid range for the playhead, limit it to the current view range but dont let it leave the actual time range
            FloatRange const playheadValidRange( (float) Math::Max( m_viewRange.m_begin, m_timeRange.m_begin ), (float) Math::Min( m_viewRange.m_end, m_timeRange.m_end ) );
            float newPlayheadTime = m_viewRange.m_begin + ConvertPixelsToFrames( mousePos.x - playheadRect.Min.x );
            newPlayheadTime = playheadValidRange.GetClampedValue( newPlayheadTime );
            SetPlayheadPosition( newPlayheadTime );
        }

        // Draw playhead
        //-------------------------------------------------------------------------

        if ( playheadStartOffsetX < 0 )
        {
            return;
        }

        ImColor const playheadColor = ImGui::IsItemHovered() ? g_playheadHoveredColor : g_playheadDefaultColor;
        ImVec2 const playheadMarkerPosition = playheadPosition + ImVec2( 0.5f, 0.5f );
        ImVec2 points[5] =
        {
            playheadMarkerPosition,
            playheadMarkerPosition + ImVec2{ -playheadHalfWidth, -playheadHeight / 2.0f },
            playheadMarkerPosition + ImVec2{ -playheadHalfWidth, -playheadHeight },
            playheadMarkerPosition + ImVec2{ playheadHalfWidth, -playheadHeight },
            playheadMarkerPosition + ImVec2{ playheadHalfWidth, -playheadHeight / 2.0f }
        };

        pDrawList->AddConvexPolyFilled( points, 5, ImColor( playheadColor ) );

        // Draw marker lines
        //-------------------------------------------------------------------------

        //pDrawList->AddLine( points[0], points[1], 0xFF00FF00 );
        pDrawList->AddLine( playheadPosition, ImVec2( playheadPosition.x, playheadRect.GetBR().y ), playheadColor );
    }

    void TimelineEditor::DrawTracks( ImRect const& fullTrackAreaRect )
    {
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        ImVec2 const mousePos = ImGui::GetMousePos();
        auto const floatViewRange = GetViewRangeAsFloatRange();

        // Are we hovered over the track editor
        m_mouseState.m_isHoveredOverTrackEditor = fullTrackAreaRect.Contains( mousePos );

        //-------------------------------------------------------------------------

        // TODO: Draw vertical scrollbar

        //-------------------------------------------------------------------------


        float trackStartY = fullTrackAreaRect.GetTL().y;
        int32_t const numTracks = m_trackContainer.GetNumTracks();
        for ( int32_t i = 0; i < numTracks; i++ )
        {
            auto pTrack = m_trackContainer.GetTrack( i );

            float const trackEndY = trackStartY + g_trackHeight;

            // Terminate loop as soon as a track is no longer visible
            if ( trackStartY > fullTrackAreaRect.GetBR().y )
            {
                break;
            }

            //-------------------------------------------------------------------------

            ImRect const trackRect( ImVec2( fullTrackAreaRect.GetTL().x, trackStartY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ) );
            ImRect const trackHeaderRect( ImVec2( fullTrackAreaRect.GetTL().x, trackStartY ), ImVec2( fullTrackAreaRect.GetTL().x + g_trackHeaderWidth, trackEndY ) );
            ImRect const trackAreaRect( ImVec2( fullTrackAreaRect.GetTL().x + g_trackHeaderWidth, trackStartY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ) );


            // Are we hovered over this track?
            if ( trackRect.Contains( mousePos ) )
            {
                m_mouseState.m_pHoveredTrack = pTrack;
            }

            // Calculate playhead position for the mouse pos
            if ( fullTrackAreaRect.Contains( mousePos ) )
            {
                FloatRange const playheadValidRange( (float) Math::Max( m_viewRange.m_begin, m_timeRange.m_begin ), (float) Math::Min( m_viewRange.m_end, m_timeRange.m_end ) );
                m_mouseState.m_playheadTimeForMouse = m_viewRange.m_begin + ConvertPixelsToFrames( mousePos.x - trackAreaRect.Min.x );
                m_mouseState.m_playheadTimeForMouse = playheadValidRange.GetClampedValue( m_mouseState.m_playheadTimeForMouse );
                m_mouseState.m_snappedPlayheadTimeForMouse = m_mouseState.m_playheadTimeForMouse;

                if ( m_isFrameSnappingEnabled )
                {
                    m_mouseState.m_snappedPlayheadTimeForMouse = Math::Round( m_mouseState.m_snappedPlayheadTimeForMouse );
                }
            }

            //-------------------------------------------------------------------------

            ImGui::PushID( pTrack );
            {
                // Draw track header
                //-------------------------------------------------------------------------

                int32_t const headerFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 1 ) );
                if( ImGui::BeginChild( "Track Header", ImVec2( g_trackHeaderWidth, g_trackHeight ), false, headerFlags ) )
                {
                    pTrack->DrawHeader( trackHeaderRect );
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

                // Track Item Area
                //-------------------------------------------------------------------------

                // Draw track highlight
                if ( IsSelected( pTrack ) )
                {
                    pDrawList->AddRectFilled( trackAreaRect.GetTL(), trackAreaRect.GetBR(), g_selectedTrackColor );
                }

                // Draw items
                //-------------------------------------------------------------------------

                ImGui::PushClipRect( trackAreaRect.GetTL(), trackAreaRect.GetBR(), false );

                for ( auto const pItem : pTrack->GetItems() )
                {
                    FloatRange const itemTimeRange = pItem->GetTimeRange();
                    if ( !floatViewRange.Overlaps( itemTimeRange ) )
                    {
                        continue;
                    }

                    //-------------------------------------------------------------------------

                    auto GetItemColor = [this, pItem] ( bool isItemHovered )
                    {
                        ImVec4 itemColor = pItem->GetColor().ToFloat4();

                        // Set selection color first
                        if ( IsSelected( pItem ) )
                        {
                            itemColor = (Float4) itemColor * 1.45f;
                        }

                        // Apply modifiers based on current state
                        if ( pItem == m_itemEditState.m_pEditedItem )
                        {
                            itemColor = (Float4) itemColor * 1.1f;
                        }
                        else if ( isItemHovered )
                        {
                            itemColor = (Float4) itemColor * 1.15f;
                        }

                        return (uint32_t) ImColor( itemColor );
                    };

                    //-------------------------------------------------------------------------

                    ImFont const* pTinyFont = ImGuiX::GetFont( ImGuiX::Font::Small );

                    if ( pItem->IsImmediateItem() )
                    {
                        float const itemPosX = trackAreaRect.GetTL().x + ( itemTimeRange.m_begin - m_viewRange.m_begin ) * m_pixelsPerFrame;
                        float const itemPosTopY = trackAreaRect.GetTL().y + g_itemMarginY;
                        float const itemPosBottomY = trackAreaRect.GetBR().y + g_itemMarginY;

                        ImVec2 const base( itemPosX, itemPosBottomY );
                        ImVec2 const topLeft( itemPosX - g_immediateItemHalfWidth, itemPosTopY );
                        ImVec2 const topRight( itemPosX + g_immediateItemHalfWidth, itemPosTopY );

                        ImRect const itemRect( ImVec2( itemPosX - g_immediateItemHalfWidth, itemPosTopY ), ImVec2( itemPosX + g_immediateItemHalfWidth, itemPosBottomY ) );
                        bool const isItemHovered = itemRect.Contains(mousePos);

                        if ( isItemHovered )
                        {
                            m_mouseState.m_pHoveredItem = pItem;
                            m_mouseState.m_hoveredItemMode = ItemEditMode::Move;
                        }

                        pDrawList->AddTriangleFilled( topLeft, topRight, base, GetItemColor( isItemHovered ) );

                        InlineString const itemLabel = pItem->GetLabel();
                        pDrawList->AddText( pTinyFont, pTinyFont->FontSize, topRight + ImVec2( 5, 1 ), 0xFF000000, itemLabel.c_str() );
                        pDrawList->AddText( pTinyFont, pTinyFont->FontSize, topRight + ImVec2( 4, 0 ), ImColor( ImGuiX::Style::s_colorText ), itemLabel.c_str() );
                        
                    }
                    else
                    {
                        float itemEndTime = itemTimeRange.m_end;
                        float const itemStartX = trackAreaRect.GetTL().x + ( itemTimeRange.m_begin - m_viewRange.m_begin ) * m_pixelsPerFrame;
                        float const itemEndX = trackAreaRect.GetTL().x + ( itemEndTime - m_viewRange.m_begin ) * m_pixelsPerFrame;
                        float const itemStartY = trackAreaRect.GetTL().y + g_itemMarginY;
                        float const itemEndY = trackAreaRect.GetBR().y - g_itemMarginY;

                        ImVec2 const itemStart( itemStartX, itemStartY );
                        ImVec2 const itemEnd( itemEndX, itemEndY );

                        bool const isItemHovered = ImRect( itemStart, itemEnd ).Contains( mousePos );
                        bool const isHoveredOverLeftHandle = ( !pItem->IsImmediateItem() && isItemHovered ) ? ImRect( itemStart, ImVec2( itemStart.x + g_itemHandleWidth, itemEnd.y ) ).Contains( mousePos ) : false;
                        bool const isHoveredOverRightHandle = ( !pItem->IsImmediateItem() && isItemHovered && !isHoveredOverLeftHandle ) ? ImRect( ImVec2( itemEnd.x - g_itemHandleWidth, itemStart.y ), itemEnd ).Contains( mousePos ) : false;

                        if ( isItemHovered )
                        {
                            m_mouseState.m_pHoveredItem = pItem;

                            if ( isHoveredOverLeftHandle )
                            {
                                m_mouseState.m_hoveredItemMode = ItemEditMode::ResizeLeft;
                            }
                            else if ( isHoveredOverRightHandle )
                            {
                                m_mouseState.m_hoveredItemMode = ItemEditMode::ResizeRight;
                            }
                            else
                            {
                                m_mouseState.m_hoveredItemMode = ItemEditMode::Move;
                            }
                        }

                        pDrawList->AddRectFilled( itemStart, itemEnd, GetItemColor( isItemHovered ), pItem->IsImmediateItem() ? 0.0f : 4.0f);

                        InlineString const itemLabel = pItem->GetLabel();
                        pDrawList->AddText( pTinyFont, pTinyFont->FontSize, itemStart + ImVec2( 5, 1 ), 0xFF000000, itemLabel.c_str() );
                        pDrawList->AddText( pTinyFont, pTinyFont->FontSize, itemStart + ImVec2( 4, 0 ), ImColor( ImGuiX::Style::s_colorText ), itemLabel.c_str() );
                    }
                }

                ImGui::PopClipRect();

                // Draw track separator
                //-------------------------------------------------------------------------

                pDrawList->AddLine( ImVec2( fullTrackAreaRect.GetTL().x, trackEndY ), ImVec2( fullTrackAreaRect.GetBR().x, trackEndY ), g_trackSeparatorColor );
                trackStartY = trackEndY + 1;
            }
            ImGui::PopID();
        }
    }

    void TimelineEditor::DrawContextMenu()
    {
        m_contextMenuState.m_isOpen = false;

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );

        // Items
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "ItemContextMenu" ) )
        {
            if ( m_trackContainer.Contains( m_contextMenuState.m_pItem ) )
            {
                bool const shouldDeleteItem = ImGui::MenuItem( "Delete Item" );

                //-------------------------------------------------------------------------

                if ( m_contextMenuState.m_pItem->HasContextMenu() )
                {
                    ImGui::Separator();
                    m_contextMenuState.m_pItem->DrawContextMenu();
                }

                //-------------------------------------------------------------------------

                if ( shouldDeleteItem )
                {
                    ClearSelection();
                    m_trackContainer.DeleteItem( m_contextMenuState.m_pItem );
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        // Tracks
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "TrackContextMenu" ) )
        {
            if ( m_trackContainer.Contains( m_contextMenuState.m_pTrack ) )
            {
                if ( ImGui::MenuItem( "Add Item" ) )
                {
                    // Calculate the appropriate item start time
                    float itemStartTime = ( m_contextMenuState.m_playheadTimeForMouse < 0.0f ) ? m_playheadTime : m_contextMenuState.m_playheadTimeForMouse;

                    if ( m_isFrameSnappingEnabled )
                    {
                        itemStartTime = Math::Floor( itemStartTime );
                    }

                    m_trackContainer.CreateItem( m_contextMenuState.m_pTrack, itemStartTime );
                }

                bool const shouldDeleteTrack = ImGui::MenuItem( "Delete Track" );

                //-------------------------------------------------------------------------

                if ( m_contextMenuState.m_pTrack->HasContextMenu() )
                {
                    ImGui::Separator();
                    m_contextMenuState.m_pTrack->DrawContextMenu( m_trackContainer.m_tracks, m_contextMenuState.m_playheadTimeForMouse < 0.0f ? m_playheadTime : m_contextMenuState.m_playheadTimeForMouse );
                }

                //-------------------------------------------------------------------------

                if ( shouldDeleteTrack )
                {
                    ClearSelection();
                    DeleteTrack( m_contextMenuState.m_pTrack );
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        // General
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( "EditorContextMenu" ) )
        {
            if ( ImGui::BeginMenu( "Add Track" ) )
            {
                DrawAddTracksMenu();
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
            m_contextMenuState.m_isOpen = true;
        }

        //-------------------------------------------------------------------------

        ImGui::PopStyleVar();
    }

    //-------------------------------------------------------------------------

    void TimelineEditor::Draw()
    {
        ImVec2 const canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 const canvasSize = ImGui::GetContentRegionAvail();

        bool const requiresHorizontalScrollBar = ( m_viewRange.m_begin > m_timeRange.m_begin || m_viewRange.m_end < m_timeRange.m_end );
        float const horizontalScrollBarHeight = requiresHorizontalScrollBar ? g_horizontalScrollbarHeight : 0.0f;

        //-------------------------------------------------------------------------
        // Manage play state and input
        //-------------------------------------------------------------------------

        if ( IsPlaying() )
        {
            if ( !m_isLoopingEnabled && m_playheadTime >= m_timeRange.m_end )
            {
                SetPlayState( PlayState::Paused );
            }
        }

        // Remove any invalid ptrs from the current selection
        EnsureValidSelection();

        // Handle the user input based on the current keyboard state and the mouse state from the last frame
        if ( ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows ) )
        {
            HandleUserInput();
        }

        // Update the view range, to ensure that we track the playhead, etc...
        UpdateViewRange();

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, 0 );
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 2 ) );
        ImGui::BeginChildFrame( 99, canvasSize );

        //-------------------------------------------------------------------------
        // Header
        //-------------------------------------------------------------------------

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddRectFilled( canvasPos, ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + g_headerHeight ), g_headerBackgroundColor, 0 );

        ImRect const timelineControlsRect( canvasPos, ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y + g_headerHeight - 1 ) );

        ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) g_headerBackgroundColor );
        DrawTimelineControls( timelineControlsRect );
        ImGui::PopStyleColor();

        ImRect const timelineRect( ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );
        DrawTimeline( timelineRect );

        //-------------------------------------------------------------------------
        // Tracks
        //-------------------------------------------------------------------------

        ImRect const trackAreaRect( ImVec2( canvasPos.x, canvasPos.y + g_headerHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ) );
        DrawTracks( trackAreaRect );

        //-------------------------------------------------------------------------
        // Playhead
        //-------------------------------------------------------------------------

        DrawPlayhead( timelineRect );

        //-------------------------------------------------------------------------
        // Horizontal Scrollbar
        //-------------------------------------------------------------------------

        ImRect const horizontalScrollBarRect( ImVec2( canvasPos.x + g_trackHeaderWidth, canvasPos.y + canvasSize.y - horizontalScrollBarHeight ), ImVec2( canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y ) );
        int64_t const currentViewSize = Math::RoundToInt( m_viewRange.GetLength() * m_pixelsPerFrame );
        int64_t const totalContentSizeNeeded = Math::RoundToInt( m_timeRange.GetLength() * m_pixelsPerFrame );
        int64_t scrollbarPosition = Math::RoundToInt( m_viewRange.m_begin * m_pixelsPerFrame );

        ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
        ImGuiID const horizontalScrollBarID = pWindow->GetID( "#TimelineScrollbarY" );
        if( ImGui::ScrollbarEx( horizontalScrollBarRect, horizontalScrollBarID, ImGuiAxis_X, &scrollbarPosition, currentViewSize, totalContentSizeNeeded, 0 ) )
        {
            int32_t const viewRangeOriginalLength = m_viewRange.GetLength();
            m_viewRange.m_begin = Math::FloorToInt( scrollbarPosition / m_pixelsPerFrame );
            m_viewRange.m_end = m_viewRange.m_begin + viewRangeOriginalLength;
        }

        //-------------------------------------------------------------------------

        ImGui::EndChildFrame();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
}