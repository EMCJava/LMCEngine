//
// Created by loyus on 8/1/2023.
//

#include "Orientation.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_SIMPLE_IMGUI_TYPE( Orientation, TranslationOrigin, RotationOrigin, ScaleOrigin, Coordinate, Scale,
                          m_UsingQuat, m_Rotation, m_Quat,
                          SameRotationScaleOrigin )

const glm::vec3&
Orientation::SetRotation( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_UsingQuat = false;

    m_Rotation.x = X;
    m_Rotation.y = Y;
    m_Rotation.z = Z;

    return m_Rotation;
}

const glm::vec4&
Orientation::GetQuatVec4( ) const noexcept
{
    return m_QuatVec4;
}

const glm::quat &
Orientation::GetQuat( ) const noexcept
{
    return m_Quat;
}

const glm::vec3&
Orientation::AlterRotation( FloatTy X, FloatTy Y, FloatTy Z )
{
    if ( m_UsingQuat )
    {
        ActivateEularRotation( );
        m_UsingQuat = false;
    }

    m_Rotation.x += X;
    m_Rotation.y += Y;
    m_Rotation.z += Z;

    return m_Rotation;
}

const glm::quat&
Orientation::SetQuat( const glm::quat& m_Quat )
{
    m_UsingQuat         = true;
    return this->m_Quat = m_Quat;
}

void
Orientation::ActivateEularRotation( )
{
    if ( m_UsingQuat )
    {
        auto Copy   = m_Quat;
        m_Rotation  = eulerAngles( Copy );
        m_UsingQuat = false;
    }
}

void
Orientation::ActivateQuatRotation( )
{
    if ( !m_UsingQuat )
    {
        auto Copy   = m_Rotation;
        m_Quat      = glm::quat( Copy );
        m_UsingQuat = true;
    }
}

const glm::vec3&
Orientation::SetOrigin( FloatTy X, FloatTy Y, FloatTy Z )
{
    TranslationOrigin.x = X;
    TranslationOrigin.y = Y;
    TranslationOrigin.z = Z;

    RotationOrigin = ScaleOrigin = TranslationOrigin;
    SameRotationScaleOrigin      = true;

    return TranslationOrigin;
}

const glm::vec3&
Orientation::AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z )
{
    TranslationOrigin.x += X;
    TranslationOrigin.y += Y;
    TranslationOrigin.z += Z;

    RotationOrigin.x += X;
    RotationOrigin.y += Y;
    RotationOrigin.z += Z;

    ScaleOrigin.x += X;
    ScaleOrigin.y += Y;
    ScaleOrigin.z += Z;

    return TranslationOrigin;
}