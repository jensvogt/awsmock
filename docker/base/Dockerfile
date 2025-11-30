FROM alpine:latest

RUN apk update && apk add git g++ make cmake ccache doxygen zip unzip libtool pandoc libarchive-dev libmagic libssh-dev  \
    automake autoconf m4 python3 python3-dev linux-headers

# Create build environment
RUN mkdir -p /usr/src

# Install jwt-cpp
RUN cd /usr/src && \
    git clone https://github.com/Thalhammer/jwt-cpp.git && \
    cd jwt-cpp && \
    cmake . && \
    cmake --install .

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver && \
    cmake . -DCMAKE_CXX_STANDARD=17 && \
    cmake --build . --config Release -j $(nproc) && \
    cmake --install . --prefix /usr/local

# Install boost 1.83
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/release/1.83.0/source/boost_1_83_0.tar.gz && \
    tar -xzf boost_1_83_0.tar.gz && \
    cd boost_1_83_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 install -j $(nproc)

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
