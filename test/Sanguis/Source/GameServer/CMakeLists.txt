find_package(asio CONFIG REQUIRED)

add_executable(GameServer Private/main.cpp)
add_executable(GameTestClient Private/test_client.cpp)
target_link_libraries(GameServer PRIVATE asio::asio)
target_link_libraries(GameTestClient PRIVATE asio::asio)
