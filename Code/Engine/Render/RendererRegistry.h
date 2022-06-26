#pragma once

#include "IRenderer.h"
#include "System/Types/Containers.h"
#include "System/Systems.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_API RendererRegistry : public ISystem
    {
    public:

        KRG_SYSTEM_ID( RendererRegistry );

    public:

        RendererRegistry() {}
        ~RendererRegistry();

        void RegisterRenderer( IRenderer* pRenderer );
        void UnregisterRenderer( IRenderer* pRenderer );

        template<typename T>
        inline T* GetRenderer() const
        {
            for ( auto pRenderer : m_registeredRenderers )
            {
                if ( pRenderer->GetRendererID() == T::RendererID )
                {
                    return static_cast<T*>( pRenderer );
                }
            }

            KRG_UNREACHABLE_CODE();
            return nullptr;
        }

        inline TInlineVector<IRenderer*, 20>& GetRegisteredRenderers() { return m_registeredRenderers; }

    private:

        TInlineVector<IRenderer*, 20>     m_registeredRenderers;
    };
}