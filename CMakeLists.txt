# Project Info

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(HelloTriangle
        VERSION 1.0.0.0
        LANGUAGES C CXX
)

# =============================================================

# CMake Settings

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(DCMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

# =============================================================

# Options

set(XGFX_API VULKAN CACHE STRING "Which graphics API to use, defaults to AUTO, can be NOOP, VULKAN, OPENGL, DIRECTX12, or METAL.")
set_property(
    CACHE
    XGFX_API PROPERTY
    STRINGS NOOP VULKAN OPENGL DIRECTX12 METAL
)

# =============================================================

# Dependencies

# CrossWindow
add_subdirectory(external/crosswindow ${CMAKE_BINARY_DIR}/crosswindow)
set_property(TARGET CrossWindow PROPERTY FOLDER "Dependencies")

# CrossWindow-Graphics
add_subdirectory(external/crosswindow-graphics ${CMAKE_BINARY_DIR}/crosswindow-graphics)

# Cross Graphics Dependencies
if(XGFX_API STREQUAL "VULKAN")
    find_path(VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h HINTS
        "$ENV{VULKAN_SDK}/include"
        "$ENV{VULKAN_SDK}/Include"
        "$ENV{VK_SDK_PATH}/Include")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        find_library(XGFX_LIBRARY
            NAMES vulkan-1 vulkan vulkan.1
            HINTS
            "$ENV{VULKAN_SDK}/lib"
            "$ENV{VULKAN_SDK}/Lib"
            "$ENV{VULKAN_SDK}/Bin"
            "$ENV{VK_SDK_PATH}/Bin")
    else()
        find_library(XGFX_LIBRARY
                    NAMES vulkan-1 vulkan vulkan.1
                    HINTS
            "$ENV{VULKAN_SDK}/Lib32"
            "$ENV{VULKAN_SDK}/Bin32"
            "$ENV{VK_SDK_PATH}/Bin32")
    endif()
endif()


# =============================================================

# Sources

file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${XGFX_API}Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${XGFX_API}Renderer.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# Solution Filters
foreach(source IN LISTS FILE_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    string(REPLACE "src" "" source_path_final "${source_path_msvc}")
    source_group("${source_path_final}" FILES "${source}")
endforeach()

# =============================================================

# Finalize App

xwin_add_executable(
    ${PROJECT_NAME}
    "${FILE_SOURCES}"
)

# =============================================================

# Finish Dependencies

target_link_libraries(
    ${PROJECT_NAME}
    ${XGFX_LIBRARY}
    CrossWindowGraphics
    CrossWindow
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "external/vectormath"
  PUBLIC ${VULKAN_INCLUDE_DIR}
)

target_compile_definitions(
  ${PROJECT_NAME} 
  PUBLIC XGFX_${XGFX_API}=1
)

# =============================================================

# Finish Settings

# Change output dir to bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
# Change working directory to top dir to access `assets/shaders/` folder
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/..)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()