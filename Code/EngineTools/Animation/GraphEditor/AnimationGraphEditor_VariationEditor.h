#pragma once
#include "System/Types/StringID.h"
#include "EngineTools/Resource/ResourceFilePicker.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }
namespace KRG::Resource { class ResourceDatabase; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class VariationHierarchy;
    class GraphEditorContext;

    //-------------------------------------------------------------------------

    class GraphVariationEditor final
    {
        enum class OperationType
        {
            None,
            Create,
            Rename,
            Delete
        };

    public:

        GraphVariationEditor( GraphEditorContext& editorContext );

        void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName );

    private:

        void DrawVariationTree();
        void DrawVariationTreeNode( VariationHierarchy const& variationHierarchy, StringID variationID );
        void DrawOverridesTable();

        void StartCreate( StringID variationID );
        void StartRename( StringID variationID );
        void StartDelete( StringID variationID );
        void DrawActiveOperationUI();

    private:

        GraphEditorContext&                 m_editorContext;
        StringID                            m_activeOperationVariationID;
        char                                m_buffer[255] = {0};
        Resource::ResourceFilePicker        m_resourcePicker;
        OperationType                       m_activeOperation = OperationType::None;
    };
}