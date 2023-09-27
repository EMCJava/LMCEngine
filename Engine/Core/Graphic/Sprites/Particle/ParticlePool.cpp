//
// Created by samsa on 9/21/2023.
//

#include "ParticlePool.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

#include <Engine/Core/Graphic/API/OpenGL.hpp>

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

        /*
         *
         * Model matrix Buffer
         *
         * */
        gl->GenBuffers( 1, &m_MatrixInstancingBuffer );
        gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer );
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
        gl->GenBuffers( 1, &m_ColorInstancingBuffer );
        gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer );
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
            const auto     ParticleCount = m_ParticlePool.GetParticleCount( );
            constexpr auto MapFlags      = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

            const auto* gl = Engine::GetEngine( )->GetGLContext( );
            GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )

            // Map buffer to calculate in-place
            GL_CHECK( gl->BindVertexArray( m_SpriteTexture->GetVAO( ) ) )
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
            REQUIRED( m_ModelMatricesGPUMap = static_cast<glm::mat4*>( gl->MapBufferRange( GL_ARRAY_BUFFER, 0, ParticleCount * sizeof( glm::mat4 ), MapFlags ) ) )
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer ) )
            REQUIRED( m_ColorsGPUMap = static_cast<glm::vec4*>( gl->MapBufferRange( GL_ARRAY_BUFFER, 0, ParticleCount * sizeof( glm::vec4 ), MapFlags ) ) )

            // Update buffer
            {
                const auto MaxThreads = 16;
                char       ThreadBuffer[ sizeof( std::thread ) * MaxThreads ];
                auto*      Threads = reinterpret_cast<std::thread*>( ThreadBuffer );

                const auto ParticlePreThread = ParticleCount / MaxThreads;

                const auto RunThreadInRange = [ this, ParticlePreThread ]( std::thread* Threads, size_t StartIndex, size_t EndIndex, size_t ThreadCount ) {
                    if ( ThreadCount == 0 ) return;
                    for ( int i = 0; i < ThreadCount - 1; ++i )
                    {
                        new ( Threads + i ) std::thread( [ this, Start = StartIndex + i * ( ParticlePreThread ), End = StartIndex + ( i + 1 ) * ( ParticlePreThread ) ]( ) {
                            const auto RenderFunc = [ this ]( size_t Index, Particle& P ) {
                                P.GetOrientation( ).CalculateModelMatrix( &m_ModelMatricesGPUMap[ Index ] );
                                m_ColorsGPUMap[ Index ] = P.GetColor( );
                            };
                            m_ParticlePool.ForEachRange( RenderFunc, Start, End );
                        } );
                    }

                    new ( Threads + ( ThreadCount - 1 ) ) std::thread( [ this, Start = StartIndex + ( ThreadCount - 1 ) * ( ParticlePreThread ), End = EndIndex ]( ) {
                        const auto RenderFunc = [ this ]( size_t Index, Particle& P ) {
                            P.GetOrientation( ).CalculateModelMatrix( &m_ModelMatricesGPUMap[ Index ] );
                            m_ColorsGPUMap[ Index ] = P.GetColor( );
                        };
                        m_ParticlePool.ForEachRange( RenderFunc, Start, End );
                    } );
                };

                if ( m_ParticlePool.m_StartIndex > m_ParticlePool.m_EndIndex )
                {
                    // Wrap
                    const auto FirstThreadCount = std::min( (int) std::round( FloatTy( m_ParticlePool.MAX_PARTICLES - m_ParticlePool.m_StartIndex ) / ParticlePreThread ), MaxThreads );
                    RunThreadInRange( Threads, m_ParticlePool.m_StartIndex, m_ParticlePool.MAX_PARTICLES, FirstThreadCount );
                    RunThreadInRange( Threads + FirstThreadCount, 0, m_ParticlePool.m_EndIndex, MaxThreads - FirstThreadCount );
                } else
                {
                    RunThreadInRange( Threads, m_ParticlePool.m_StartIndex, m_ParticlePool.m_EndIndex, MaxThreads );
                }

                for ( int i = 0; i < MaxThreads; ++i )
                {
                    if ( Threads[ i ].joinable( ) ) Threads[ i ].join( );
                    Threads[ i ].~thread( );
                }
            }
            // Unmap buffer
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_MatrixInstancingBuffer ) )
            gl->UnmapBuffer( GL_ARRAY_BUFFER );
            GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_ColorInstancingBuffer ) )
            gl->UnmapBuffer( GL_ARRAY_BUFFER );

            // Render
            ShaderPtr->Bind( );
            GL_CHECK( gl->DrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, ParticleCount ) )
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
    if ( m_MatrixInstancingBuffer != 0 ) GL_CHECK( gl->DeleteBuffers( 1, &m_MatrixInstancingBuffer ) )
    if ( m_ColorInstancingBuffer != 0 ) GL_CHECK( gl->DeleteBuffers( 1, &m_ColorInstancingBuffer ) )

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

size_t
ParticlePool::GetParticleCount( ) const
{
    if ( m_EndIndex >= m_StartIndex ) return m_EndIndex - m_StartIndex;

    // Wrap around
    return MAX_PARTICLES + m_EndIndex - m_StartIndex;
}

void
ParticlePool::ForEachRange( auto&& Func, size_t Start, size_t End )
{
    for ( size_t i = Start; i < End; i++ )
    {
        Func( i, m_Particles[ i ] );
    }
}
