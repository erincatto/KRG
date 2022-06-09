﻿#include "PropertyGridEditors.h"
#include "Tools/Core/Resource/ResourceFilePicker.h"
#include "Tools/Core/Resource/ResourceDatabase.h"
#include "Tools/Core/Widgets/CurveEditor.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/EnumInfo.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    constexpr static float const g_iconButtonWidth = 22;

    //-------------------------------------------------------------------------

    PropertyEditor::PropertyEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
        : m_pToolsContext( pToolsContext )
        , m_propertyInfo( propertyInfo )
        , m_pPropertyInstance( m_pPropertyInstance )
        , m_coreType( GetCoreType( propertyInfo.m_typeID ) )
    {
        KRG_ASSERT( m_pPropertyInstance != nullptr );
    }

    bool PropertyEditor::UpdateAndDraw()
    {
        ImGui::PushID( m_pPropertyInstance );
        HandleExternalUpdate();
        bool const result = InternalUpdateAndDraw();
        ImGui::PopID();

        return result;
    }

    //-------------------------------------------------------------------------
    // Core Editors
    //-------------------------------------------------------------------------

    class EnumEditor final : public PropertyEditor
    {
    public:

        EnumEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
            , m_pEnumInfo( pToolsContext->m_pTypeRegistry->GetEnumInfo( m_propertyInfo.m_typeID ) )
        {
            KRG_ASSERT( m_pEnumInfo != nullptr );
            EnumEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
            if ( ImGui::BeginCombo( "##enumCombo", m_pEnumInfo->GetConstantLabel( m_value_imgui ).c_str() ) )
            {
                for ( auto const& enumValue : m_pEnumInfo->m_constants )
                {
                    bool const isSelected = ( enumValue.second == m_value_imgui );
                    if ( ImGui::Selectable( enumValue.first.c_str(), isSelected ) )
                    {
                        m_value_imgui = enumValue.second;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if ( isSelected )
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            return  m_value_cached != m_value_imgui;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    *reinterpret_cast<uint8_t*>( m_pPropertyInstance ) = (uint8_t) m_value_cached;
                }
                break;

                case CoreTypeID::Int8:
                {
                    *reinterpret_cast<int8_t*>( m_pPropertyInstance ) = (int8_t) m_value_cached;
                }
                break;

                case CoreTypeID::Uint16:
                {
                    *reinterpret_cast<uint16_t*>( m_pPropertyInstance ) = (uint16_t) m_value_cached;
                }
                break;

                case CoreTypeID::Int16:
                {
                    *reinterpret_cast<int16_t*>( m_pPropertyInstance ) = (int16_t) m_value_cached;
                }
                break;

                case CoreTypeID::Uint32:
                {
                    *reinterpret_cast<uint32_t*>( m_pPropertyInstance ) = (uint32_t) m_value_cached;
                }
                break;

                case CoreTypeID::Int32:
                {
                    *reinterpret_cast<int32_t*>( m_pPropertyInstance ) = (int32_t) m_value_cached;
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

        virtual void ResetWorkingCopy() override
        {
            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<uint8_t const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int8:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<int8_t const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<uint16_t const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int16:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<int16_t const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<uint32_t const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int32:
                {
                    m_value_cached = m_value_imgui = ( int64_t ) *reinterpret_cast<int32_t const*>( m_pPropertyInstance );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

        virtual void HandleExternalUpdate() override
        {
            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<uint8_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int8:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<int8_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Uint16:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<uint16_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int16:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<int16_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Uint32:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<uint32_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int32:
                {
                    auto actualValue = ( int64_t ) * reinterpret_cast<int32_t const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

    private:

        EnumInfo const*     m_pEnumInfo = nullptr;
        int64_t               m_value_cached;
        int64_t               m_value_imgui;
    };

    //-------------------------------------------------------------------------

    class BoolEditor final : public PropertyEditor
    {
    public:

        BoolEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext,  propertyInfo, m_pPropertyInstance )
        {
            BoolEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::Checkbox( "##be", &m_value );
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            *reinterpret_cast<bool*>( m_pPropertyInstance ) = m_value;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value = *reinterpret_cast<bool*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            ResetWorkingCopy();
        }

    private:

        bool m_value;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class ScalarEditor final : public PropertyEditor
    {
    public:

        ScalarEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            ScalarEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );

            switch ( m_coreType )
            {
                case CoreTypeID::Int8:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S8, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int16:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S16, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int32:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S32, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int64:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S64, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint8:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U8, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U16, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U32, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint64:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U64, &m_value_imgui );
                }
                break;

                case CoreTypeID::Float:
                {
                    ImGui::InputFloat( "##scaed", (float*) &m_value_imgui );
                }
                break;

                case CoreTypeID::Double:
                {
                    ImGui::InputDouble( "##scaed", (double*) &m_value_imgui );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }

            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<T*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<T*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            T const actualValue = *reinterpret_cast<T*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        T                   m_value_imgui;
        T                   m_value_cached;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class VectorEditor final : public PropertyEditor
    {
    public:

        VectorEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            VectorEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            switch ( m_coreType )
            {
                case CoreTypeID::Float2:
                {
                    if ( ImGuiX::InputFloat2( "F2Ed", (Float2&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Float3:
                {
                    if ( ImGuiX::InputFloat3( "F3Ed", (Float3&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Float4:
                {
                    if ( ImGuiX::InputFloat4( "F4Ed", (Float4&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Vector:
                {
                    if ( ImGuiX::InputFloat4( "VectorEd", (Vector&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<T*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<T*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            T const actualValue = *reinterpret_cast<T*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        T                   m_value_imgui;
        T                   m_value_cached;
    };

    //-------------------------------------------------------------------------

    class ColorEditor final : public PropertyEditor
    {
    public:

        ColorEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            ColorEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
            ImGui::ColorEdit4( "##ce", &m_value_imgui.x );
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<Color*>( m_pPropertyInstance ) = Color( m_value_cached );
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = reinterpret_cast<Color*>( m_pPropertyInstance )->ToFloat4();
        }

        virtual void HandleExternalUpdate() override
        {
            Float4 const actualValue = reinterpret_cast<Color*>( m_pPropertyInstance )->ToFloat4();
            if ( !Vector( actualValue ).IsNearEqual4( m_value_cached ) )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        ImVec4 m_value_imgui;
        ImVec4 m_value_cached;
    };

    //-------------------------------------------------------------------------

    class RotationEditor final : public PropertyEditor
    {
    public:

        RotationEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            RotationEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::EulerAngles || m_coreType == CoreTypeID::Quaternion );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            return ImGuiX::InputFloat3( "Rotation", m_anglesInDegrees_imgui );
        }

        virtual void UpdateInstanceValue() override
        {
            m_anglesInDegrees_cached = m_anglesInDegrees_imgui;
            EulerAngles const newRotation( Degrees( m_anglesInDegrees_cached.m_x ).GetClamped180(), Degrees( m_anglesInDegrees_cached.m_y ).GetClamped180(), Degrees( m_anglesInDegrees_cached.m_z ).GetClamped180() );

            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                *reinterpret_cast<EulerAngles*>( m_pPropertyInstance ) = newRotation;
            }
            else if( m_coreType == CoreTypeID::Quaternion )
            {
                *reinterpret_cast<Quaternion*>( m_pPropertyInstance ) = Quaternion( newRotation );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = reinterpret_cast<EulerAngles*>( m_pPropertyInstance )->GetAsDegrees();
            }
            else if( m_coreType == CoreTypeID::Quaternion )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = reinterpret_cast<Quaternion*>( m_pPropertyInstance )->ToEulerAngles().GetAsDegrees();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            Float3 actualValue;
            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                actualValue = reinterpret_cast<EulerAngles*>( m_pPropertyInstance )->GetAsDegrees();
            }
            else if ( m_coreType == CoreTypeID::Quaternion )
            {
                actualValue = reinterpret_cast<Quaternion*>( m_pPropertyInstance )->ToEulerAngles().GetAsDegrees();
            }

            if ( !Vector( actualValue ).IsNearEqual3( m_anglesInDegrees_cached ) )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = actualValue;
            }
        }

    private:

        Float3                m_anglesInDegrees_imgui;
        Float3                m_anglesInDegrees_cached;
    };

    //-------------------------------------------------------------------------

    class AngleEditor final : public PropertyEditor
    {
    public:

        AngleEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            AngleEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Degrees || m_coreType == CoreTypeID::Radians );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const textAreaWidth = ImGui::GetContentRegionAvail().x - ( g_iconButtonWidth * 2 ) - ( ImGui::GetStyle().ItemSpacing.x * 2 );

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputFloat( "##ae", &m_valueInDegrees_imgui );
            bool valueChanged = ImGui::IsItemDeactivatedAfterEdit();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::Button( KRG_ICON_ANGLE_ACUTE "##ClampShortest", ImVec2( g_iconButtonWidth, 0 ) ) )
            {
                m_valueInDegrees_imgui = Degrees( m_valueInDegrees_imgui ).GetClamped180().ToFloat();
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp to Shortest Rotation [-180 : 180]" );

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::Button( KRG_ICON_ANGLE_OBTUSE "##ClampFull", ImVec2( g_iconButtonWidth, 0 ) ) )
            {
                m_valueInDegrees_imgui = Degrees( m_valueInDegrees_imgui ).GetClamped360().ToFloat();
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp to full rotation [-360 : 360]" );

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_valueInDegrees_cached = m_valueInDegrees_imgui;
            if ( m_coreType == CoreTypeID::Degrees )
            {
                *reinterpret_cast<Degrees*>( m_pPropertyInstance ) = Degrees( m_valueInDegrees_cached );
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                *reinterpret_cast<Radians*>( m_pPropertyInstance ) = Degrees( m_valueInDegrees_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Degrees )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = reinterpret_cast<Degrees*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = reinterpret_cast<Radians*>( m_pPropertyInstance )->ToDegrees().ToFloat();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            float actualValue = 0;

            if ( m_coreType == CoreTypeID::Degrees )
            {
                actualValue = reinterpret_cast<Degrees*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                actualValue = reinterpret_cast<Radians*>( m_pPropertyInstance )->ToDegrees().ToFloat();
            }

            if ( actualValue != m_valueInDegrees_cached )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = actualValue;
            }
        }

    private:

        float                m_valueInDegrees_imgui;
        float                m_valueInDegrees_cached;
    };

    //-------------------------------------------------------------------------

    class UUIDEditor final : public PropertyEditor
    {
    public:

        UUIDEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            UUIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const textAreaWidth = ImGui::GetContentRegionAvail().x - g_iconButtonWidth - ImGui::GetStyle().ItemSpacing.x;

            bool valueChanged = false;

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputText( "##ue", m_stringValue.data(), m_stringValue.length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::Button( KRG_ICON_SYNC"##Generate", ImVec2( g_iconButtonWidth, 0 ) ) )
            {
                m_value_imgui = UUID::GenerateID();
                m_stringValue = m_value_imgui.ToString();
                valueChanged = true;
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<UUID*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<UUID*>( m_pPropertyInstance );
            m_stringValue = m_value_imgui.ToString();
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<UUID*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
                m_stringValue = m_value_imgui.ToString();
            }
        }

    private:

        UUID            m_value_imgui;
        UUID            m_value_cached;
        UUIDString      m_stringValue;
    };

    //-------------------------------------------------------------------------

    class TimeEditor final : public PropertyEditor
    {
    public:

        TimeEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            TimeEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Microseconds || m_coreType == CoreTypeID::Milliseconds || m_coreType == CoreTypeID::Seconds );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
            if ( m_coreType == CoreTypeID::Microseconds )
            {
                ImGui::InputFloat( "##teus", &m_value_imgui, 0.0f, 0.0f, "%.2fus" );
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                ImGui::InputFloat( "#tems", &m_value_imgui, 0.0f, 0.0f, "%.2fms" );
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                ImGui::InputFloat( "##tes", &m_value_imgui, 0.0f, 0.0f, "%.2fs" );
            }
            
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            if ( m_coreType == CoreTypeID::Microseconds )
            {
                *reinterpret_cast<Microseconds*>( m_pPropertyInstance ) = Microseconds( m_value_cached );
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                *reinterpret_cast<Milliseconds*>( m_pPropertyInstance ) = Milliseconds( m_value_cached );
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                *reinterpret_cast<Seconds*>( m_pPropertyInstance ) = Seconds( m_value_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Microseconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Microseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Milliseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Seconds*>( m_pPropertyInstance )->ToFloat();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            float actualValue = 0;

            if ( m_coreType == CoreTypeID::Microseconds )
            {
                actualValue = reinterpret_cast<Microseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                actualValue = reinterpret_cast<Milliseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                actualValue = reinterpret_cast<Seconds*>( m_pPropertyInstance )->ToFloat();
            }

            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        float                m_value_imgui;
        float                m_value_cached;
    };

    //-------------------------------------------------------------------------

    class PercentageEditor final : public PropertyEditor
    {
    public:

        PercentageEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            PercentageEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const textAreaWidth = ImGui::GetContentRegionAvail().x - g_iconButtonWidth - ImGui::GetStyle().ItemSpacing.x;

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputFloat( "##pe", &m_value_imgui, 0, 0, "%.2f%%" );
            bool valueChanged = ImGui::IsItemDeactivatedAfterEdit();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::Button( KRG_ICON_PERCENT"##ClampPercentage", ImVec2( g_iconButtonWidth, 0 ) ) )
            {
                m_value_imgui = ( Percentage( m_value_imgui / 100 ).GetClamped( true ) ).ToFloat() * 100;
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp [-100 : 100]" );

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<Percentage*>( m_pPropertyInstance ) = Percentage( m_value_cached / 100.0f );
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = reinterpret_cast<Percentage*>( m_pPropertyInstance )->ToFloat() * 100;
        }

        virtual void HandleExternalUpdate() override
        {
            auto actualValue = reinterpret_cast<Percentage*>( m_pPropertyInstance )->ToFloat() * 100;;
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        float  m_value_imgui;
        float  m_value_cached;
    };

    //-------------------------------------------------------------------------

    class TransformEditor final : public PropertyEditor
    {
    public:

        TransformEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            TransformEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Transform || m_coreType == CoreTypeID::Matrix );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool transformUpdated = false;

            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 0, 2 ) );
            if ( ImGui::BeginTable( "Transform", 2, ImGuiTableFlags_None ) )
            {
                ImGui::TableSetupColumn( "Header", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 24 );
                ImGui::TableSetupColumn( "Values", ImGuiTableColumnFlags_NoHide );

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Rot" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "R", m_rotation_imgui ) )
                    {
                        m_rotation_imgui.m_x = Degrees( m_rotation_imgui.m_x ).GetClamped180().ToFloat();
                        m_rotation_imgui.m_y = Degrees( m_rotation_imgui.m_y ).GetClamped180().ToFloat();
                        m_rotation_imgui.m_z = Degrees( m_rotation_imgui.m_z ).GetClamped180().ToFloat();
                        m_rotation_cached = m_rotation_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Pos" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "T", m_translation_imgui ) )
                    {
                        m_translation_cached = m_translation_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Scl" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "S", m_scale_imgui ) )
                    {
                        m_scale_imgui.m_x = Math::IsNearZero( m_scale_imgui.m_x ) ? 0.01f : m_scale_imgui.m_x;
                        m_scale_imgui.m_y = Math::IsNearZero( m_scale_imgui.m_y ) ? 0.01f : m_scale_imgui.m_y;
                        m_scale_imgui.m_z = Math::IsNearZero( m_scale_imgui.m_z ) ? 0.01f : m_scale_imgui.m_z;
                        m_scale_cached = m_scale_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            return transformUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            EulerAngles const rot( m_rotation_cached );
            Quaternion const q( rot );

            if ( m_coreType == CoreTypeID::Transform )
            {
                *reinterpret_cast<Transform*>( m_pPropertyInstance ) = Transform( q, m_translation_cached, m_scale_cached );
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                *reinterpret_cast<Matrix*>( m_pPropertyInstance ) = Matrix( q, m_translation_cached, m_scale_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Transform )
            {
                auto const& transform = reinterpret_cast<Transform*>( m_pPropertyInstance );
                
                m_rotation_cached = m_rotation_imgui = transform->GetRotation().ToEulerAngles().GetAsDegrees();
                m_translation_cached = m_translation_imgui = transform->GetTranslation().ToFloat3();
                m_scale_cached = m_scale_imgui = transform->GetScale().ToFloat3();
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                auto const& matrix = reinterpret_cast<Matrix*>( m_pPropertyInstance );

                Quaternion q;
                Vector t, s;
                matrix->Decompose( q, t, s );

                m_rotation_cached = m_rotation_imgui = q.ToEulerAngles().GetAsDegrees();
                m_translation_cached = m_translation_imgui = t.ToFloat3();
                m_scale_cached = m_scale_imgui = s.ToFloat3();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            Quaternion actualQ;
            Vector actualTranslation;
            Vector actualScale;

            // Get actual transform values
            //-------------------------------------------------------------------------

            if ( m_coreType == CoreTypeID::Transform )
            {
                auto const& transform = reinterpret_cast<Transform*>( m_pPropertyInstance );
                actualQ = transform->GetRotation();
                actualTranslation = transform->GetTranslation();
                actualScale = transform->GetScale();
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                auto const& matrix = reinterpret_cast<Matrix*>( m_pPropertyInstance );
                matrix->Decompose( actualQ, actualTranslation, actualScale );
            }

            // Update the cached (and the imgui transform) when the actual is sufficiently different
            //-------------------------------------------------------------------------

            EulerAngles const currentRotation( m_rotation_cached );
            Quaternion const currentQ( currentRotation );

            Radians const angularDistance = Quaternion::Distance( currentQ, actualQ );
            if ( angularDistance > Degrees( 1.0f ) )
            {
                m_rotation_cached = actualQ.ToEulerAngles().GetAsDegrees();
                m_rotation_imgui = m_rotation_cached;
            }

            if ( !actualTranslation.IsNearEqual3( m_translation_cached ) )
            {
                m_translation_cached = actualTranslation.ToFloat3();
                m_translation_imgui = m_translation_cached;
            }

            if ( !actualScale.IsNearEqual3( m_scale_cached ) )
            {
                m_scale_cached = actualScale.ToFloat3();
                m_scale_imgui = m_scale_cached;
            }
        }

    private:

        Float3                m_rotation_imgui;
        Float3                m_translation_imgui;
        Float3                m_scale_imgui;

        Float3                m_rotation_cached;
        Float3                m_translation_cached;
        Float3                m_scale_cached;
    };

    //-------------------------------------------------------------------------

    class ResourcePathEditor final : public PropertyEditor
    {
    public:

        ResourcePathEditor( ToolsContext const* pToolsContext, Resource::ResourceFilePicker& resourcePicker, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
            , m_resourceFilePicker( resourcePicker )
        {
            ResourcePathEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::ResourcePath || m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr );

            //-------------------------------------------------------------------------

            if ( m_coreType == CoreTypeID::TResourcePtr )
            {
                m_resourceTypeID = m_pToolsContext->m_pTypeRegistry->GetResourceInfoForType( propertyInfo.m_templateArgumentTypeID )->m_resourceTypeID;
            }
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                float const cellContentWidth = ImGui::GetContentRegionAvail().x;
                float const textAreaWidth = cellContentWidth - ( g_iconButtonWidth * 2 ) - ( ImGui::GetStyle().ItemSpacing.x * 2 );

                ImGui::SetNextItemWidth( textAreaWidth );
                ImGui::InputText( "##pathstring", const_cast<char*>( m_value_imgui.GetString().data() ), m_value_imgui.GetString().length(), ImGuiInputTextFlags_ReadOnly );

                ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
                if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( g_iconButtonWidth, 0 ) ) )
                {
                    auto const selectedFiles = pfd::open_file( "Choose Data File", m_resourceFilePicker.GetRawResourceDirectoryPath().c_str(), { "All Files", "*" }, pfd::opt::none ).result();
                    if ( !selectedFiles.empty() )
                    {
                        FileSystem::Path const selectedPath( selectedFiles[0].c_str() );

                        if ( selectedPath.IsUnderDirectory( m_resourceFilePicker.GetRawResourceDirectoryPath() ) )
                        {
                            m_value_imgui = ResourcePath::FromFileSystemPath( m_resourceFilePicker.GetRawResourceDirectoryPath().c_str(), selectedPath );
                            valueChanged = true;
                        }
                        else
                        {
                            pfd::message( "Error", "Selected file is not with the resource folder!", pfd::choice::ok, pfd::icon::error ).result();
                        }
                    }
                }

                ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
                if ( ImGui::Button( KRG_ICON_CLOSE_CIRCLE"##Clear", ImVec2( g_iconButtonWidth, 0 ) ) )
                {
                    m_value_imgui.Clear();
                    valueChanged = true;
                }
            }
            else if ( m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                if ( m_resourceFilePicker.DrawPicker( m_resourceTypeID, &m_tempResourceID ) )
                {
                    m_tempResourceID = m_resourceFilePicker.GetSelectedResourceID();
                    m_value_imgui = m_tempResourceID.GetResourcePath();
                    valueChanged = true;
                }
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                *reinterpret_cast<ResourcePath*>( m_pPropertyInstance ) = m_value_cached;
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                *reinterpret_cast<ResourceID*>( m_pPropertyInstance ) = m_value_cached.IsValid() ? ResourceID( m_value_cached ) : ResourceID();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                *reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance ) = m_value_cached.IsValid() ? ResourceID( m_value_cached ) : ResourceID();
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                m_value_cached = m_value_imgui = *reinterpret_cast<ResourcePath*>( m_pPropertyInstance );
                m_tempResourceID.Clear();
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                m_tempResourceID = *reinterpret_cast<ResourceID*>( m_pPropertyInstance );
                m_value_cached = m_value_imgui = m_tempResourceID.GetResourcePath();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                Resource::ResourcePtr* pResourcePtr = reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance );
                m_value_cached = m_value_imgui = pResourcePtr->GetResourcePath();
                if ( m_value_imgui.IsValid() )
                {
                    m_tempResourceID = m_value_imgui;
                }
                else
                {
                    m_tempResourceID.Clear();
                }
            }
        }

        virtual void HandleExternalUpdate() override
        {
            ResourcePath const* pActualPath = nullptr;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                pActualPath = reinterpret_cast<ResourcePath*>( m_pPropertyInstance );
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                ResourceID* pResourceID = reinterpret_cast<ResourceID*>( m_pPropertyInstance );
                pActualPath = &pResourceID->GetResourcePath();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                Resource::ResourcePtr* pResourcePtr = reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance );
                pActualPath = &pResourcePtr->GetResourcePath();
            }

            if ( *pActualPath != m_value_cached )
            {
                m_value_cached = m_value_imgui = *pActualPath;

                if ( m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
                {
                    if ( m_value_imgui.IsValid() )
                    {
                        m_tempResourceID = m_value_imgui;
                    }
                    else
                    {
                        m_tempResourceID.Clear();
                    }
                }
            }
        }

    private:

        Resource::ResourceFilePicker&        m_resourceFilePicker;
        ResourceTypeID                          m_resourceTypeID;
        ResourceID                              m_tempResourceID;
        ResourcePath                            m_value_imgui;
        ResourcePath                            m_value_cached;
    };

    //-------------------------------------------------------------------------

    class ResourceTypeIDEditor final : public PropertyEditor
    {
    public:

        ResourceTypeIDEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            ResourceTypeIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
            if ( ImGui::BeginCombo( "##resTypeID", m_resourceTypeFriendlyName.c_str()) )
            {
                auto AddComboItem = [this, &valueChanged] ( ResourceInfo const& resourceInfo )
                {
                    bool const isSelected = ( m_value_imgui.m_ID == resourceInfo.m_resourceTypeID );
                    if ( ImGui::Selectable( resourceInfo.m_friendlyName.empty() ? "None" :  resourceInfo.m_friendlyName.c_str(), isSelected) )
                    {
                        if ( resourceInfo.m_resourceTypeID != m_value_imgui )
                        {
                            m_value_imgui = resourceInfo.m_resourceTypeID;
                            m_resourceTypeFriendlyName = resourceInfo.m_friendlyName;
                            valueChanged = true;
                        }
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if ( isSelected )
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                };

                //-------------------------------------------------------------------------

                AddComboItem( ResourceInfo() );

                //-------------------------------------------------------------------------

                auto const& registeredResourceTypes = m_pToolsContext->m_pTypeRegistry->GetRegisteredResourceTypes();
                for ( auto& pair : registeredResourceTypes )
                {
                    AddComboItem( pair.second );
                }

                ImGui::EndCombo();
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance );

            if ( m_value_cached.IsValid() )
            {
                m_resourceTypeFriendlyName = m_pToolsContext->m_pTypeRegistry->GetResourceInfoForResourceType( m_value_cached )->m_friendlyName;
            }
            else
            {
                m_resourceTypeFriendlyName = "";
            }
        }

        virtual void HandleExternalUpdate() override
        {
            auto actualValue = *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                ResetWorkingCopy();
            }
        }

    private:

        ResourceTypeID              m_value_imgui;
        ResourceTypeID              m_value_cached;
        String                      m_resourceTypeFriendlyName;
    };

    //-------------------------------------------------------------------------

    class BitflagsEditor final : public PropertyEditor
    {
    public:

        BitflagsEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            BitflagsEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::BitFlags || m_coreType == CoreTypeID::TBitFlags );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            if ( ImGui::BeginTable( "FlagsTable", 9, ImGuiTableFlags_SizingFixedFit ) )
            {
                if ( m_coreType == CoreTypeID::BitFlags )
                {
                    constexpr static char const* const rowLabels[4] = { "00-07", "08-15", "16-23", "24-31" };

                    for ( uint8_t i = 0u; i < 4; i++ )
                    {
                        ImGui::TableNextRow();

                        for ( uint8_t j = 0u; j < 8; j++ )
                        {
                            uint8_t const flagIdx = i * 8 + j;
                            ImGui::TableNextColumn();

                            ImGui::PushID( &m_values_imgui[flagIdx] );
                            if ( ImGui::Checkbox( "##flag", &m_values_imgui[flagIdx]) )
                            {
                                valueChanged = true;
                            }
                            ImGui::PopID();
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text( rowLabels[i] );
                    }
                }
                else if ( m_coreType == CoreTypeID::TBitFlags )
                {
                    // Get enum type for specific flags
                    TypeID const enumTypeID = m_propertyInfo.m_templateArgumentTypeID;
                    EnumInfo const* pEnumInfo = m_pToolsContext->m_pTypeRegistry->GetEnumInfo( enumTypeID );
                    KRG_ASSERT( pEnumInfo != nullptr );

                    //-------------------------------------------------------------------------

                    int32_t flagCount = 0;

                    // For each label 
                    for ( auto const& constant : pEnumInfo->m_constants )
                    {
                        if ( ( flagCount % 2 ) == 0 )
                        {
                            ImGui::TableNextRow();
                        }

                        ImGui::TableNextColumn();
                        flagCount++;

                        //-------------------------------------------------------------------------

                        int64_t const flagValue = constant.second;
                        KRG_ASSERT( flagValue >= 0 && flagValue <= 31 );
                        if ( ImGui::Checkbox( constant.first.c_str(), &m_values_imgui[flagValue] ) )
                        {
                            valueChanged = true;
                        }
                    }
                }

                ImGui::EndTable();
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            for ( uint8_t i = 0; i < 32; i++ )
            {
                pFlags->SetFlag( i, m_values_imgui[i] );
            }

            m_cachedFlags = pFlags->Get();
        }

        virtual void ResetWorkingCopy() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            for ( uint8_t i = 0; i < 32; i++ )
            {
                m_values_imgui[i] = pFlags->IsFlagSet( i );
            }

            m_cachedFlags = pFlags->Get();
        }

        virtual void HandleExternalUpdate() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            if ( pFlags->Get() != m_cachedFlags )
            {
                ResetWorkingCopy();
            }
        }

    private:

        bool        m_values_imgui[32];
        uint32_t      m_cachedFlags = 0;
    };

    //-------------------------------------------------------------------------

    class StringEditor final : public PropertyEditor
    {
    public:

        StringEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            StringEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
            ImGui::InputText( "##stringEd", m_buffer_imgui, 256);
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            strcpy_s( m_buffer_cached, 256, m_buffer_imgui );
            *reinterpret_cast<String*>( m_pPropertyInstance ) = m_buffer_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            String* pValue = reinterpret_cast<String*>( m_pPropertyInstance );
            strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
            strcpy_s( m_buffer_cached, 256, pValue->c_str() );
        }

        virtual void HandleExternalUpdate() override
        {
            String* pActualValue = reinterpret_cast<String*>( m_pPropertyInstance );
            if ( strcmp( pActualValue->c_str(), m_buffer_cached ) != 0 )
            {
                strcpy_s( m_buffer_imgui, 256, pActualValue->c_str() );
                strcpy_s( m_buffer_cached, 256, pActualValue->c_str() );
            }
        }

    private:

        char    m_buffer_imgui[256];
        char    m_buffer_cached[256];
    };

    //-------------------------------------------------------------------------

    class StringIDEditor final : public PropertyEditor
    {
        constexpr static uint32_t const s_bufferSize = 256;

    public:

        StringIDEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            StringIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - 60 );
            ImGui::InputText( "##StringInput", m_buffer_imgui, s_bufferSize );
            
            bool const itemEdited = ImGui::IsItemDeactivatedAfterEdit();
            if ( itemEdited )
            {
                StringUtils::StripTrailingWhitespace( m_buffer_imgui );
            }

            ImGui::SameLine();

            ImGui::SetNextItemWidth( -1 );
            {
                ImGuiX::ScopedFont const sf( ImGuiX::Font::SmallBold );
                ImGui::TextColored( Colors::LightGreen.ToFloat4(), m_IDString.data() );
            }

            return itemEdited;
        }

        virtual void UpdateInstanceValue() override
        {
            auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );

            if ( strlen( m_buffer_imgui ) > 0 )
            {
                m_buffer_cached = m_buffer_imgui;
                *pValue = StringID( m_buffer_imgui );
                m_IDString.sprintf( "%u", pValue->GetID() );
            }
            else
            {
                pValue->Clear();
                m_buffer_cached.clear();
                m_IDString = "Invalid";
            }
        }

        virtual void ResetWorkingCopy() override
        {
            auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            if ( pValue->IsValid() )
            {
                strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
                m_buffer_cached = pValue->c_str();
                m_IDString.sprintf( "%u", pValue->GetID() );
            }
            else
            {
                Memory::MemsetZero( m_buffer_imgui, s_bufferSize );
                m_buffer_cached.clear();
                m_IDString = "Invalid";
            }
        }

        virtual void HandleExternalUpdate() override
        {
            StringID const* pActualValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            if ( pActualValue->IsValid() )
            {
                if ( m_buffer_cached != pActualValue->c_str() )
                {
                    ResetWorkingCopy();
                }
            }
            else // Invalid String ID
            {
                if ( !m_buffer_cached.empty() )
                {
                    ResetWorkingCopy();
                }
            }
        }

    private:

        char                             m_buffer_imgui[s_bufferSize];
        TInlineString<s_bufferSize>      m_buffer_cached;
        TInlineString<s_bufferSize>      m_IDString;
    };

    //-------------------------------------------------------------------------

    class TagEditor final : public PropertyEditor
    {
    public:

        TagEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            TagEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            //float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            //float const childWindowWidth = 80;
            //float const textAreaWidth = cellContentWidth - childWindowWidth - ImGui::GetStyle().ItemSpacing.x;

            ////-------------------------------------------------------------------------

            //ImGui::SetNextItemWidth( childWindowWidth );
            //ImGui::PushStyleColor( ImGuiCol_Text, Colors::LightGreen.ToUInt32_ABGR() );
            //ImGui::InputText( "##IDString", m_IDString.data(), m_IDString.length(), ImGuiInputTextFlags_ReadOnly );
            //ImGui::PopStyleColor();

            ////-------------------------------------------------------------------------

            //ImGui::SetNextItemWidth( textAreaWidth );
            //ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            //ImGui::InputText( "##StringInput", m_buffer_imgui, 256 );
            //return ImGui::IsItemDeactivatedAfterEdit();

            ImGui::Text( "WIP" );
            return false;
        }

        virtual void UpdateInstanceValue() override
        {
            /*auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_cached, 256, m_buffer_imgui );
            *pValue = StringID( m_buffer_cached );
            m_IDString.sprintf( "%u", pValue->GetID() );*/
        }

        virtual void ResetWorkingCopy() override
        {
            /*auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
            strcpy_s( m_buffer_cached, 256, pValue->c_str() );
            m_IDString.sprintf( "%u", pValue->GetID() );*/
        }

        virtual void HandleExternalUpdate() override
        {
            /*auto pActualValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            if ( strcmp( pActualValue->c_str(), m_buffer_cached ) != 0 )
            {
                strcpy_s( m_buffer_imgui, 256, pActualValue->c_str() );
                strcpy_s( m_buffer_cached, 256, pActualValue->c_str() );
                m_IDString.sprintf( "%u", pActualValue->GetID() );
            }*/
        }

    private:
    };

    //-------------------------------------------------------------------------

    class IntRangeEditor final : public PropertyEditor
    {
    public:

        IntRangeEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            IntRangeEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const contentWidth = ImGui::GetContentRegionAvail().x;
            float const inputWidth = ( contentWidth - ( ImGui::GetStyle().ItemSpacing.x * 1 ) ) / 2;

            //-------------------------------------------------------------------------

            bool valueUpdated = false;

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##min", ImGuiDataType_S32, &m_value_imgui.m_begin, 0, 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##max", ImGuiDataType_S32, &m_value_imgui.m_end, 0, 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            return valueUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<IntRange*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<IntRange*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<IntRange*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        IntRange m_value_imgui;
        IntRange m_value_cached;
    };

    //-------------------------------------------------------------------------

    class FloatRangeEditor final : public PropertyEditor
    {
    public:

        FloatRangeEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
        {
            FloatRangeEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const contentWidth = ImGui::GetContentRegionAvail().x;
            float const inputWidth = ( contentWidth - ( ImGui::GetStyle().ItemSpacing.x * 1 ) ) / 2;

            //-------------------------------------------------------------------------

            bool valueUpdated = false;

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##min", &m_value_imgui.m_begin, 0, 0, "%.3f", 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                m_value_imgui.m_begin = Math::Min( m_value_imgui.m_begin, m_value_imgui.m_end );
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##max", &m_value_imgui.m_end, 0, 0, "%.3f", 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                m_value_imgui.m_end = Math::Max( m_value_imgui.m_begin, m_value_imgui.m_end );
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            return valueUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<FloatRange*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<FloatRange*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<FloatRange*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        FloatRange m_value_imgui;
        FloatRange m_value_cached;
    };

    //-------------------------------------------------------------------------

    class FloatCurveEditor final : public PropertyEditor
    {
    public:

        FloatCurveEditor( ToolsContext const* pToolsContext, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
            : PropertyEditor( pToolsContext, propertyInfo, m_pPropertyInstance )
            , m_editor( m_value_imgui )
        {
            FloatCurveEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const previewWidth = ImGui::GetContentRegionAvail().x - g_iconButtonWidth - ImGui::GetStyle().ItemSpacing.x;

            bool valueChanged = false;
            if ( ImGui::BeginChild( "##Preview", ImVec2( previewWidth, 140 ) ) )
            {
                valueChanged = m_editor.UpdateAndDraw();
            }
            ImGui::EndChild();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::Button( KRG_ICON_PLAYLIST_EDIT"##OpenCurveEditor" ) )
            {
                ImGui::OpenPopup( "CurveEditor" );
            }
            ImGuiX::ItemTooltip( "Open Full Curve Editor" );

            //-------------------------------------------------------------------------

            bool isOpen = true;
            ImGui::SetNextWindowSize( ImVec2( 800, 600 ), ImGuiCond_FirstUseEver );
            if ( ImGui::BeginPopupModal( "CurveEditor", &isOpen ) )
            {
                valueChanged = m_editor.UpdateAndDraw();
                ImGui::EndPopup();
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<FloatCurve*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            auto const& originalCurve = *reinterpret_cast<FloatCurve*>( m_pPropertyInstance );
            m_value_cached = m_value_imgui = originalCurve;

            m_editor.OnCurveExternallyUpdated();
            m_editor.ResetView();
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<FloatCurve*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
                m_editor.OnCurveExternallyUpdated();
            }
        }

    private:

        FloatCurve      m_value_imgui;
        FloatCurve      m_value_cached;
        CurveEditor     m_editor;
    };

    //-------------------------------------------------------------------------
    // Factory Method
    //-------------------------------------------------------------------------

    PropertyEditor* CreatePropertyEditor( ToolsContext const* pToolsContext, Resource::ResourceFilePicker& resourcePicker, PropertyInfo const& propertyInfo, uint8_t* m_pPropertyInstance )
    {
        if ( propertyInfo.IsEnumProperty() )
        {
            return KRG::New<EnumEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
        }
        else
        {
            CoreTypeID const coreType = GetCoreType( propertyInfo.m_typeID );
            switch ( coreType )
            {
                case CoreTypeID::Bool:
                {
                    return KRG::New<BoolEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int8:
                {
                    return KRG::New<ScalarEditor<int8_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int16:
                {
                    return KRG::New<ScalarEditor<int16_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int32:
                {
                    return KRG::New<ScalarEditor<int32_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int64:
                {
                    return KRG::New<ScalarEditor<int64_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;
                
                case CoreTypeID::Uint8:
                {
                    return KRG::New<ScalarEditor<uint8_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    return KRG::New<ScalarEditor<uint16_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    return KRG::New<ScalarEditor<uint32_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint64:
                {
                    return KRG::New<ScalarEditor<uint64_t>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;
                
                case CoreTypeID::Float:
                {
                    return KRG::New<ScalarEditor<float>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Double:
                {
                    return KRG::New<ScalarEditor<double>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Color:
                {
                    return KRG::New<ColorEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::EulerAngles:
                case CoreTypeID::Quaternion:
                {
                    return KRG::New<RotationEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::UUID:
                {
                    return KRG::New<UUIDEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Microseconds:
                case CoreTypeID::Milliseconds:
                case CoreTypeID::Seconds:
                {
                    return KRG::New<TimeEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Degrees:
                case CoreTypeID::Radians:
                {
                    return KRG::New<AngleEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Matrix:
                case CoreTypeID::Transform:
                {
                    return KRG::New<TransformEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::ResourcePath:
                case CoreTypeID::ResourceID:
                case CoreTypeID::ResourcePtr:
                case CoreTypeID::TResourcePtr:
                {
                    return KRG::New<ResourcePathEditor>( pToolsContext, resourcePicker, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::ResourceTypeID:
                {
                    return KRG::New<ResourceTypeIDEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::BitFlags:
                case CoreTypeID::TBitFlags:
                {
                    return KRG::New<BitflagsEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::StringID:
                {
                    return KRG::New<StringIDEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Tag:
                {
                    return KRG::New<TagEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::String:
                {
                    return KRG::New<StringEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float2:
                {
                    return KRG::New<VectorEditor<Float2>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float3:
                {
                    return KRG::New<VectorEditor<Float3>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float4:
                {
                    return KRG::New<VectorEditor<Float4>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Vector:
                {
                    return KRG::New<VectorEditor<Vector>>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Percentage:
                {
                    return KRG::New<PercentageEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::IntRange:
                {
                    return KRG::New<IntRangeEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::FloatRange:
                {
                    return KRG::New<FloatRangeEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::FloatCurve:
                {
                    return KRG::New<FloatCurveEditor>( pToolsContext, propertyInfo, m_pPropertyInstance );
                }
                break;
            }
        }

        return nullptr;
    }
}