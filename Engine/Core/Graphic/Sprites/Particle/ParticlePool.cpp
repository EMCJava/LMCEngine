//
// Created by samsa on 9/21/2023.
//

#include "ParticleAttributesRandomizer.hpp"
#include "ParticlePool.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>

#include <Engine/Core/Graphic/API/OpenGL.hpp>

#include <cmath>

DEFINE_CONCEPT_DS( ParticlePool )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ParticlePool, ConceptApplicable, m_ParticleCount, m_Sprite )

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
        if ( m_ParticlePool.m_ParticleCount == 0 ) return;   // Empty

        m_SpriteTexture->BindTexture( );
        if ( m_ActiveCamera != nullptr ) m_SpriteTexture->SetActiveCamera( m_ActiveCamera );
        m_SpriteTexture->SetCameraMatrix( );
        m_SpriteTexture->GetShader( )->SetMat4( "modelMatrix", m_SpriteTexture->GetModelMatrix( ) );
        m_SpriteTexture->ApplyShaderUniforms( );

        auto* ShaderPtr = m_SpriteTexture->GetShader( );
        if ( ShaderPtr == nullptr ) [[unlikely]]
        {
            m_SpriteTexture->PureRender( );   // lol
        } else
        {
            ParticleCount           = m_ParticlePool.GetParticleCount( );
            constexpr auto MapFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

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
                const auto NumberOfThreadNeeded = std::min( (int) ceil( (FloatTy) ParticleCount / MinParticlesPreThread ), MaxThreads );
                auto*      Threads              = reinterpret_cast<std::thread*>( m_ThreadBuffer );
                const auto ParticlePreThread    = ParticleCount / NumberOfThreadNeeded;

                const auto RunThreadInRange = [ this, ParticlePreThread ]( std::thread* Threads, size_t StartIndex, size_t EndIndex, size_t ThreadCount ) {
                    if ( ThreadCount == 0 ) return;
                    for ( int i = 0; i < ThreadCount - 1; ++i )
                    {
                        new ( Threads + i ) std::thread( [ this, Start = StartIndex + i * ( ParticlePreThread ), End = StartIndex + ( i + 1 ) * ( ParticlePreThread ) ]( ) {
                            const auto RenderFunc = [ this ]( size_t Index, Particle& P ) {
                                OrientationMatrix::CalculateModelMatrix( &m_ModelMatricesGPUMap[ Index ], P.GetOrientation( ) );
                                m_ColorsGPUMap[ Index ] = P.GetColor( );
                            };
                            m_ParticlePool.ForEachRange( RenderFunc, Start, End );
                        } );
                    }

                    new ( Threads + ( ThreadCount - 1 ) ) std::thread( [ this, Start = StartIndex + ( ThreadCount - 1 ) * ( ParticlePreThread ), End = EndIndex ]( ) {
                        const auto RenderFunc = [ this ]( size_t Index, Particle& P ) {
                            OrientationMatrix::CalculateModelMatrix( &m_ModelMatricesGPUMap[ Index ], P.GetOrientation( ) );
                            m_ColorsGPUMap[ Index ] = P.GetColor( );
                        };
                        m_ParticlePool.ForEachRange( RenderFunc, Start, End );
                    } );
                };

                RunThreadInRange( Threads, 0, m_ParticlePool.m_ParticleCount - 1, NumberOfThreadNeeded );

                for ( int i = 0; i < NumberOfThreadNeeded; ++i )
                {
                    if ( Threads[ i ].joinable( ) ) Threads[ i ].join( );
                    Threads[ i ].~thread( );
                }

                ThreadUsed = NumberOfThreadNeeded;
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

    static constexpr auto MaxThreads            = 16;
    static constexpr auto MinParticlesPreThread = 200;
    char                  m_ThreadBuffer[ sizeof( std::thread ) * MaxThreads ];

public:
    /*
     *
     * For ImGui inspect
     *
     * */
    int ThreadUsed    = 0;
    int ParticleCount = 0;

    ENABLE_IMGUI( ParticlePoolConceptRender )
};

ParticlePoolConceptRender::~ParticlePoolConceptRender( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    if ( m_MatrixInstancingBuffer != 0 ) GL_CHECK( gl->DeleteBuffers( 1, &m_MatrixInstancingBuffer ) )
    if ( m_ColorInstancingBuffer != 0 ) GL_CHECK( gl->DeleteBuffers( 1, &m_ColorInstancingBuffer ) )

    m_MatrixInstancingBuffer = m_ColorInstancingBuffer = 0;
}

DEFINE_CONCEPT( ParticlePoolConceptRender )
DEFINE_SIMPLE_IMGUI_TYPE( ParticlePoolConceptRender, ThreadUsed, ParticleCount )

ParticlePool::ParticlePool( )
{
    SetSearchThrough( );
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
    if ( m_ParticleCount == 0 ) return;

    for ( size_t i = 0; i < m_ParticleCount; i++ )
    {
        Func( m_MinHeapParticles[ i ] );
    }
}

void
ParticlePool::Apply( )
{
    if ( !IsEnabled( ) ) return;

    const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );

    ForEach( [ &DeltaTime ]( Particle& P ) {
        P.Update( DeltaTime );
    } );

    // Not empty, and first is not "dead"
    while ( m_ParticleCount > 0 && !m_MinHeapParticles.front( ).IsAlive( ) )
    {
        KillFirstToDieParticle( );
    }
}

size_t
ParticlePool::GetParticleCount( ) const
{
    return m_ParticleCount;
}

void
ParticlePool::ForEachRange( auto&& Func, size_t Start, size_t End )
{
    for ( size_t i = Start; i < End; i++ )
    {
        Func( i, m_MinHeapParticles[ i ] );
    }
}

void
ParticlePool::KillFirstToDieParticle( )
{
    // Empty
    if ( m_ParticleCount == 0 ) return;

    std::ranges::pop_heap( m_MinHeapParticles.begin( ), m_MinHeapParticles.begin( ) + m_ParticleCount, ParticleTimeGreater { } );
    m_ParticleCount--;
}

void
ParticlePool::PushLastParticleInHeap( )
{
    m_ParticleCount++;
    std::ranges::push_heap( m_MinHeapParticles.begin( ), m_MinHeapParticles.begin( ) + m_ParticleCount, ParticleTimeGreater { } );
}
