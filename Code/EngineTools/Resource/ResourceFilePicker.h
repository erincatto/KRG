#pragma once
#include "EngineTools/_Module/API.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class ToolsContext;
}

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceDatabase;

    //-------------------------------------------------------------------------

    class KRG_ENGINETOOLS_API ResourceFilePicker final
    {
        struct PickerOption
        {
            PickerOption( ResourceID const& resourceID );

            ResourceID                  m_resourceID;
            String                      m_filename;
            String                      m_parentFolder;
        };

    public:

        ResourceFilePicker( ToolsContext const& toolsContext );

        // Get the raw resource data path
        FileSystem::Path const& GetRawResourceDirectoryPath() const;

        // Draws the resource field - returns true if the value is changed
        bool DrawPicker( ResourceTypeID resourceTypeID, ResourceID const* pResourceID );

        // Get the selected ID - Only use if DrawPicker returns true
        inline ResourceID const GetSelectedResourceID() const { return m_selectedID; }

    private:

        // Draw the selection dialog, returns true if the dialog is closed
        bool DrawDialog( ResourceTypeID resourceTypeID, ResourceID const* pResourceID );

        void RefreshResourceList( ResourceTypeID resourceTypeID );

    private:

        ToolsContext const&             m_toolsContext;
        char                            m_filterBuffer[256];
        TVector<PickerOption>           m_knownResourceIDs;
        TVector<PickerOption>           m_filteredResourceIDs;
        ResourceID                      m_selectedID;
        bool                            m_initializeFocus = false;
    };
}