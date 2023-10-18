//
// Created by samsa on 10/14/2023.
//

#include "PureConceptCameraStack.hpp"
#include "PureConceptCamera.hpp"

DEFINE_CONCEPT_DS( PureConceptCameraStack )

std::shared_ptr<PureConceptCamera>
PureConceptCameraStack::GetCamera( ) const
{
    REQUIRED_IF( !m_CameraStack.empty( ) )
    {
        return m_CameraStack.top( );
    }

    return nullptr;
}

std::shared_ptr<PureConceptCamera>&
PureConceptCameraStack::GetCameraRef( )
{
    REQUIRED_IF( !m_CameraStack.empty( ) )
    {
        return m_CameraStack.top( );
    }

    throw std::runtime_error( "Camera stack is empty" );
}

void
PureConceptCameraStack::PushCamera( std::shared_ptr<PureConceptCamera> Camera )
{
    m_CameraStack.push( std::move( Camera ) );
}

std::shared_ptr<PureConceptCamera>
PureConceptCameraStack::PopCamera( )
{
    auto Result = std::move( m_CameraStack.top( ) );
    m_CameraStack.pop( );

    return Result;
}

void
PureConceptCameraStack::Clear( )
{
    m_CameraStack = std::stack<std::shared_ptr<PureConceptCamera>>( );
}
