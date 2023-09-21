add_library(Particle.lib Particle.hpp Particle.cpp)
add_library(ParticlePool.lib ParticlePool.hpp ParticlePool.cpp)

target_link_libraries(Particle.lib Orientation.lib)
target_link_libraries(ParticlePool.lib Particle.lib SpriteSquareTexture.lib)
