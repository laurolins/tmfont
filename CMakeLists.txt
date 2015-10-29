cmake_minimum_required (VERSION 2.8)
project (tmfont)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
       message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(APPLE)
   set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++" CACHE STRING "compile flags" FORCE)
   set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++" CACHE STRING "compile flags" FORCE)
   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Deployment target for OSX" FORCE)
endif(APPLE)

if(UNIX)
   set(CMAKE_CXX_FLAGS "-std=c++11" CACHE STRING "compile flags" FORCE)
endif(UNIX)

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

message("---- freetype ----")
message("FREETYPE_INCLUDE_DIRS:          ${FREETYPE_INCLUDE_DIRS}")
message("FREETYPE_LIBRARIES:             ${FREETYPE_LIBRARIES}")

add_subdirectory (src)


