#include "ResourceEditorApplication_Win32.h"
#include "Resource.h"
#include "iniparser/krg_ini.h"
#include "Applications/Shared/Win32/SharedHelpers_Win32.h"
#include "System/Input/InputSystem.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"
#include <tchar.h>
#include <windows.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditorApplication::ResourceEditorApplication( HINSTANCE hInstance )
        : Win32Application( hInstance, "Kruger Resource Editor", IDI_EDITOR_ICON )
        , m_editorEngine( TFunction<bool( String const& error )>( [this] ( String const& error )-> bool  { return FatalError( error ); } ) )
    {
        // This is hard-coded since this is always the default map for the editor. There is no need for this to change.
        m_editorEngine.m_startupMap = DataPath( "data://Editor/EditorMap.map" );
    }

    bool ResourceEditorApplication::ReadSettings( int32 argc, char** argv )
    {
        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        FileSystem::Path const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
        if ( !m_editorEngine.m_settingsRegistry.LoadFromFile( iniPath ) )
        {
            return FatalError( "Failed to read required settings from INI file" );
        }

        return true;
    }

    bool ResourceEditorApplication::Initialize()
    {
        if ( !EnsureResourceServerIsRunning( m_editorEngine.m_resourceSettings.m_resourceServerExecutablePath ) )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        // Initialize editor
        //-------------------------------------------------------------------------

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        if ( !m_editorEngine.Initialize( m_applicationNameNoWhitespace, windowDimensions ) )
        {
            return FatalError( "Failed to initialize engine" );
        }

        return true;
    }

    bool ResourceEditorApplication::Shutdown()
    {
        return m_editorEngine.Shutdown();
    }  

    bool ResourceEditorApplication::ApplicationLoop()
    {
        // Uncomment for live editing of ImguiTheme
        ImGuiX::Theme::ApplyTheme();
        return m_editorEngine.Update();
    }

    //-------------------------------------------------------------------------

    LRESULT ResourceEditorApplication::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        if ( !IsInitialized() )
        {
            return DefWindowProc( hWnd, message, wParam, lParam );
        }

        //-------------------------------------------------------------------------

        switch ( message )
        {
            case WM_SIZE:
            {
                uint32 width = LOWORD( lParam );
                uint32 height = HIWORD( lParam );
                if ( width > 0 && height > 0 )
                {
                    m_editorEngine.UpdateMainWindowSize( Int2( width, height ) );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_SETFOCUS:
            {
                m_editorEngine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            case WM_KILLFOCUS:
            {
                m_editorEngine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            // Forward input messages to the input system
            case WM_INPUT:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
            case WM_MOUSEMOVE:
            {
                m_editorEngine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            case WM_CLOSE:
            case WM_QUIT:
            {
                RequestExit();
            }
            break;

            //-------------------------------------------------------------------------

            case WM_DESTROY:
            {
                PostQuitMessage( 0 );
            }
            break;
        }

        // ImGui specific message processing
        //-------------------------------------------------------------------------

        auto const imguiResult = m_editorEngine.m_pImguiSystem->ProcessInput( { hWnd, message, wParam, lParam } );
        if ( imguiResult != 0 )
        {
            return imguiResult;
        }

        // Default
        //-------------------------------------------------------------------------

        return DefWindowProc( hWnd, message, wParam, lParam );
    }
}

//-------------------------------------------------------------------------

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );
    #endif

    //-------------------------------------------------------------------------

    KRG::ApplicationGlobalState globalState;
    KRG::ResourceEditorApplication editorApplication( hInstance );
    return editorApplication.Run( __argc, __argv );
}