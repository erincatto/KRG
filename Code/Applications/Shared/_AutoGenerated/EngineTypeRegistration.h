//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "System/Entity/EntityTypeHelpers.h"
#include "System/TypeSystem/TypeRegistry.h"

#include "D:\Kruger\Code\Game\Core\_Module\Module.h"
#include "D:\Kruger\Code\Engine\Physics\_Module\Module.h"
#include "D:\Kruger\Code\Engine\Navmesh\_Module\Module.h"
#include "D:\Kruger\Code\Engine\Core\_Module\Module.h"
#include "D:\Kruger\Code\Engine\Render\_Module\Module.h"
#include "D:\Kruger\Code\Engine\Animation\_Module\Module.h"

namespace KRG
{
    namespace AutoGenerated
    {
        inline void RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
        {
            TypeSystem::RegisterCoreEntityTypes( typeRegistry );

            KRG::Engine::EngineModule::RegisterTypes( typeRegistry );
            KRG::Render::EngineModule::RegisterTypes( typeRegistry );
            KRG::Animation::EngineModule::RegisterTypes( typeRegistry );
            KRG::Navmesh::EngineModule::RegisterTypes( typeRegistry );
            KRG::Physics::EngineModule::RegisterTypes( typeRegistry );
            KRG::Game::GameModule::RegisterTypes( typeRegistry );

            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::EntityModel::EntityMapDescriptor"), ResourceTypeID( "MAP" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Shader"), ResourceTypeID( "SHDR" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::PixelShader"), ResourceTypeID( "PSDR" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::GeometryShader"), ResourceTypeID( "GSDR" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::VertexShader"), ResourceTypeID( "VSDR" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Texture"), ResourceTypeID( "TXTR" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::StaticMesh"), ResourceTypeID( "MSH" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Material"), ResourceTypeID( "MTRL" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::SkeletalMesh"), ResourceTypeID( "SMSH" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Animation::Skeleton"), ResourceTypeID( "SKEL" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Animation::AnimationData"), ResourceTypeID( "ANIM" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Navmesh::NavmeshData"), ResourceTypeID( "NAV" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Physics::PhysicsMesh"), ResourceTypeID( "PMSH" ) );
            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialDatabase"), ResourceTypeID( "PMDB" ) );
        }

        inline void UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
        {
            KRG::Game::GameModule::UnregisterTypes( typeRegistry );
            KRG::Physics::EngineModule::UnregisterTypes( typeRegistry );
            KRG::Navmesh::EngineModule::UnregisterTypes( typeRegistry );
            KRG::Animation::EngineModule::UnregisterTypes( typeRegistry );
            KRG::Render::EngineModule::UnregisterTypes( typeRegistry );
            KRG::Engine::EngineModule::UnregisterTypes( typeRegistry );

            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialDatabase" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Physics::PhysicsMesh" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Navmesh::NavmeshData" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Animation::AnimationData" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Animation::Skeleton" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::SkeletalMesh" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Material" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::StaticMesh" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Texture" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::VertexShader" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::GeometryShader" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::PixelShader" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::Render::Shader" ) );
            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( "KRG::EntityModel::EntityMapDescriptor" ) );

            TypeSystem::UnregisterCoreEntityTypes( typeRegistry );
        }
    }
}
