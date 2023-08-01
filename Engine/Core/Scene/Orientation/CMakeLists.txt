add_library(Orientation.lib Orientation.hpp Orientation.cpp)
add_library(OrientationCoordinate.lib OrientationCoordinate.hpp OrientationCoordinate.cpp)
add_library(OrientationRotation.lib OrientationRotation.hpp OrientationRotation.cpp)

target_link_libraries(OrientationCoordinate.lib spdlog::spdlog)
target_link_libraries(OrientationRotation.lib spdlog::spdlog)

target_link_libraries(Orientation.lib OrientationCoordinate.lib OrientationRotation.lib)