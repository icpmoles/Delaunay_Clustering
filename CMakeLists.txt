# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.
cmake_minimum_required(VERSION 3.1...3.23)
if (POLICY CMP0167) # Use upstream's FindBoost.cmake to find Boost libraries.
    cmake_policy(SET CMP0167 NEW) # Use upstream's FindBoost.cmake to find Boost libraries.
endif ()

project(Delaunay_Clustering)

find_package(CGAL REQUIRED OPTIONAL_COMPONENTS Qt5)
find_package(MPI REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.83.0 COMPONENTS program_options)
message(STATUS "Boost: ${Boost_VERSION}")
include_directories(${Boost_INCLUDE_DIRS})

# create a target per cppfile
create_single_source_cgal_program(src/polygon_triangulation.cpp)
include_directories(polygon_triangulation include)

### PAR/METIS OPTIONS
target_include_directories(polygon_triangulation PRIVATE
        /usr/include/parmetis
        /usr/include/metis
)

if (CGAL_Qt5_FOUND AND Boost_FOUND)
    target_link_libraries(polygon_triangulation PRIVATE
            CGAL::CGAL_Basic_viewer
            Boost::program_options
            MPI::MPI_C
            parmetis
    )

else ()
    if (!CGAL_Qt5_FOUND)
        message(STATUS "NOTICE: This project requires Qt5 and will not be compiled.")
    endif ()
    if (!Boost_FOUND)
        message(STATUS "NOTICE: This project requires Boost Program Options and will not be compiled.")
    endif ()
endif ()

install(TARGETS polygon_triangulation
        EXPORT CGALTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
# copy data files
install(DIRECTORY "data/" DESTINATION bin/data)
