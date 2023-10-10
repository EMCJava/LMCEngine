add_library(NetControllerLib NetController.hpp NetController.cpp)
add_library(NetServerLib NetServer.hpp NetServer.cpp)
add_library(NetClientLib NetClient.hpp NetClient.cpp)

target_link_libraries(NetControllerLib spdlog::spdlog)
target_link_libraries(NetServerLib NetControllerLib)
target_link_libraries(NetClientLib NetServerLib NetControllerLib)