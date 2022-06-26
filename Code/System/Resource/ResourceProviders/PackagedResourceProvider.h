#pragma once

#include "System/Resource/ResourceProvider.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceSettings;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_API PackagedResourceProvider final : public ResourceProvider
    {

    public:

        using ResourceProvider::ResourceProvider;
        virtual bool IsReady() const override final;

    private:

        virtual bool Initialize() override;
        virtual void RequestRawResource( ResourceRequest* pRequest ) override;
        virtual void CancelRequest( ResourceRequest* pRequest ) override;
    };
}