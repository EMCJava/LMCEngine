add_library(OSFile.lib OSFile.hpp OSFile.cpp)

target_link_libraries(OSFile.lib PRIVATE nativefiledialog.lib)