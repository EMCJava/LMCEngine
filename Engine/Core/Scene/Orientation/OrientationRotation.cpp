//
// Created by loyus on 8/1/2023.
//

#include "OrientationRotation.hpp"

const OrientationRotation::Rotation&
OrientationRotation::GetRotation( ) const
{
    return m_Rotation;
}

OrientationRotation::Rotation&
OrientationRotation::GetRotation( )
{
    return m_Rotation;
}
