#include "ResourceCompiler_AnimationGraph.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Definition.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationGraph.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphCompiler::AnimationGraphCompiler()
        : Resource::Compiler( "GraphCompiler", s_version )
    {
        m_outputTypes.push_back( GraphDefinition::GetStaticResourceTypeID() );
        m_outputTypes.push_back( GraphVariation::GetStaticResourceTypeID() );
        m_virtualTypes.push_back( GraphDataSet::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationGraphCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        auto const resourceTypeID = ctx.m_resourceID.GetResourceTypeID();
        if ( resourceTypeID == GraphDefinition::GetStaticResourceTypeID() )
        {
            return CompileGraphDefinition( ctx );
        }
        else if ( resourceTypeID == GraphVariation::GetStaticResourceTypeID() )
        {
            return CompileGraphVariation( ctx );
        }

        KRG_UNREACHABLE_CODE();
        return CompilationFailed( ctx );
    }

    Resource::CompilationResult AnimationGraphCompiler::CompileGraphDefinition( Resource::CompileContext const& ctx ) const
    {
        JsonReader jsonReader;
        if ( !jsonReader.ReadFromFile( ctx.m_inputFilePath ) )
        {
            return Error( "Failed to read animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        EditorGraphDefinition editorGraph;
        if ( !editorGraph.LoadFromJson( ctx.m_typeRegistry, jsonReader.GetDocument() ) )
        {
            return Error( "Malformed animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Compile
        //-------------------------------------------------------------------------

        GraphDefinitionCompiler definitionCompiler;
        if ( !definitionCompiler.CompileGraph( editorGraph ) )
        {
            // Dump log
            for ( auto const& logEntry : definitionCompiler.GetLog() )
            {
                if ( logEntry.m_severity == Log::Severity::Error )
                {
                    Error( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Warning )
                {
                    Warning( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Message )
                {
                    Message( "%s", logEntry.m_message.c_str() );
                }
            }

            return Resource::CompilationResult::Failure;
        }

        // Serialize
        //-------------------------------------------------------------------------

        auto pRuntimeGraph = definitionCompiler.GetCompiledGraph();

        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, GraphDefinition::GetStaticResourceTypeID() );
            archive << *pRuntimeGraph;
            
            // Serialize node paths only in non-shipping builds
            #if !KRG_CONFIGURATION_SHIPPING
            archive << pRuntimeGraph->m_nodePaths;
            #endif

            // Node settings type descs
            TypeSystem::TypeDescriptorCollection settingsTypeDescriptors;
            for ( auto pSettings : pRuntimeGraph->m_nodeSettings )
            {
                settingsTypeDescriptors.m_descriptors.emplace_back( TypeSystem::TypeDescriptor( ctx.m_typeRegistry, pSettings ) );
            }
            archive << settingsTypeDescriptors;

            // Node settings data
            cereal::BinaryOutputArchive& settingsArchive = *archive.GetOutputArchive();
            for ( auto pSettings : pRuntimeGraph->m_nodeSettings )
            {
                pSettings->Save( settingsArchive );
            }

            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    Resource::CompilationResult AnimationGraphCompiler::CompileGraphVariation( Resource::CompileContext const& ctx ) const
    {
        GraphVariationResourceDescriptor resourceDescriptor;
        if ( !Resource::ResourceDescriptor::TryReadFromFile( ctx.m_typeRegistry, ctx.m_inputFilePath, resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Load anim graph
        //-------------------------------------------------------------------------

        FileSystem::Path graphFilePath;
        if ( !ctx.ConvertResourcePathToFilePath( resourceDescriptor.m_graphPath, graphFilePath ) )
        {
            return Error( "invalid graph data path: %s", resourceDescriptor.m_graphPath.c_str() );
        }

        JsonReader jsonReader;
        if ( !jsonReader.ReadFromFile( graphFilePath ) )
        {
            return Error( "Failed to read animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        EditorGraphDefinition editorGraph;
        if ( !editorGraph.LoadFromJson( ctx.m_typeRegistry, jsonReader.GetDocument() ) )
        {
            return Error( "Malformed animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        StringID const variationID = resourceDescriptor.m_variationID.IsValid() ? resourceDescriptor.m_variationID : GraphVariation::DefaultVariationID;
        if ( !editorGraph.IsValidVariation( variationID ) )
        {
            return Error( "Invalid variation requested: %s", variationID.c_str() );
        }

        // Compile
        //-------------------------------------------------------------------------
        // We need to compile the graph to get the order of the data slots

        GraphDefinitionCompiler definitionCompiler;

        if ( !definitionCompiler.CompileGraph( editorGraph ) )
        {
            // Dump log
            for ( auto const& logEntry : definitionCompiler.GetLog() )
            {
                if ( logEntry.m_severity == Log::Severity::Error )
                {
                    Error( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Warning )
                {
                    Warning( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Message )
                {
                    Message( "%s", logEntry.m_message.c_str() );
                }
            }

            return Resource::CompilationResult::Failure;
        }

        // Create requested data set resource
        //-------------------------------------------------------------------------

        String dataSetFileName;
        dataSetFileName.sprintf( "%s_%s.%s", graphFilePath.GetFileNameWithoutExtension().c_str(), variationID.c_str(), GraphDataSet::GetStaticResourceTypeID().ToString().c_str() );

        FileSystem::Path dataSetFilePath = graphFilePath.GetParentDirectory();
        dataSetFilePath.Append( dataSetFileName );
        ResourcePath const dataSetPath = ResourcePath::FromFileSystemPath( ctx.m_rawResourceDirectoryPath, dataSetFilePath );

        if ( !GenerateVirtualDataSetResource( ctx, editorGraph, definitionCompiler.GetRegisteredDataSlots(), variationID, dataSetPath ) )
        {
            return Error( "Failed to create data set: %s", dataSetPath.c_str() );
        }

        // Create variation resource and serialize
        //-------------------------------------------------------------------------

        GraphVariation variation;
        variation.m_pGraphDefinition = ResourceID( resourceDescriptor.m_graphPath );
        variation.m_pDataSet = ResourceID( dataSetPath );

        //-------------------------------------------------------------------------

        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, GraphDataSet::GetStaticResourceTypeID() );
            hdr.AddInstallDependency( variation.m_pDataSet.GetResourceID() );
            hdr.AddInstallDependency( variation.m_pGraphDefinition.GetResourceID() );

            archive << hdr;
            archive << variation;

            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    //-------------------------------------------------------------------------

    bool AnimationGraphCompiler::GenerateVirtualDataSetResource( Resource::CompileContext const& ctx, EditorGraphDefinition const& editorGraph, TVector<UUID> const& registeredDataSlots, StringID const& variationID, ResourcePath const& dataSetPath ) const
    {
        auto pRootGraph = editorGraph.GetRootGraph();

        GraphDataSet dataSet;
        dataSet.m_variationID = variationID;

        //-------------------------------------------------------------------------
        // Get skeleton for variation
        //-------------------------------------------------------------------------

        KRG_ASSERT( editorGraph.IsValidVariation( variationID ) );
        auto const pVariation = editorGraph.GetVariation( variationID );
        KRG_ASSERT( pVariation != nullptr ); 
        if ( !pVariation->m_pSkeleton.IsValid() )
        {
            Error( "Skeleton not set for variation: %s", variationID.c_str() );
            return false;
        }

        dataSet.m_pSkeleton = pVariation->m_pSkeleton;

        //-------------------------------------------------------------------------
        // Fill data slots
        //-------------------------------------------------------------------------

        THashMap<UUID, GraphNodes::DataSlotEditorNode const*> dataSlotLookupMap;
        auto const& dataSlotNodes = pRootGraph->FindAllNodesOfType<GraphNodes::DataSlotEditorNode>( VisualGraph::SearchMode::Recursive, VisualGraph::SearchTypeMatch::Derived );
        for ( auto pSlotNode : dataSlotNodes )
        {
            dataSlotLookupMap.insert( TPair<UUID, GraphNodes::DataSlotEditorNode const*>( pSlotNode->GetID(), pSlotNode ) );
        }

        dataSet.m_resources.reserve( registeredDataSlots.size() );

        for ( auto const& dataSlotID : registeredDataSlots )
        {
            auto iter = dataSlotLookupMap.find( dataSlotID );
            if ( iter == dataSlotLookupMap.end() )
            {
                Error( "Unknown data slot encountered (%s) when generating data set", dataSlotID.ToString().c_str() );
                return false;
            }

            auto const dataSlotResourceID = iter->second->GetResourceID( editorGraph.GetVariationHierarchy(), variationID );
            dataSet.m_resources.emplace_back( dataSlotResourceID );
        }

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::Path const dataSetOutputPath = dataSetPath.ToFileSystemPath( ctx.m_compiledResourceDirectoryPath );

        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, dataSetOutputPath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, GraphDataSet::GetStaticResourceTypeID() );

            hdr.AddInstallDependency( dataSet.m_pSkeleton.GetResourceID() );

            for ( auto const& dataRecord : dataSet.m_resources )
            {
                if ( dataRecord.IsValid() )
                {
                    hdr.AddInstallDependency( dataRecord.GetResourceID() );
                }
            }

            archive << hdr << dataSet;
            return true;
        }
        else
        {
            return false;
        }
    }
}
