## ---------- Shared source (Hot reload) setting ----------
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(SHARED_BINARY_PATH_FORMAT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}{}.lib${CMAKE_SHARED_LIBRARY_SUFFIX}")

## Write hot reload config in the project config file
file(READ ${PROJECT_PATH_CONFIG_PATH} project_config_data)
string(JSON project_new_config_data SET "${project_config_data}" "shared_library_path_${CMAKE_BUILD_TYPE}_format" "\"${SHARED_BINARY_PATH_FORMAT}\"")
file(WRITE ${PROJECT_PATH_CONFIG_PATH} "${project_new_config_data}")