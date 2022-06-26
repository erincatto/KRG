#pragma once

#include "System/_Module/API.h"
#include "System/Types/String.h"
#include "System/Types/Containers.h"
#include "System/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::Log
{
    enum class Severity
    {
        Message = 0,
        Warning,
        Error,
        FatalError,
    };

    struct LogEntry
    {
        String      m_timestamp;
        String      m_message;
        String      m_channel;
        String      m_filename;
        uint32_t      m_lineNumber;
        Severity    m_severity;
    };

    // Lifetime
    //-------------------------------------------------------------------------

    KRG_SYSTEM_API void Initialize();
    KRG_SYSTEM_API void Shutdown();
    KRG_SYSTEM_API bool IsInitialized();

    // Logging
    //-------------------------------------------------------------------------

    KRG_SYSTEM_API void AddEntry( Severity severity, char const* pChannel, char const* pFilename, int pLineNumber, char const* pMessageFormat, ... );
    KRG_SYSTEM_API void AddEntryVarArgs( Severity severity, char const* pChannel, char const* pFilename, int pLineNumber, char const* pMessageFormat, va_list args );
    KRG_SYSTEM_API TVector<LogEntry> const& GetLogEntries();
    KRG_SYSTEM_API int32_t GetNumWarnings();
    KRG_SYSTEM_API int32_t GetNumErrors();

    // Output
    //-------------------------------------------------------------------------

    KRG_SYSTEM_API void SaveToFile( FileSystem::Path const& logFilePath );

    // Warnings and errors
    //-------------------------------------------------------------------------

    KRG_SYSTEM_API bool HasFatalErrorOccurred();
    KRG_SYSTEM_API LogEntry const& GetFatalError();

    // Transfers a list of unhandled warnings and errors - useful for displaying all errors for a given frame.
    // Calling this function will clear the list of warnings and errors.
    KRG_SYSTEM_API TVector<LogEntry> GetUnhandledWarningsAndErrors();
}

//-------------------------------------------------------------------------

#define KRG_LOG_MESSAGE( CHANNEL, ... ) KRG::Log::AddEntry( KRG::Log::Severity::Message, CHANNEL, __FILE__, __LINE__, __VA_ARGS__ )
#define KRG_LOG_WARNING( CHANNEL, ... ) KRG::Log::AddEntry( KRG::Log::Severity::Warning, CHANNEL, __FILE__, __LINE__, __VA_ARGS__ )
#define KRG_LOG_ERROR( CHANNEL, ... ) KRG::Log::AddEntry( KRG::Log::Severity::Error, CHANNEL, __FILE__, __LINE__, __VA_ARGS__ )
#define KRG_LOG_FATAL_ERROR( CHANNEL, ... ) KRG::Log::AddEntry( KRG::Log::Severity::FatalError, CHANNEL, __FILE__, __LINE__, __VA_ARGS__ ); KRG_HALT()