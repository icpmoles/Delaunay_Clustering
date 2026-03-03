#!/bin/bash

set -e

cmake -S .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j 4
ls
cp polygon_triangulation bin/
#cp -r ../data bin/