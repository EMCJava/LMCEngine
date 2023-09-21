//
// Created by samsa on 9/21/2023.
//

#include "ParticlePool.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

DEFINE_CONCEPT_DS( ParticlePool )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ParticlePool, ConceptApplicable, m_StartIndex, m_EndIndex )

class ParticlePoolConceptRender : public ConceptRenderable
{
    DECLARE_CONCEPT( ParticlePoolConceptRender, ConceptRenderable )

public:
    explicit ParticlePoolConceptRender( ParticlePool& particlePool, const std::shared_ptr<SpriteSquareTexture>& spriteTexture )
        : m_ParticlePool( particlePool )
        , m_SpriteTexture( spriteTexture )
    { }

    void
    Render( ) override
    {
        if ( !m_ParticlePool.m_Enabled ) return;
        if ( m_ParticlePool.m_StartIndex == m_ParticlePool.m_EndIndex ) return;   // Empty

        m_SpriteTexture->BindTexture( );
        m_SpriteTexture->SetShaderMatrix( );
        m_SpriteTexture->ApplyShaderUniforms( );

        auto*             ShaderPtr  = m_SpriteTexture->GetShader( );
        static const auto RenderFunc = [ this, ShaderPtr ]( Particle& P ) {
            if ( ShaderPtr != nullptr )
            {
                ShaderPtr->Bind( );

                const auto ModelMatrix = P.GetOrientation( ).GetTranslationMatrix( ) * P.GetOrientation( ).GetRotationMatrix( );
                ShaderPtr->SetMat4( "modelMatrix", ModelMatrix );
            }

            m_SpriteTexture->PureRender( );
        };

        m_ParticlePool.ForEach( RenderFunc );
    }

private:
    ParticlePool&                        m_ParticlePool;
    std::shared_ptr<SpriteSquareTexture> m_SpriteTexture;
};
DEFINE_CONCEPT_DS( ParticlePoolConceptRender )

ParticlePool::ParticlePool( )
{
    SetSearchThrough( );
}

Particle&
ParticlePool::AddParticle( )
{
    auto& NewParticle = m_Particles[ m_EndIndex ];
    m_EndIndex        = ( m_EndIndex + 1 ) % MAX_PARTICLES;

    // Replace the very first particle
    if ( m_StartIndex == m_EndIndex )
    {
        m_StartIndex = ( m_StartIndex + 1 ) % MAX_PARTICLES;
        m_Particles[ m_EndIndex ].SetAlive( false );
    }

    NewParticle.SetAlive( );
    return NewParticle;
}

void
ParticlePool::RemoveFirstParticle( )
{
    // Empty
    if ( m_StartIndex == m_EndIndex ) return;

    m_Particles[ m_StartIndex ].SetAlive( false );
    m_StartIndex = ( m_StartIndex + 1 ) % MAX_PARTICLES;
}

void
ParticlePool::SetSprite( const std::shared_ptr<SpriteSquareTexture>& Sprite )
{
    RemoveConcepts<PureConcept>( );
    AddConcept<ParticlePoolConceptRender>( *this, Sprite );
}

void
ParticlePool::ForEach( auto&& Func )
{
    if ( m_EndIndex > m_StartIndex )
    {
        for ( size_t i = m_StartIndex; i != m_EndIndex; i++ )
        {
            Func( m_Particles[ i ] );
        }
    } else
    {
        for ( size_t i = m_StartIndex; i < MAX_PARTICLES; i++ )
        {
            Func( m_Particles[ i ] );
        }

        for ( size_t i = 0; i < m_EndIndex; i++ )
        {
            Func( m_Particles[ i ] );
        }
    }
}

void
ParticlePool::Apply( )
{
    const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );

    ForEach( [ &DeltaTime ]( Particle& P ) {
        auto& Ori = P.GetOrientation( );
        Ori.AlterCoordinate( P.GetVelocity( ) * DeltaTime );
        Ori.AlterRotation( 0, 0, P.GetAngularVelocity( ) * DeltaTime );
    } );
}