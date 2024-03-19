find_package(asio CONFIG REQUIRED)

# Gather all .cpp files in the current directory
file(GLOB_RECURSE ALL_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/Private *.cpp)

# Create an empty list for our filtered sources
set(FILTERED_CPP "")

# Iterate over all .cpp files
foreach(SOURCE_FILE ${ALL_CPP})
    # Get the filename without the directory
    get_filename_component(FILENAME ${SOURCE_FILE} NAME)

    # Check if the file name starts with a lowercase letter
    if(NOT ${FILENAME} MATCHES "^[a-z]")
        # Add the file to the filtered sources list
        list(APPEND FILTERED_CPP ${CMAKE_CURRENT_SOURCE_DIR}/Private/${SOURCE_FILE})
    endif()
endforeach()

message("Net files: ${FILTERED_CPP}")
add_library(GameServer.NetLibs.lib ${FILTERED_CPP})
target_link_libraries(GameServer.NetLibs.lib asio::asio )


add_executable(GameServer Private/main.cpp)
add_executable(GameTestClient Private/test_client.cpp)
target_link_libraries(GameServer PRIVATE GameServer.NetLibs.lib)
target_link_libraries(GameTestClient PRIVATE GameServer.NetLibs.lib)
