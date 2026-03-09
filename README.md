# Build Requirements:

- CMAKE
- CGAL 5.6.3
- Boost Libraries
- QT5
- libmetis-dev

Tested on Ubuntu 24.04

To compile:

```
cmake -S . -B build_dir -DCMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=build_dir/install
cmake --build build_dir -j 4
cmake --install build_dir
```

To run:

```
cd build_dir/install/bin/
./polygon_triangulation
```

## Docker Alternative

```
docker-compose build 
docker-compose up
```

Then run with:

```
.bin/build_dir/install/bin/polygon_triangulation --help
```

# Run

After building the binaries can be shared. To run them install the dependencies:

```
sudo apt update
sudo apt install \
    libcgal-qt5-dev \
    libcgal-dev \
    libmetis-dev
```