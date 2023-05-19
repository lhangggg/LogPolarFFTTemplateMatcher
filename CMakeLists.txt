# ----------------------------------------------------------------------------
#  Minimal Cmake version
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

project(fourier_mellin_transform VERSION 0.0.1 LANGUAGES CXX)

option(BUILD_APPS "Build apps" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)
option(GENORATE_VS_DEBUG_INFORM "Generate vs bebug information" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/infra)
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_${TARGET_ARCHITECTURE}_${CMAKE_BUILD_TYPE})

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

if(MSVC) 
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

    # warning level 4 and all warnings as errors
    add_compile_options(
        /W3
        # disable the following warnings
        /wd4275 /wd4251 /wd4068 /wd4819 /wd4309
    )

    if(GENORATE_VS_DEBUG_INFORM)
        message(STATUS "Generate vs debug information")
        add_compile_options(/Zi)
    endif()
else() # linux
    # lots of warnings and all warnings as errors
    add_compile_options(
        -Wall # -Wextra -pedantic -Werror

        # disable the following warnings
        # -Wno-overloaded-virtual
        -Wno-missing-braces
    )
endif()

set(OpenCV_DIR ${PROJECT_SOURCE_DIR}/3rdparty_linux/opencv_linux/lib/cmake/opencv4)
if(MSVC)
    set(OpenCV_DIR ${PROJECT_SOURCE_DIR}/3rdparty_linux/opencv_linux/lib/cmake/opencv4)
endif()

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR})

include(GNUInstallDirs)

# source code
add_subdirectory(src)

# build apps
if(BUILD_APPS)
    message(STATUS "Build apps project...")
    include(infra/Apps.cmake)
    add_subdirectory(apps)
endif()