FROM gcc:13.2.0-bookworm

RUN apt update && apt-get install -y groff make cmake ccache git libmagic-dev doxygen zip unzip libtool linux-libc-dev  \
    pandoc wget libarchive-dev libmagic-dev libssh-dev

# Create build environment
RUN mkdir -p /usr/src

# Install jwt-cpp
RUN cd /usr/src && \
    git clone https://github.com/Thalhammer/jwt-cpp.git && \
    cd jwt-cpp && \
    cmake . && \
    cmake --install .

# Prometheus-CPP
RUN cd /usr/src && \
    git clone --recursive https://github.com/jupp0r/prometheus-cpp && \
    cd prometheus-cpp && \
    cmake . -DBUILD_SHARED_LIBS=ON -DENABLE_PUSH=OFF && \
    cmake --build . -j16 && \
    cmake --install .

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/v3.10 https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver && \
    cmake -DCMAKE_CXX_STANDARD=20 -DENABLE_UNINSTALL=OFF . && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --prefix /usr/local

# Install boost 1.88
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/beta/1.88.0.beta1/source/boost_1_88_0_b1.tar.gz && \
    tar -xzf boost_1_88_0_b1.tar.gz && \
    cd boost_1_88_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 -j 16 && \
    ./b2 install

# Install gtest
RUN cd /usr/src && \
    rm -rf googletest gtest && \
    git clone https://github.com/google/googletest.git && \
    cd googletest && \
    cmake . && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --prefix /usr/local
