#pragma once

#include "Engine/_Module/API.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "Engine/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_API MeshComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( MeshComponent );

    public:

        inline MeshComponent() = default;
        inline MeshComponent( StringID name ) : SpatialEntityComponent( name ) {}

        // Does this component have a valid mesh set
        virtual bool HasMeshResourceSet() const = 0;

        // Materials
        //-------------------------------------------------------------------------

        inline int32_t GetNumRequiredMaterials() const { return (int32_t) GetDefaultMaterials().size(); }
        inline TVector<Material const*> const& GetMaterials() const { return m_materials; }
        void SetMaterialOverride( int32_t materialIdx, ResourceID materialResourceID );

    protected:

        virtual void Initialize() override;
        virtual void Shutdown() override;

        // Get the default materials for the set mesh
        virtual TVector<TResourcePtr<Material>> const& GetDefaultMaterials() const = 0;

    private:

        void UpdateMaterialCache();

    private:

        // Any user material overrides
        KRG_EXPOSE TVector<TResourcePtr<Material>>                          m_materialOverrides;

        // The final set of materials to use for this component
        TVector<Material const*>                                            m_materials;
    };
}