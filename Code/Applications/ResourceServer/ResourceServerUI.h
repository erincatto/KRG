#pragma once
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceServer;
    class CompilationRequest;

    //-------------------------------------------------------------------------

    class ResourceServerUI final
    {

    public:

        ResourceServerUI( ResourceServer& resourceServer );
        void Draw();

    private:

        void DrawRequests();
        void DrawWorkerStatus();
        void DrawServerInfo();
        void DrawConnectionInfo();
        void DrawPackagingControls();

    private:

        ResourceServer&                         m_resourceServer;
        CompilationRequest const*               m_pSelectedCompletedRequest = nullptr;
        TVector<CompilationRequest const*>      m_combinedRequests; // Temp working memory for UI
    };
}
