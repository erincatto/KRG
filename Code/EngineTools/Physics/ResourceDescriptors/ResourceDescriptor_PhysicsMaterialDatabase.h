#pragma  once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/ResourceDescriptor.h"
#include "Engine/Physics/PhysicsMaterial.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_ENGINETOOLS_API PhysicsMaterialDatabaseResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( PhysicsMaterialDatabaseResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return PhysicsMaterialDatabase::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE TVector<ResourcePath>         m_materialLibraries;
    };
}