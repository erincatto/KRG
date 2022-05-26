#include "ImguiSystem.h"

#if KRG_DEVELOPMENT_TOOLS
#include "ImguiFont.h"
#include "ImguiStyle.h"
#include "System/Render/Fonts/FontDecompressor.h"
#include "System/Render/Fonts/FontData_Lexend.h"
#include "System/Render/Fonts/FontData_MaterialDesign.h"

//-------------------------------------------------------------------------

#if _WIN32
#include "Platform/ImguiPlatform_Win32.h"
#include "misc/freetype/imgui_freetype.h"
#endif

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    bool ImguiSystem::Initialize( String const& iniFilename, Render::RenderDevice* pRenderDevice, bool enableViewports )
    {
        ImGui::CreateContext();

        //-------------------------------------------------------------------------

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        if ( enableViewports )
        {
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        // Set render device in the user data
        KRG_ASSERT( pRenderDevice != nullptr );
        io.BackendRendererUserData = pRenderDevice;

        //-------------------------------------------------------------------------

        m_iniFilename = iniFilename;
        if ( !m_iniFilename.empty() )
        {
            io.IniFilename = m_iniFilename.c_str();
        }

        //-------------------------------------------------------------------------

        Platform::InitializePlatform();
        InitializeFonts();

        //-------------------------------------------------------------------------

        Style::Apply();

        return true;
    }

    void ImguiSystem::Shutdown()
    {
        for ( int i = 0; i < (int8) Font::NumFonts; i++ )
        {
            SystemFonts::s_fonts[i] = nullptr;
        }

        Platform::ShutdownPlatform();
        ImGui::DestroyContext();
    }

    void ImguiSystem::InitializeFonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Decompress fonts
        //-------------------------------------------------------------------------

        TVector<Byte> fontData, boldFontData;
        Fonts::GetDecompressedFontData( Fonts::Lexend::Regular::GetData(), fontData );
        Fonts::GetDecompressedFontData( Fonts::Lexend::Bold::GetData(), boldFontData );

        ImWchar const icons_ranges[] = { KRG_ICONRANGE_MIN, KRG_ICONRANGE_MAX, 0 };
        TVector<Byte> iconFontData;
        Fonts::GetDecompressedFontData( (Byte const*) Fonts::MaterialDesignIcons::GetData(), iconFontData );

        // Base font configs
        //-------------------------------------------------------------------------

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;

        ImFontConfig iconFontConfig;
        iconFontConfig.FontDataOwnedByAtlas = false;
        iconFontConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoAutoHint | ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
        iconFontConfig.MergeMode = true;
        iconFontConfig.RasterizerMultiply = 1.5f;

        auto CreateFont = [&] ( TVector<Byte>& fontData, float fontSize, float iconFontSize, Font fontID, char const* pName, ImVec2 const& glyphOffset = ImVec2( 0, 0 ) )
        {
            Printf( fontConfig.Name, 40, pName );
            ImFont* pFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), (int32) fontData.size(), fontSize, &fontConfig );
            SystemFonts::s_fonts[(uint8) fontID] = pFont;

            iconFontConfig.GlyphOffset = glyphOffset;
            io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), (int32) iconFontData.size(), iconFontSize, &iconFontConfig, icons_ranges );
        };

        CreateFont( fontData, 14, 18, Font::Small, "Small", ImVec2( 0, 3 ) );
        CreateFont( boldFontData, 14, 18, Font::SmallBold, "Small Bold", ImVec2( 0, 3 ) );

        CreateFont( fontData, 16, 20, Font::Medium, "Medium", ImVec2( 0, 3 ) );
        CreateFont( boldFontData, 16, 20, Font::MediumBold, "Medium Bold", ImVec2( 0, 3 ) );

        CreateFont( fontData, 18, 22, Font::Large, "Large", ImVec2( 0, 4 ) );
        CreateFont( boldFontData, 18, 22, Font::LargeBold, "Large Bold", ImVec2( 0, 4 ) );

        CreateFont( fontData, 36, 40, Font::Huge, "Huge", ImVec2( 0, 4 ) );
        CreateFont( boldFontData, 36, 40, Font::HugeBold, "Huge Bold", ImVec2( 0, 4 ) );

        // Build font atlas
        //-------------------------------------------------------------------------

        io.Fonts->Build();
        KRG_ASSERT( io.Fonts->IsBuilt() );

        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Small]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::SmallBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Medium]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::MediumBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Large]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::LargeBold]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::Huge]->IsLoaded() );
        KRG_ASSERT( SystemFonts::s_fonts[(uint8) Font::HugeBold]->IsLoaded() );
        #endif

        io.FontDefault = SystemFonts::s_fonts[(uint8) Font::Medium];
    }

    //-------------------------------------------------------------------------

    void ImguiSystem::StartFrame( float deltaTime )
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = deltaTime;

        Platform::UpdateDisplayInformation();
        Platform::UpdateInputInformation();

        ImGui::NewFrame();
    }

    void ImguiSystem::EndFrame()
    {
        ImGui::EndFrame();
    }
}
#endif