#pragma once
#if KRG_ENABLE_NAVPOWER
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Systems/ISystem.h"

#include <bfxSystemSpace.h>
#include <bfxPlannerSpace.h>

//-------------------------------------------------------------------------

namespace KRG::Drawing { class DrawingSystem; }

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    //-------------------------------------------------------------------------
    // Type Conversion
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE Float3 FromBfx( bfx::Vector3 const& v )
    {
        return Float3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE Quaternion FromBfx( bfx::Quaternion const& v )
    {
        return Quaternion( v.m_x, v.m_y, v.m_z, v.m_w );
    }

    KRG_FORCE_INLINE Color FromBfx( bfx::Color const& color )
    {
        return Color( uint8_t( color.m_r * 255 ), uint8_t( color.m_g * 255 ), uint8_t( color.m_b ), uint8_t( color.m_a * 255 ) );
    }

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float3 const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float4 const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Vector const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Quaternion ToBfx( Quaternion const& q )
    {
        bfx::Quaternion quaternion;
        quaternion.m_x = q.m_x;
        quaternion.m_y = q.m_y;
        quaternion.m_z = q.m_z;
        quaternion.m_w = q.m_w;
        return quaternion;
    }

    KRG_FORCE_INLINE bfx::Color ToBfx( Color const& color )
    {
        return bfx::Color( color.m_byteColor.m_r / 255.0f, color.m_byteColor.m_g / 255.0f, color.m_byteColor.m_b / 255.0f, color.m_byteColor.m_a / 255.0f );
    }

    //-------------------------------------------------------------------------
    // Debug drawing
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    class NavpowerRenderer final : public bfx::Renderer
    {

    public:

        void SetDebugDrawingSystem( Drawing::DrawingSystem* pDebugDrawingSystem )
        {
            m_pDebugDrawingSystem = pDebugDrawingSystem;
        }

        void Reset()
        {
            m_statsPos = Float2( 5, 20 );
        }

        inline bool IsDepthTestEnabled() const { return m_depthTestEnabled; }
        inline void SetDepthTestState( bool isEnabled ) { m_depthTestEnabled = isEnabled; }

    private:

        virtual void DrawLineList( bfx::LineSegment const* pLines, uint32_t numLines, bfx::Color const& color ) override;
        virtual void DrawTriList( bfx::Triangle const* pTris, uint32_t numTris, bfx::Color const& color ) override;
        virtual void DrawString( bfx::Color const& color, char const* str ) override;
        virtual void DrawString( bfx::Color const& color, bfx::Vector3 const& pos, char const* str ) override;

    private:

        Drawing::DrawingSystem*                     m_pDebugDrawingSystem = nullptr;
        Float2                                      m_statsPos = Float2::Zero;
        bool                                        m_depthTestEnabled = true;
    };
    #endif

    //-------------------------------------------------------------------------
    // System and Allocator
    //-------------------------------------------------------------------------

    class NavPowerAllocator final : public bfx::CustomAllocator
    {
        virtual void* CustomMalloc( size_t size ) override final { return KRG::Alloc( size ); }
        virtual void* CustomAlignedMalloc( uint32_t alignment, size_t size ) override final { return KRG::Alloc( size, alignment ); }
        virtual void CustomFree( void* ptr ) override final { KRG::Free( ptr ); }
        virtual bool IsThreadSafe() const override final { return true; }
        virtual const char* GetName() const override { return "KRUGER"; }
    };

    class KRG_ENGINE_NAVMESH_API NavmeshSystem : public ISystem
    {
        friend class NavmeshWorldSystem;

    public:

        KRG_SYSTEM_ID( NavmeshSystem );

    public:

        NavmeshSystem() = default;

    private:

        NavPowerAllocator                               m_allocator;
    };
}
#endif