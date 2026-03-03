#!/bin/bash

set -e


cp -rf /project_source /project
cd /project/
ls
cd build

cmake -S /project_source -DCMAKE_BUILD_TYPE=Release
cmake --build . -j 4
ls
cp polygon_triangulation bin/
#cp -r ../data bin/