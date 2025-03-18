FROM alpine:latest

RUN apk add --no-cache cmake --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main

RUN apk update && apk add groff g++ make cmake ccache git python3 aws-cli github-cli pandoc icu-libs doxygen curl  \
    libmagic file-dev nodejs npm linux-headers perl build-base zip unzip ninja-is-really-ninja bash autoconf automake \
    autoconf-archive libtool

# Create build environment
RUN mkdir -p /usr/src

# Install vcpkg
RUN cd /usr/src && \
    git clone https://github.com/microsoft/vcpkg.git && \
    cd /usr/src/vcpkg && \
    ./bootstrap-vcpkg.sh && \
    git pull && \
    ./vcpkg update

RUN cd /usr/src/vcpkg && \
    ./vcpkg install openssl boost yaml-cpp jwt-cpp prometheus-cpp mongo-cxx-driver plog libarchive libmagic gtest

