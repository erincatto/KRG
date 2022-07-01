#pragma once
#include "System/Types/UUID.h"
#include "System/Types/Arrays.h"
#include "EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class GraphEditorContext;

    //-------------------------------------------------------------------------

    class GraphCompilationLog
    {
    public:

        GraphCompilationLog( GraphEditorContext& editorContext );

        // Draw the control parameter editor, returns true if there is a request the calling code needs to fulfill i.e. navigation
        void UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName );

        // Update the compilation results to show
        void UpdateCompilationResults( TVector<NodeCompilationLogEntry> const& compilationLog ) { m_compilationLog = compilationLog; }

    private:

        GraphEditorContext&                             m_editorContext;
        TVector<NodeCompilationLogEntry>                m_compilationLog;
    };
}