#pragma once

#include "System/_Module/API.h"
#include "System/Types/String.h"
#include "System/Types/Arrays.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    struct KRG_SYSTEM_API Settings
    {
        static char const s_pathDelimiter;
    };

    // General functions
    //-------------------------------------------------------------------------

    // Converts a file path (relative, short, etc...) to a full path
    KRG_SYSTEM_API bool GetFullPathString( char const* pPath, String& outPath );

    // Converts a file path (relative, short, etc...) to a full path
    KRG_FORCE_INLINE String GetFullPathString( char const* pPath )
    {
        String fullPath;
        GetFullPathString( pPath, fullPath );
        return fullPath;
    }

    // Gets the path with the correct case - only relevant for case-insensitive platforms
    // Note: This is an expensive function that will create a file handle so be careful
    KRG_SYSTEM_API bool GetCorrectCaseForPath( char const* pPath, String& outPath );

    // Does the path point to an existing file/directory
    KRG_SYSTEM_API bool Exists( char const* pPath );

    // Does the path point to an existing file/directory
    KRG_FORCE_INLINE bool Exists( String const& filePath ) { return Exists( filePath.c_str() ); }

    // Is this file directory read only
    KRG_SYSTEM_API bool IsReadOnly( char const* pPath );

    // File functions
    //-------------------------------------------------------------------------
    // These function also perform additional validation based on the path type

    // Does the path refer to an existing file
    KRG_SYSTEM_API bool IsExistingFile( char const* pPath );

    // Does the path refer to an existing file
    KRG_FORCE_INLINE bool IsExistingFile( String const& filePath ) { return IsExistingFile( filePath.c_str() ); }

    KRG_SYSTEM_API bool IsFileReadOnly( char const* filePath );
    KRG_FORCE_INLINE bool IsFileReadOnly( String const& filePath ) { return IsFileReadOnly( filePath.c_str() ); }

    KRG_SYSTEM_API uint64_t GetFileModifiedTime( char const* filePath );
    KRG_FORCE_INLINE uint64_t GetFileModifiedTime( String const& filePath ) { return GetFileModifiedTime( filePath.c_str() ); }
    
    KRG_SYSTEM_API bool EraseFile( char const* filePath );
    KRG_FORCE_INLINE bool EraseFile( String const& filePath ) { return EraseFile( filePath.c_str() ); }

    KRG_SYSTEM_API bool LoadFile( char const* filePath, TVector<uint8_t>& fileData );
    KRG_FORCE_INLINE bool LoadFile( String const& filePath, TVector<uint8_t>& fileData ) { return LoadFile( filePath.c_str(), fileData ); }
    
    // Directory Functions
    //-------------------------------------------------------------------------

    // Does the path refer to an existing directory
    KRG_SYSTEM_API bool IsExistingDirectory( char const* pPath );

    // Does the path refer to an existing directory
    KRG_FORCE_INLINE bool IsExistingDirectory( String const& filePath ) { return IsExistingDirectory( filePath.c_str() ); }

    KRG_SYSTEM_API bool CreateDir( char const* path );
    KRG_FORCE_INLINE bool CreateDir( String const& path ) { return CreateDir( path.c_str() ); }

    KRG_SYSTEM_API bool EraseDir( char const* path );
    KRG_FORCE_INLINE bool EraseDir( String const& path ) { return EraseDir( path.c_str() ); }
}