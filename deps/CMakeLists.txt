
if (WIN32)
set(OS_SHARED_LIB_SUFFIX .dll)
set(OS_STATIC_LIB_SUFFIX .lib)
else ()
set(OS_SHARED_LIB_SUFFIX .so)
set(OS_STATIC_LIB_SUFFIX .a)
endif ()

include(ExternalProject)

find_library(SYS_BGFX bgfx)
if (SYS_BGFX_FOUND)
    set(BGFX_LIB_NAME ${SYS_BGFX_LIBRARY})
    set(BGFX_LIB_PATH ${SYS_BGFX_LIBRARY})
    set(BGFX_LIB_INCLUDE ${SYS_BGFX_INCLUDE})
else ()
    ExternalProject_Add(bgfx-git
        GIT_REPOSITORY    https://github.com/bkaradzic/bgfx.cmake
        GIT_TAG           v1.115.8144-1b2b921
        CMAKE_ARGS        -DBGFX_INSTALL=OFF -DBGFX_BUILD_EXAMPLES=OFF -DBGFX_CUSTOM_TARGETS=OFF
        INSTALL_COMMAND   ""
    )

    add_library(bgfx IMPORTED STATIC GLOBAL)
    set_target_properties(bgfx PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git-build/bgfx${OS_STATIC_LIB_SUFFIX})
    target_include_directories(bgfx INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git/bgfx/include)

    add_library(bx IMPORTED STATIC GLOBAL)
    set_target_properties(bx PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git-build/bx${OS_STATIC_LIB_SUFFIX})
    target_include_directories(bx INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git/bx/include)

    add_library(bimg IMPORTED STATIC GLOBAL)
    set_target_properties(bimg PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git-build/bimg${OS_STATIC_LIB_SUFFIX})
    target_include_directories(bimg INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/bgfx-git-prefix/src/bgfx-git/bimg/include)
endif ()
