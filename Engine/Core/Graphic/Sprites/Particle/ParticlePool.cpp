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

        /*
         *
         * Model matrix Buffer
         *
         * */
        GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
        gl->GenBuffers( 1, &m_MatrixInstancingBuffer );
        gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer );
        // GL_CHECK( gl->BufferStorage( GL_ARRAY_BUFFER, m_ParticlePool.MAX_PARTICLES * sizeof( glm::mat4 ), nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT ) )
        GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, m_ParticlePool.MAX_PARTICLES * sizeof( glm::mat4 ), nullptr, GL_DYNAMIC_DRAW ) )

        const auto MatricesLocation = 2;
        for ( unsigned int i = 0; i < 4; i++ )
        {
            gl->EnableVertexAttribArray( MatricesLocation + i );
            gl->VertexAttribPointer( MatricesLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof( glm::mat4 ),
                                     (const GLvoid*) ( sizeof( GLfloat ) * i * 4 ) );
            gl->VertexAttribDivisor( MatricesLocation + i, 1 );
        }

        /*
         *
         * Color Buffer
         *
         * */
        GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
        gl->GenBuffers( 1, &m_ColorInstancingBuffer );
        gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer );
        // GL_CHECK( gl->BufferStorage( GL_ARRAY_BUFFER, m_ParticlePool.MAX_PARTICLES * sizeof( glm::vec4 ), nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT ) )
        GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, m_ParticlePool.MAX_PARTICLES * sizeof( glm::vec4 ), nullptr, GL_DYNAMIC_DRAW ) )

        gl->EnableVertexAttribArray( MatricesLocation + 4 );
        gl->VertexAttribPointer( MatricesLocation + 4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (const GLvoid*) nullptr );
        gl->VertexAttribDivisor( MatricesLocation + 4, 1 );
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

            const auto* gl    = Engine::GetEngine( )->GetGLContext( );
            int         Index = 0;

            // GL_MAP_WRITE_BIT GL_MAP_INVALIDATE_BUFFER_BIT GL_MAP_UNSYNCHRONIZED_BIT;
            GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
            REQUIRED( m_ModelMatricesGPUMap = static_cast<glm::mat4*>( gl->MapBuffer( GL_ARRAY_BUFFER, GL_READ_WRITE ) ) )
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer ) )
            REQUIRED( m_ColorsGPUMap = static_cast<glm::vec4*>( gl->MapBuffer( GL_ARRAY_BUFFER, GL_READ_WRITE ) ) )

            const auto RenderFunc = [ this, &Index ]( Particle& P ) {
                P.GetOrientation( ).CalculateModelMatrix( &m_ModelMatricesGPUMap[ Index ] );
                m_ColorsGPUMap[ Index++ ] = P.GetColor( );
            };

            m_ParticlePool.ForEach( RenderFunc );

            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
            gl->UnmapBuffer( GL_ARRAY_BUFFER );
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer ) )
            gl->UnmapBuffer( GL_ARRAY_BUFFER );

            GL_CHECK( gl->DrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, Index ) )
        }
    }

private:
    ParticlePool&                        m_ParticlePool;
    std::shared_ptr<SpriteSquareTexture> m_SpriteTexture;

    unsigned int m_MatrixInstancingBuffer { };
    unsigned int m_ColorInstancingBuffer { };
    glm::mat4*   m_ModelMatricesGPUMap = nullptr;
    glm::vec4*   m_ColorsGPUMap        = nullptr;
};
ParticlePoolConceptRender::~ParticlePoolConceptRender( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    if ( m_MatrixInstancingBuffer != 0 )
    {
//        if ( m_ModelMatricesGPUMap != nullptr )
//        {
//            m_ModelMatricesGPUMap = nullptr;
//            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
//            GL_CHECK( gl->UnmapBuffer( m_MatrixInstancingBuffer ) )
//        }

        GL_CHECK( gl->DeleteBuffers( 1, &m_MatrixInstancingBuffer ) )
    }

    if ( m_ColorInstancingBuffer != 0 )
    {
//        if ( m_ColorsGPUMap != nullptr )
//        {
//            m_ColorsGPUMap = nullptr;
//            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer ) )
//            GL_CHECK( gl->UnmapBuffer( m_ColorInstancingBuffer ) )
//        }

        GL_CHECK( gl->DeleteBuffers( 1, &m_ColorInstancingBuffer ) )
    }

    m_MatrixInstancingBuffer = m_ColorInstancingBuffer = 0;
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
        m_Particles[ m_EndIndex ].SetLifeTime( 0 );
    }

    return NewParticle;
}

void
ParticlePool::RemoveFirstParticle( )
{
    // Empty
    if ( m_StartIndex == m_EndIndex ) return;

    m_Particles[ m_StartIndex ].SetLifeTime( 0 );
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
        Ori.AlterCoordinate( P.GetVelocity( ) * DeltaTime, false );
        Ori.AlterRotation( 0, 0, P.GetAngularVelocity( ) * DeltaTime, false );

        P.GetColor( ) += P.GetLinearColorVelocity( ) * DeltaTime;
        P.AlterLifeTime( -DeltaTime );
    } );

    // Not empty, and first is not "dead"
    while ( m_EndIndex != m_StartIndex && !m_Particles[ m_StartIndex ].IsAlive( ) )
    {
        m_StartIndex = ( m_StartIndex + 1 ) % MAX_PARTICLES;
    }
}