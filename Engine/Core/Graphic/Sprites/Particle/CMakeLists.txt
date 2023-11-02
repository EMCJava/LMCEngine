add_library(Particle.lib Particle.hpp Particle.cpp)
add_library(ParticlePool.lib ParticlePool.hpp ParticlePool.cpp)
add_library(ParticleAttributesRandomizer.lib ParticleAttributesRandomizer.hpp ParticleAttributesRandomizer.cpp)

target_link_libraries(Particle.lib Orientation.lib)
target_link_libraries(ParticleAttributesRandomizer.lib Particle.lib)
target_link_libraries(ParticlePool.lib Particle.lib ParticleAttributesRandomizer.lib SpriteSquareTexture.lib)
