//
// Created by loyus on 8/1/2023.
//

#include "Orientation.hpp"

OrientationCoordinate::Coordinate
Orientation::GetWorldCoordinate( ) const
{
    const auto ModelMatrix = m_TranslationMatrix * m_RotationMatrix;
    glm::vec3  WorldOffset { ModelMatrix[ 3 ] };

    return OrientationCoordinate::Coordinate( WorldOffset.x, WorldOffset.y, WorldOffset.z );
}
