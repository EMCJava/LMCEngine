if (WIN32)
    set(VCPKG_TARGET_ARCHITECTURE x64)
    set(VCPKG_CRT_LINKAGE dynamic)
    set(VCPKG_LIBRARY_LINKAGE dynamic)
elseif (APPLE)
    set(VCPKG_TARGET_ARCHITECTURE arm64)
    # set(VCPKG_OSX_ARCHITECTURES x86_64)

    set(VCPKG_CRT_LINKAGE dynamic)

    message(WARNING "${PORT}")

    if (PORT MATCHES "[pP][hH][yY][sS][xX]")
        set(VCPKG_LIBRARY_LINKAGE dynamic)
    else ()
        set(VCPKG_LIBRARY_LINKAGE static)
    endif ()

    set(VCPKG_CMAKE_SYSTEM_NAME "Darwin")
elseif (UNIX)
    set(VCPKG_TARGET_ARCHITECTURE x64)
    set(VCPKG_CRT_LINKAGE dynamic)
    set(VCPKG_LIBRARY_LINKAGE dynamic)

    set(VCPKG_CMAKE_SYSTEM_NAME "Linux")
else ()
    message(FATAL_ERROR "Unsupported target platform")
endif ()
