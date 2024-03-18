find_package(asio CONFIG REQUIRED)

add_executable(GameServer Private/main.cpp)
target_link_libraries(GameServer PRIVATE asio::asio)
