#pragma once

#include "EngineTools/_Module/API.h"
#include "EngineTools/Resource/ResourceDescriptor.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawMesh; }

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class Mesh;

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API MeshResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( MeshResourceDescriptor );

        // The path to the mesh source file
        KRG_EXPOSE ResourcePath                         m_meshPath;

        // Default materials - TODO: extract from FBX
        KRG_EXPOSE TVector<TResourcePtr<Material>>      m_materials;

        // Optional value that specifies the specific sub-mesh to compile, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
        KRG_EXPOSE String                               m_meshName;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API StaticMeshResourceDescriptor : public MeshResourceDescriptor
    {
        KRG_REGISTER_TYPE( StaticMeshResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return StaticMesh::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINETOOLS_API SkeletalMeshResourceDescriptor : public MeshResourceDescriptor
    {
        KRG_REGISTER_TYPE( SkeletalMeshResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return SkeletalMesh::GetStaticResourceTypeID(); }
    };
}