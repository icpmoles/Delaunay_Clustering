#!/bin/bash

set -e


cp -rf /project_source/* /project/
cd /project/
cmake -S . -B build_dir -DCMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=build_dir/install
cmake --build build_dir -j 4
cmake --install build_dir