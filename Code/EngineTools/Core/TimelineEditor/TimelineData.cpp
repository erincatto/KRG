#include "TimelineData.h"
#include "imgui.h"
#include "System/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    Track::~Track()
    {
        for ( auto pItem : m_items )
        {
            KRG::Delete( pItem );
        }

        m_items.clear();
    }

    void Track::DrawHeader( ImRect const& headerRect )
    {
        ImGui::SameLine( 10 );
        ImGui::AlignTextToFramePadding();
        ImGui::Text( GetLabel() );
    }

    bool Track::IsDirty() const
    {
        if ( m_isDirty )
        {
            return true;
        }

        for ( auto const pItem : m_items )
        {
            if ( pItem->IsDirty() )
            {
                return true;
            }
        }

        return false;
    }

    void Track::ClearDirtyFlags()
    {
        m_isDirty = false;

        for ( auto pItem : m_items )
        {
            pItem->ClearDirtyFlag();
        }
    }
}

//-------------------------------------------------------------------------

namespace KRG::Timeline
{
    TEvent<TrackContainer*> TrackContainer::s_onEndModification;
    TEvent<TrackContainer*> TrackContainer::s_onBeginModification;

    //-------------------------------------------------------------------------

    void TrackContainer::Reset()
    {
        for ( auto pTrack : m_tracks )
        {
            KRG::Delete( pTrack );
        }
        m_tracks.clear();
        m_isDirty = false;
    }

    bool TrackContainer::IsDirty() const
    {
        if ( m_isDirty )
        {
            return true;
        }

        for ( auto const pTrack : m_tracks )
        {
            if ( pTrack->IsDirty() )
            {
                return true;
            }
        }

        return false;
    }

    void TrackContainer::ClearDirtyFlags()
    {
        m_isDirty = false;

        for ( auto pTrack : m_tracks )
        {
            pTrack->ClearDirtyFlags();
        }
    }

    bool TrackContainer::Contains( Track const* pTrack ) const
    {
        return eastl::find( m_tracks.begin(), m_tracks.end(), pTrack ) != m_tracks.end();
    }

    bool TrackContainer::Contains( TrackItem const* pItem ) const
    {
        for ( auto pTrack : m_tracks )
        {
            if ( pTrack->Contains( pItem ) )
            {
                return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------

    void TrackContainer::BeginModification()
    {
        if ( m_beginModificationCallCount == 0 )
        {
            if ( s_onBeginModification.HasBoundUsers() )
            {
                s_onBeginModification.Execute( this );
            }
        }
        m_beginModificationCallCount++;
    }

    void TrackContainer::EndModification()
    {
        KRG_ASSERT( m_beginModificationCallCount > 0 );
        m_beginModificationCallCount--;

        if ( m_beginModificationCallCount == 0 )
        {
            if ( s_onEndModification.HasBoundUsers() )
            {
                s_onEndModification.Execute( this );
            }
        }

        m_isDirty = true;
    }

    //-------------------------------------------------------------------------

    void TrackContainer::DeleteTrack( Track* pTrack )
    {
        KRG_ASSERT( Contains( pTrack ) );

        BeginModification();
        m_tracks.erase_first( pTrack );
        KRG::Delete( pTrack );
        EndModification();
    }

    void TrackContainer::CreateItem( Track* pTrack, float itemStartTime )
    {
        KRG_ASSERT( pTrack != nullptr );
        KRG_ASSERT( Contains( pTrack ) );

        BeginModification();
        pTrack->CreateItem( itemStartTime );
        EndModification();
    }

    void TrackContainer::UpdateItemTimeRange( TrackItem* pItem, FloatRange const& newTimeRange )
    {
        KRG_ASSERT( pItem != nullptr );
        KRG_ASSERT( Contains( pItem ) );
        KRG_ASSERT( newTimeRange.IsSetAndValid() );

        BeginModification();
        pItem->SetTimeRange( newTimeRange );
        EndModification();
    }

    void TrackContainer::DeleteItem( TrackItem* pItem )
    {
        KRG_ASSERT( pItem != nullptr );
        KRG_ASSERT( Contains( pItem ) );

        BeginModification();

        for ( auto pTrack : m_tracks )
        {
            if ( pTrack->DeleteItem( pItem ) )
            {
                break;
            }
        }

        EndModification();
    }

    //-------------------------------------------------------------------------

    bool TrackContainer::Serialize( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonValue const& dataObjectValue )
    {
        auto FreeTrackData = [this] ()
        {
            for ( auto pTrack : m_tracks )
            {
                KRG::Delete( pTrack );
            }
            m_tracks.clear();
        };

        FreeTrackData();

        //-------------------------------------------------------------------------

        if ( !dataObjectValue.IsArray() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        bool failed = false;

        for ( auto const& trackObjectValue : dataObjectValue.GetArray() )
        {
            auto trackDataIter = trackObjectValue.FindMember( "TrackData" );
            if ( trackDataIter == trackObjectValue.MemberEnd() )
            {
                failed = true;
                break;
            }

            // Create track
            Track* pTrack = Serialization::CreateAndReadNativeType<Track>( typeRegistry, trackDataIter->value );
            m_tracks.emplace_back( pTrack );

            // Custom serialization
            pTrack->SerializeCustom( typeRegistry, trackObjectValue );

            //-------------------------------------------------------------------------

            auto itemArrayIter = trackObjectValue.FindMember( "Items" );
            if ( itemArrayIter == trackObjectValue.MemberEnd() || !itemArrayIter->value.IsArray() )
            {
                failed = true;
                break;
            }

            //-------------------------------------------------------------------------

            // Deserialize items
            for ( auto const& itemObjectValue : itemArrayIter->value.GetArray() )
            {
                auto itemDataIter = itemObjectValue.FindMember( "ItemData" );
                auto startTimeDataIter = itemObjectValue.FindMember( "StartTime" );
                auto endTimeDataIter = itemObjectValue.FindMember( "EndTime" );
                if ( itemDataIter == itemObjectValue.MemberEnd() || startTimeDataIter == itemObjectValue.MemberEnd() || endTimeDataIter == itemObjectValue.MemberEnd() )
                {
                    failed = true;
                    break;
                }

                // Create item
                TrackItem* pItem = Serialization::CreateAndReadNativeType<TrackItem>( typeRegistry, itemDataIter->value );
                pTrack->m_items.emplace_back( pItem );

                // Custom serialization
                pItem->SerializeCustom( typeRegistry, itemObjectValue );

                // Set time range
                FloatRange itemTimeRange;
                itemTimeRange.m_begin = (float) startTimeDataIter->value.GetDouble();
                itemTimeRange.m_end = (float) endTimeDataIter->value.GetDouble();
                pItem->InitializeTimeRange( itemTimeRange );
            }

            if ( failed )
            {
                break;
            }
        }

        //-------------------------------------------------------------------------

        if ( failed )
        {
            FreeTrackData();
            m_isDirty = false;
            return false;
        }

        ClearDirtyFlags();
        return true;
    }

    void TrackContainer::Serialize( TypeSystem::TypeRegistry const& typeRegistry, Serialization::JsonWriter& writer )
    {
        writer.StartArray();

        //-------------------------------------------------------------------------

        for ( auto pTrack : m_tracks )
        {
            writer.StartObject();

            // Track
            //-------------------------------------------------------------------------

            writer.Key( "TrackData" );
            Serialization::WriteNativeType( typeRegistry, pTrack, writer );

            pTrack->SerializeCustom( typeRegistry, writer );

            // Items
            //-------------------------------------------------------------------------

            writer.Key( "Items" );
            writer.StartArray();
            {
                for ( auto pItem : pTrack->m_items )
                {
                    writer.StartObject();
                    {
                        writer.Key( "ItemData" );
                        Serialization::WriteNativeType( typeRegistry, pItem, writer );

                        auto const& itemTimeRange = pItem->GetTimeRange();
                        writer.Key( "StartTime" );
                        writer.Double( itemTimeRange.m_begin );

                        writer.Key( "EndTime" );
                        writer.Double( itemTimeRange.m_end );

                        pItem->SerializeCustom( typeRegistry, writer );
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();

            //-------------------------------------------------------------------------

            writer.EndObject();
        }

        //-------------------------------------------------------------------------

        writer.EndArray();
        ClearDirtyFlags();
    }
}