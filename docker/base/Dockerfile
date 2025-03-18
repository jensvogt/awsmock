FROM gcc:14

#RUN apk add --no-cache cmake --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main

RUN apt update && apt-get install -y groff make cmake ccache git libmagic-dev doxygen zip unzip autoconf automake \
    autoconf-archive libtool linux-libc-dev pandoc

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

