FROM gcc:13.4.0-bookworm

RUN apt update && apt-get install -y groff make cmake ccache git libmagic-dev doxygen zip unzip libtool linux-libc-dev  \
    pandoc wget libarchive-dev libmagic-dev libssh-dev automake autoconf libtool

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

# Install mongo-c-driver
RUN cd /usr/src && \
    git clone -b r2.0 https://github.com/mongodb/mongo-c-driver.git && \
    cd mongo-c-driver && \
    cmake . -DCMAKE_BUILD_TYPE=Release -DENABLE_MONGOC=ON && \
    cmake --build . -j16 && \
    cmake --install .

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver && \
    cmake . -DCMAKE_CXX_STANDARD=17 && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --prefix /usr/local

# Install boost 1.88
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/release/1.89.0/source/boost_1_89_0.tar.gz && \
    tar -xzf boost_1_89_0.tar.gz && \
    cd boost_1_89_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 -j 16 && \
    ./b2 install

# Install libmagic \
RUN cd /usr/src && \
    git clone https://github.com/file/file.git && \
    cd file && \
    libtoolize --force && \
    aclocal && \
    autoheader && \
    automake --force-missing --add-missing && \
    autoconf && \
    ./configure && \
    make && \
    make install
