#include "ResourceCompiler_RenderMesh.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderMesh.h"
#include "Tools/Core/Resource/RawAssets/RawMesh.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

#include <MeshOptimizer.h>

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void MeshCompiler::TransferMeshGeometry( RawAssets::RawMesh const& rawMesh, Mesh& mesh, int32 maxBoneInfluences ) const
    {
        KRG_ASSERT( maxBoneInfluences > 0 && maxBoneInfluences <= 8 );
        KRG_ASSERT( maxBoneInfluences <= 4 );// TEMP HACK - we dont support 8 bones for now

        // Merge all mesh geometries into the main vertex and index buffers
        //-------------------------------------------------------------------------

        uint32 numVertices = 0;
        uint32 numIndices = 0;

        for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
        {
            // Add sub-mesh record
            mesh.m_sections.push_back( Mesh::GeometrySection( StringID( geometrySection.m_name ), numIndices, (uint32) geometrySection.m_indices.size() ) );

            for ( auto idx : geometrySection.m_indices )
            {
                mesh.m_indices.push_back( numVertices + idx );
            }

            numIndices += (uint32) geometrySection.m_indices.size();
            numVertices += (uint32) geometrySection.m_vertices.size();
        }

        // Copy mesh vertex data
        //-------------------------------------------------------------------------

        AABB meshAlignedBounds;
        int32 vertexSize = 0;
        int32 vertexBufferSize = 0;

        if ( rawMesh.IsSkeletalMesh() )
        {
            mesh.m_vertexBuffer.m_vertexFormat = VertexFormat::SkeletalMesh;
            vertexSize = VertexLayoutRegistry::GetDescriptorForFormat( mesh.m_vertexBuffer.m_vertexFormat ).m_byteSize;
            KRG_ASSERT( vertexSize == sizeof( SkeletalMeshVertex ) );

            vertexBufferSize = vertexSize * numVertices;
            mesh.m_vertices.resize( vertexBufferSize );
            auto pVertexMemory = (SkeletalMeshVertex*) mesh.m_vertices.data();

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                for ( auto const& vert : geometrySection.m_vertices )
                {
                    auto pVertex = new( pVertexMemory ) SkeletalMeshVertex();

                    pVertex->m_position = vert.m_position;
                    pVertex->m_normal = vert.m_normal;
                    pVertex->m_UV0 = vert.m_texCoords[0];
                    pVertex->m_UV1 = ( geometrySection.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

                    int32 const numInfluences = (int32) vert.m_boneIndices.size();
                    KRG_ASSERT( numInfluences <= maxBoneInfluences && vert.m_boneIndices.size() == vert.m_boneWeights.size() );

                    pVertex->m_boneIndices = Int4( InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex );
                    pVertex->m_boneWeights = Float4::Zero;

                    int32 const numWeights = Math::Min( numInfluences, 4 );
                    for ( int32 i = 0; i < numWeights; i++ )
                    {
                        pVertex->m_boneIndices[i] = vert.m_boneIndices[i];
                        pVertex->m_boneWeights[i] = vert.m_boneWeights[i];
                    }

                    // Re-enable this when we add back support for 8 bone weights
                    /*pVertex->m_boneIndices1 = Int4( InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex );
                    pVertex->m_boneWeights1 = Float4::Zero;
                    for ( int32 i = 4; i < numInfluences; i++ )
                    {
                        pVertex->m_boneIndices1[i - 4] = vert.m_boneIndices[i];
                        pVertex->m_boneWeights1[i - 4] = vert.m_boneWeights[i];
                    }*/

                    pVertexMemory++;

                    //-------------------------------------------------------------------------

                    meshAlignedBounds.AddPoint( vert.m_position );
                }
            }
        }
        else
        {
            mesh.m_vertexBuffer.m_vertexFormat = VertexFormat::StaticMesh;
            vertexSize = VertexLayoutRegistry::GetDescriptorForFormat( mesh.m_vertexBuffer.m_vertexFormat ).m_byteSize;
            KRG_ASSERT( vertexSize == sizeof( StaticMeshVertex ) );

            vertexBufferSize = vertexSize * numVertices;
            mesh.m_vertices.resize( vertexBufferSize );
            auto pVertexMemory = (StaticMeshVertex*) mesh.m_vertices.data();

            for ( auto const& geometrySection : rawMesh.GetGeometrySections() )
            {
                for ( auto const& vert : geometrySection.m_vertices )
                {
                    auto pVertex = new( pVertexMemory ) StaticMeshVertex();

                    pVertex->m_position = vert.m_position;
                    pVertex->m_normal = vert.m_normal;
                    pVertex->m_UV0 = vert.m_texCoords[0];
                    pVertex->m_UV1 = ( geometrySection.GetNumUVChannels() > 1 ) ? vert.m_texCoords[1] : vert.m_texCoords[0];

                    pVertexMemory++;

                    //-------------------------------------------------------------------------

                    meshAlignedBounds.AddPoint( vert.m_position );
                }
            }
        }

        // Set Mesh buffer descriptors
        //-------------------------------------------------------------------------

        mesh.m_vertexBuffer.m_byteStride = vertexSize;
        mesh.m_vertexBuffer.m_byteSize = vertexBufferSize;
        mesh.m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
        mesh.m_vertexBuffer.m_usage = RenderBuffer::Usage::GPU_only;

        mesh.m_indexBuffer.m_byteStride = sizeof( uint32 );
        mesh.m_indexBuffer.m_byteSize = (uint32) mesh.m_indices.size() * sizeof( uint32 );
        mesh.m_indexBuffer.m_type = RenderBuffer::Type::Index;
        mesh.m_indexBuffer.m_usage = RenderBuffer::Usage::GPU_only;

        // Calculate bounding volume
        //-------------------------------------------------------------------------
        // TODO: use real algorithm to find minimal bounding box, for now use AABB

        mesh.m_bounds = OBB( meshAlignedBounds );
    }

    void MeshCompiler::OptimizeMeshGeometry( Mesh& mesh ) const
    {
        size_t const vertexSize = (size_t) mesh.m_vertexBuffer.m_byteStride;
        size_t const numVertices = (size_t) mesh.GetNumVertices();

        meshopt_optimizeVertexCache( &mesh.m_indices[0], &mesh.m_indices[0], mesh.m_indices.size(), mesh.m_vertices.size() );

        // Reorder indices for overdraw, balancing overdraw and vertex cache efficiency
        const float kThreshold = 1.01f; // allow up to 1% worse ACMR to get more reordering opportunities for overdraw
        meshopt_optimizeOverdraw( &mesh.m_indices[0], &mesh.m_indices[0], mesh.m_indices.size(), (float*) &mesh.m_vertices[0], numVertices, vertexSize, kThreshold );

        // Vertex fetch optimization should go last as it depends on the final index order
        meshopt_optimizeVertexFetch( &mesh.m_vertices[0], &mesh.m_indices[0], mesh.m_indices.size(), &mesh.m_vertices[0], numVertices, vertexSize );
    }

    void MeshCompiler::SetMeshDefaultMaterials( MeshResourceDescriptor const& descriptor, Mesh& mesh ) const
    {
        mesh.m_materials.reserve( mesh.GetNumSections() );

        for ( auto i = 0u; i < mesh.GetNumSections(); i++ )
        {
            if ( i < descriptor.m_materials.size() )
            {
                mesh.m_materials.push_back( descriptor.m_materials[i] );
            }
            else
            {
                mesh.m_materials.push_back( TResourcePtr<Material>() );
            }
        }
    }

    void MeshCompiler::SetMeshInstallDependencies( Mesh const& mesh, Resource::ResourceHeader& hdr ) const
    {
        for ( auto i = 0u; i < mesh.m_materials.size(); i++ )
        {
            if ( mesh.m_materials[i].IsValid() )
            {
                hdr.AddInstallDependency( mesh.m_materials[i].GetResourceID() );
            }
        }
    }

    //-------------------------------------------------------------------------

    StaticMeshCompiler::StaticMeshCompiler()
        : MeshCompiler( "StaticMeshCompiler", s_version )
    {
        m_outputTypes.push_back( StaticMesh::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult StaticMeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        StaticMeshResourceDescriptor resourceDescriptor;
        if ( !Resource::ResourceDescriptor::TryReadFromFile( ctx.m_typeRegistry, ctx.m_inputFilePath, resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Read mesh data
        //-------------------------------------------------------------------------

        FileSystem::Path meshFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_meshPath, meshFilePath ) )
        {
            return Error( "Invalid mesh data path: %s", resourceDescriptor.m_meshPath.c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        TUniquePtr<RawAssets::RawMesh> pRawMesh = RawAssets::ReadStaticMesh( readerCtx, meshFilePath, resourceDescriptor.m_meshName );
        if ( pRawMesh == nullptr )
        {
            return Error( "Failed to read mesh from source file" );
        }

        KRG_ASSERT( pRawMesh->IsValid() );

        // Reflect FBX data into runtime format
        //-------------------------------------------------------------------------

        StaticMesh staticMesh;
        TransferMeshGeometry( *pRawMesh, staticMesh, 4 );
        OptimizeMeshGeometry( staticMesh );
        SetMeshDefaultMaterials( resourceDescriptor, staticMesh );

        // Serialize
        //-------------------------------------------------------------------------

        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, StaticMesh::GetStaticResourceTypeID() );

            SetMeshInstallDependencies( staticMesh, hdr );

            archive << hdr << staticMesh;

            if ( pRawMesh->HasWarnings() )
            {
                return CompilationSucceededWithWarnings( ctx );
            }
            else
            {
                return CompilationSucceeded( ctx );
            }
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    //-------------------------------------------------------------------------

    SkeletalMeshCompiler::SkeletalMeshCompiler()
        : MeshCompiler( "SkeletalMeshCompiler", s_version )
    {
        m_outputTypes.push_back( SkeletalMesh::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult SkeletalMeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        SkeletalMeshResourceDescriptor resourceDescriptor;
        if ( !Resource::ResourceDescriptor::TryReadFromFile( ctx.m_typeRegistry, ctx.m_inputFilePath, resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        bool const isSkeletalMesh = ( ctx.m_resourceID.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID() );

        // Read mesh data
        //-------------------------------------------------------------------------

        FileSystem::Path meshFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_meshPath, meshFilePath ) )
        {
            return Error( "Invalid mesh data path: %s", resourceDescriptor.m_meshPath.c_str() );
        }

        RawAssets::ReaderContext readerCtx = { [this]( char const* pString ) { Warning( pString ); }, [this] ( char const* pString ) { Error( pString ); } };
        int32 const maxBoneInfluences = 4;
        TUniquePtr<RawAssets::RawMesh> pRawMesh = RawAssets::ReadSkeletalMesh( readerCtx, meshFilePath, maxBoneInfluences );
        if ( pRawMesh == nullptr )
        {
            return Error( "Failed to read mesh from source file" );
        }

        KRG_ASSERT( pRawMesh->IsValid() );

        // Reflect FBX data into runtime format
        //-------------------------------------------------------------------------

        SkeletalMesh skeletalMesh;
        TransferMeshGeometry( *pRawMesh, skeletalMesh, maxBoneInfluences );
        OptimizeMeshGeometry( skeletalMesh );
        TransferSkeletalMeshData( *pRawMesh, skeletalMesh );
        SetMeshDefaultMaterials( resourceDescriptor, skeletalMesh );

        // Serialize
        //-------------------------------------------------------------------------

        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, SkeletalMesh::GetStaticResourceTypeID() );

            SetMeshInstallDependencies( skeletalMesh, hdr );

            archive << hdr << skeletalMesh;

            if ( pRawMesh->HasWarnings() )
            {
                return CompilationSucceededWithWarnings( ctx );
            }
            else
            {
                return CompilationSucceeded( ctx );
            }
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    void SkeletalMeshCompiler::TransferSkeletalMeshData( RawAssets::RawMesh const& rawMesh, SkeletalMesh& mesh ) const
    {
        KRG_ASSERT( rawMesh.IsSkeletalMesh() );

        auto const& skeleton = rawMesh.GetSkeleton();
        auto const& boneData = skeleton.GetBoneData();

        auto const numBones = skeleton.GetNumBones();
        for ( auto i = 0; i < numBones; i++ )
        {
            mesh.m_boneIDs.push_back( boneData[i].m_name );
            mesh.m_parentBoneIndices.push_back( boneData[i].m_parentBoneIdx );
            mesh.m_bindPose.push_back( boneData[i].m_globalTransform );
            mesh.m_inverseBindPose.push_back( boneData[i].m_globalTransform.GetInverse() );
        }
    }
}