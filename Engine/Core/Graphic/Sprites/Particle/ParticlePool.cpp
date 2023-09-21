//
// Created by samsa on 9/21/2023.
//

#include "ParticlePool.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

#include <Engine/Core/Graphic/API/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( ParticlePool )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ParticlePool, ConceptApplicable, m_StartIndex, m_EndIndex, m_Sprite, m_EndIndex )

class ParticlePoolConceptRender : public ConceptRenderable
{
    DECLARE_CONCEPT( ParticlePoolConceptRender, ConceptRenderable )

public:
    explicit ParticlePoolConceptRender( ParticlePool& particlePool, const std::shared_ptr<SpriteSquareTexture>& spriteTexture )
        : m_ParticlePool( particlePool )
        , m_SpriteTexture( spriteTexture )
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );

        GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
        gl->GenBuffers( 1, &m_MatrixInstancingBuffer );
        gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer );

        const auto MatricesLocation = 2;
        for ( unsigned int i = 0; i < 4; i++ )
        {
            gl->EnableVertexAttribArray( MatricesLocation + i );
            gl->VertexAttribPointer( MatricesLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof( glm::mat4 ),
                                     (const GLvoid*) ( sizeof( GLfloat ) * i * 4 ) );
            gl->VertexAttribDivisor( MatricesLocation + i, 1 );
        }
    }

    void
    Render( ) override
    {
        if ( !m_ParticlePool.m_Enabled ) return;
        if ( m_ParticlePool.m_StartIndex == m_ParticlePool.m_EndIndex ) return;   // Empty

        m_SpriteTexture->BindTexture( );
        m_SpriteTexture->SetShaderMatrix( );
        m_SpriteTexture->ApplyShaderUniforms( );

        auto* ShaderPtr = m_SpriteTexture->GetShader( );
        if ( ShaderPtr == nullptr ) [[unlikely]]
        {
            m_SpriteTexture->PureRender( );   // lol
        } else
        {
            ShaderPtr->Bind( );
            int        Index      = 0;
            const auto RenderFunc = [ this, &Index ]( Particle& P ) {
                m_ModelMatrices[ Index++ ] = P.GetOrientation( ).GetModelMatrix( );
            };

            m_ParticlePool.ForEach( RenderFunc );

            const auto* gl = Engine::GetEngine( )->GetGLContext( );
            GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
            GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, Index * sizeof( glm::mat4 ), m_ModelMatrices.data( ), GL_DYNAMIC_DRAW ) )

            GL_CHECK( gl->DrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, Index ) )
        }
    }

private:
    ParticlePool&                        m_ParticlePool;
    std::shared_ptr<SpriteSquareTexture> m_SpriteTexture;

    unsigned int                                       m_MatrixInstancingBuffer { };
    std::array<glm::mat4, ParticlePool::MAX_PARTICLES> m_ModelMatrices;
};
ParticlePoolConceptRender::~ParticlePoolConceptRender( )
{
    if ( m_MatrixInstancingBuffer != 0 )
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        GL_CHECK( gl->DeleteBuffers( 1, &m_MatrixInstancingBuffer ) )
    }
}
DEFINE_CONCEPT( ParticlePoolConceptRender )

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
    AddConcept<ParticlePoolConceptRender>( *this, m_Sprite = Sprite );
}

void
ParticlePool::ForEach( auto&& Func )
{
    // Empty
    if ( m_EndIndex == m_StartIndex ) return;

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
    if ( !IsEnabled( ) ) return;

    const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );

    ForEach( [ &DeltaTime ]( Particle& P ) {
        auto& Ori = P.GetOrientation( );
        Ori.AlterCoordinate( P.GetVelocity( ) * DeltaTime );
        Ori.AlterRotation( 0, 0, P.GetAngularVelocity( ) * DeltaTime );
    } );
}