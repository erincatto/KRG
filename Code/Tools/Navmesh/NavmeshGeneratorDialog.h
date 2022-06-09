#pragma once
#include "_Module/API.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "Engine/Core/Entity/EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG { class ToolsContext; class UpdateContext; }
namespace KRG::TypeSystem { class TypeRegistry; }
namespace KRG::EntityModel { class EntityCollectionDescriptor; }

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshGenerator;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_NAVMESH_API NavmeshGeneratorDialog
    {

    public:

        NavmeshGeneratorDialog( ToolsContext const* pToolsContext, NavmeshBuildSettings const& initialBuildSettings, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, FileSystem::Path const& navmeshOutputPath );
        ~NavmeshGeneratorDialog();

        bool UpdateAndDrawDialog( UpdateContext const& ctx );

        NavmeshBuildSettings const& GetBuildSettings() const { return m_buildSettings; }
        bool WereBuildSettingsUpdated() const { return m_propertyGrid.IsDirty(); }

    private:

        ToolsContext const*                                 m_pToolsContext = nullptr;
        NavmeshBuildSettings                                m_buildSettings;
        EntityModel::EntityCollectionDescriptor const       m_entityCollectionDesc;
        FileSystem::Path const                              m_navmeshOutputPath;
        PropertyGrid                                        m_propertyGrid;
        NavmeshGenerator*                                   m_pGenerator = nullptr;
    };
}