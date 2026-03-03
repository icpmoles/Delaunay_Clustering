# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.

cmake_minimum_required(VERSION 3.1...3.23)
project(Delaunay_Clustering)

find_package(CGAL REQUIRED OPTIONAL_COMPONENTS Qt5)


if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

# create a target per cppfile
create_single_source_cgal_program(polygon_triangulation.cpp)
include_directories(polygon_triangulation include)

if (CGAL_Qt5_FOUND AND Boost_FOUND)
    target_link_libraries(polygon_triangulation PUBLIC CGAL::CGAL_Basic_viewer Boost::program_options)
else ()
    message(STATUS "NOTICE: This project requires Qt5 and will not be compiled.")
endif ()
