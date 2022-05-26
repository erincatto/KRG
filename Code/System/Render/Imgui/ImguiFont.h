#pragma once
#include "../_Module/API.h"
#include "System/Render/ThirdParty/imgui/imgui.h"
#include "System/Core/Types/IntegralTypes.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

struct ImFont;
struct ImVec4;

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    enum class Font : uint8
    {
        Small,
        SmallBold,
        Medium,
        MediumBold,
        Large,
        LargeBold,
        Huge,
        HugeBold,

        NumFonts,
        Default = Medium,
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_RENDER_API SystemFonts
    {
        static ImFont* s_fonts[(int32) Font::NumFonts];
    };

    KRG_FORCE_INLINE ImFont* GetFont( Font font ) { return SystemFonts::s_fonts[(int32) font]; }

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RENDER_API [[nodiscard]] ScopedFont
    {
    public:

        ScopedFont( Font font );
        ScopedFont( Font font, ImColor const& color );
        explicit ScopedFont( Font font, Color const& color ) : ScopedFont( font, (ImColor) IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a ) ) {}
        ~ScopedFont();

    private:

        bool m_colorApplied = false;
    };

    //-------------------------------------------------------------------------

    KRG_SYSTEM_RENDER_API inline void PushFont( Font font ) 
    {
        ImGui::PushFont( SystemFonts::s_fonts[(int8) font] ); 
    }

    KRG_SYSTEM_RENDER_API inline void PushFontAndColor( Font font, ImColor const& color ) 
    {
        ImGui::PushFont( SystemFonts::s_fonts[(int8) font] );
        ImGui::PushStyleColor( ImGuiCol_Text, color.Value );
    }

    KRG_SYSTEM_RENDER_API inline void PushFontAndColor( Font font, Color const& color )
    {
        ImGui::PushFont( SystemFonts::s_fonts[(int8) font] );
        ImGui::PushStyleColor( ImGuiCol_Text, IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a ) );
    }
}

//-------------------------------------------------------------------------

#include "MaterialDesignIcons.h"

#endif