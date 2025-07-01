# Root
cmake_minimum_required(VERSION 3.10)
project(Spark LANGUAGES CXX VERSION 1.0.0)

# Set a default build target if none is specified
if(NOT DEFINED BUILD_TARGET)
    if(EMSCRIPTEN)
        set(BUILD_TARGET web)
    else()
        set(BUILD_TARGET native)
    endif()
endif()

# Provide an option for the user to override
set(BUILD_TARGET ${BUILD_TARGET} CACHE STRING "Specify the build target: native or web")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build.")
endif()

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/extern/${BUILD_TARGET} CACHE PATH "Base directory for fetched content" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<0:>) # The /$<0:> prevents MSVC from adding "Release" / "Debug" directories


# Add subdirectories
add_subdirectory(extern)
add_subdirectory(src)