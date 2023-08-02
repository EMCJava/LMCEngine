//
// Created by loyus on 8/1/2023.
//

#include "TileSpriteSet.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS( TileSpriteSet, ConceptRenderable )

void
TileSpriteSet::Render( )
{
    glm::mat4 TransformationMatrix { 1 };

    for ( const auto& Tail : m_TailList )
    {
        REQUIRED_IF( m_Sprites.contains( Tail.Degree ) )
        {

            auto& Sp       = m_Sprites[ Tail.Degree ];
            auto* SpShader = Sp->GetShader( );

            if ( SpShader != nullptr )
            {
                SpShader->Bind( );
                if ( m_ActiveCamera != nullptr )
                {
                    SpShader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
                }

                SpShader->SetMat4( "modelMatrix", TransformationMatrix );

                TransformationMatrix = Sp->GetTranslationMatrix( ) * Sp->GetRotationMatrix( );

                const auto* gl = Engine::GetEngine( )->GetGLContext( );
                gl->ActiveTexture( GL_TEXTURE0 );
                gl->BindTexture( GL_TEXTURE_2D, Sp->GetTextureID( ) );

                Sp->PureRender( );
            }
        }
    }
}

SpriteSquareTexture*
TileSpriteSet::RegisterSprite( uint32_t Degree, std::unique_ptr<SpriteSquareTexture>&& TSprite )
{
    if ( m_Sprites.contains( Degree ) )
    {
        spdlog::warn( "Overriding existing sprite for degree {}", Degree );
    }

    auto* Backup        = TSprite.get( );
    m_Sprites[ Degree ] = std::move( TSprite );
    return Backup;
}

void
TileSpriteSet::AddTile( TileSpriteSet::TileMeta Tile )
{
    Tile.AccumulatedDegree = Tile.Degree;
    if ( !m_TailList.empty( ) )
    {
        Tile.AccumulatedDegree += m_TailList.back( ).AccumulatedDegree;
    }

    m_TailList.push_back( Tile );
}
