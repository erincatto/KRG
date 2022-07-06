#pragma once

#include "EngineTools/Core/ToolsContext.h"
#include "System/KRG.h"
#include "System/TypeSystem/CoreTypeIDs.h"

//-------------------------------------------------------------------------

namespace KRG::Resource { class ResourceFilePicker; }

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class PropertyInfo;
    class TypeRegistry;

    //-------------------------------------------------------------------------

    class PropertyEditor
    {

    public:

        PropertyEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* pPropertyInstance );
        virtual ~PropertyEditor() = default;

        // Returns true if the value was updated
        bool UpdateAndDraw();

        // Actually set the instance property value from the working copy
        virtual void UpdateInstanceValue() = 0;

        // Called to reset the working copy value to that of the actual instance
        virtual void ResetWorkingCopy() = 0;

    protected:

        virtual void HandleExternalUpdate() = 0;
        virtual bool InternalUpdateAndDraw() = 0;

    protected:

        ToolsContext const*     m_pToolsContext;
        PropertyInfo const&     m_propertyInfo;
        uint8_t*                m_pPropertyInstance;
        CoreTypeID const        m_coreType;
    };

    //-------------------------------------------------------------------------

    PropertyEditor* CreatePropertyEditor( ToolsContext const* pToolsContext, Resource::ResourceFilePicker& resourcePicker, PropertyInfo const& propertyInfo, uint8_t* pPropertyInstance );
}