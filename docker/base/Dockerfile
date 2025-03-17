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
    ./vcpkg install openssl boost jwt-cpp prometheus-cpp mongo-cxx-driver plog

# Install jwt-cpp
#RUN cd /usr/src && \
#    git clone https://github.com/Thalhammer/jwt-cpp.git && \
#    cd jwt-cpp && \
#    cmake . && \
#    cmake --install .

# Prometheus-CPP
#RUN cd /usr/src && \
#    git clone --recursive https://github.com/jupp0r/prometheus-cpp && \
#    cd prometheus-cpp && \
#    cmake . -DBUILD_SHARED_LIBS=ON && \
#    cmake --build . -j16 && \
#    cmake --install .

# Install mongo-cxx-driver
#RUN cd /usr/src && \
#    git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git && \
#    cd mongo-cxx-driver && \
#    cmake -DCMAKE_CXX_STANDARD=20 -DENABLE_UNINSTALL=OFF . && \
#    cmake --build . --config Release -j 16 && \
#    cmake --install . --prefix /usr/local

# Install Boost 1.87
#RUN cd /usr/src && \
#    wget https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.gz && \
#    tar -xzf boost_1_87_0.tar.gz && \
#    cd boost_1_87_0 && \
#    ./bootstrap.sh && \
#    ./b2 install

# Cleanup
RUN rm -rf /usr/src
