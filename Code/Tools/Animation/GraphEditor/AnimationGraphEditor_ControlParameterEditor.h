#pragma once
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphControlParameterEditor
    {
        enum class OperationType
        {
            None,
            Rename,
            Delete
        };

    public:

        GraphControlParameterEditor( GraphEditorModel& graphModel );

        void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName );

    private:

        void DrawAddParameterUI();

        void StartRename( UUID parameterID );
        void StartDelete( UUID parameterID );
        void DrawActiveOperationUI();

    private:

        GraphEditorModel&           m_graphModel;
        UUID                        m_currentOperationParameterID;
        char                        m_buffer[255];
        OperationType               m_activeOperation;
    };
}