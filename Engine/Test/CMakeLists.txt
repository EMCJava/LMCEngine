
file(GLOB CPP_FILES *.cpp)

add_executable(Catch2Test ${CPP_FILES})
target_link_libraries(Catch2Test PRIVATE Catch2::Catch2WithMain)

target_link_libraries(Catch2Test PRIVATE
        NetServerLib
        NetClientLib
        Concept.lib
        imgui.lib
        ParticleAttributesRandomizer.lib)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(Catch2Test)
