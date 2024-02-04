//
// Created by samsa on 2/2/2024.
//

#include "RenderableMeshCluster.hpp"
#include "RenderableMesh.hpp"

DEFINE_CONCEPT_DS( RenderableMeshCluster )

RenderableMeshCluster::RenderableMeshCluster( )
{
    SetSearchThrough( false );
}

void
RenderableMeshCluster::Render( )
{
    GetConcepts( m_RenderableMeshes );
    m_RenderableMeshes.ForEachOriginal( []( auto& RM ) { RM->Render( ); } );
}
