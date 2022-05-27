#pragma once

#include "InputState.h"

//-------------------------------------------------------------------------

namespace KRG::Input
{
    enum class MouseButton
    {
        Left = 0,
        Right,
        Middle,
        Back,
        Forward,
        Button5,
        Button6,
        Button7,
        Button8,
        Button9,
    };

    //-------------------------------------------------------------------------

    class MouseInputState : public ButtonStates<10>
    {
        friend class KeyboardMouseInputDevice;

    public:

        enum class WheelType
        {
            Vertical,
            Horizontal
        };

    public:

        void Clear() { ResetFrameState( ResetType::Full ); }

        void ReflectFrom( Seconds const deltaTime, float timeScale, MouseInputState const& sourceState )
        {
            m_movementDelta = sourceState.m_movementDelta;
            m_position = m_position;
            m_verticalWheelDelta = m_verticalWheelDelta;
            m_horizontalWheelDelta = m_horizontalWheelDelta;

            ButtonStates<10>::ReflectFrom( deltaTime, timeScale, sourceState );
        }

        inline Float2 GetMovementDelta() const { return m_movementDelta; }
        inline int16_t GetWheelDelta( WheelType wheelType = WheelType::Vertical ) const { return ( wheelType == WheelType::Vertical ) ? m_verticalWheelDelta : m_horizontalWheelDelta; }
        inline Int2 GetCursorPosition() const { return m_position; }

        // Was the button just pressed (i.e. went from up to down this frame)
        KRG_FORCE_INLINE bool WasPressed( MouseButton buttonID ) const { return ButtonStates::WasPressed( (uint32_t) buttonID ); }

        // Was the button just release (i.e. went from down to up this frame). Also optionally returns how long the button was held for
        KRG_FORCE_INLINE bool WasReleased( MouseButton buttonID, Seconds* pHeldDownDuration = nullptr ) const { return ButtonStates::WasReleased( (uint32_t) buttonID, pHeldDownDuration ); }

        // Is the button being held down?
        KRG_FORCE_INLINE bool IsHeldDown( MouseButton buttonID, Seconds* pHeldDownDuration = nullptr ) const { return ButtonStates::IsHeldDown( (uint32_t) buttonID, pHeldDownDuration ); }

        // How long has the button been held down for?
        KRG_FORCE_INLINE Seconds GetHeldDuration( MouseButton buttonID ) const { return ButtonStates::GetHeldDuration( (uint32_t) buttonID ); }

    private:

        KRG_FORCE_INLINE void Press( MouseButton buttonID ) { ButtonStates::Press( (uint32_t) buttonID ); }
        KRG_FORCE_INLINE void Release( MouseButton buttonID ) { ButtonStates::Release( (uint32_t) buttonID ); }

        inline void ResetFrameState( ResetType resetType )
        {
            m_movementDelta = Float2::Zero;
            m_verticalWheelDelta = m_horizontalWheelDelta = 0;

            if ( resetType == ResetType::Full )
            {
                ClearButtonState();
            }
        }

    private:

        Float2                                  m_movementDelta;
        Int2                                    m_position;
        int16_t                                   m_verticalWheelDelta = 0;
        int16_t                                   m_horizontalWheelDelta = 0;
    };
}