FROM ubuntu:noble
LABEL authors="icpmoles"

RUN apt-get update && apt-get install -y --no-install-recommends \
  libcgal-dev=5.6-1build3 \
  libcgal-qt5-dev=5.6-1build3 \
  libboost-program-options1.83-dev=1.83.0-2.1ubuntu3.2 \
  libmetis-dev=5.1.0.dfsg-7build3 \
  libparmetis-dev=4.0.3-7build2 \
  make=4.3-4.1build2 \
  cmake=3.28.3-1build7 \
  g++=4:13.2.0-7ubuntu1 \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /project/


