#!/bin/bash

set -e

cp -rf /project_source/* /project/
cd /project/
cmake -S . -B common_build_dir/build_dir -DCMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=common_build_dir/build_dir/install
cmake --build common_build_dir/build_dir -j 4
cmake --install common_build_dir/build_dir

cmake -S . -B common_build_dir/build_dir_debug -DCMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=common_build_dir/build_dir_debug/install
cmake --build common_build_dir/build_dir_debug -j 4
cmake --install common_build_dir/build_dir_debug