FROM ubuntu:noble
LABEL authors="icpmoles"

RUN apt-get update && apt-get install -y --no-install-recommends \
  libcgal-dev \
  libcgal-qt5-dev \
  libboost-program-options1.83-dev \
  make \
  cmake \
  g++ \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /project/build


