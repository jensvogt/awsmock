FROM gcc:13.2.0-bookworm

RUN apt update && apt-get install -y groff make cmake ccache git libmagic-dev doxygen zip unzip libtool linux-libc-dev  \
    pandoc wget libarchive-dev libmagic-dev libgtest-dev libgmock-dev

# Create build environment
RUN mkdir -p /usr/src

# Install plog
RUN cd /usr/src && \
    git clone https://github.com/SergiusTheBest/plog.git && \
    cd plog && \
    cmake . -DPLOG_BUILD_SAMPLES=OFF && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --prefix /usr/local

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
    cmake . && \
    cmake --build . -j16 && \
    cmake --install .

# Yaml-CPP
RUN cd /usr/src && \
    git clone https://github.com/jbeder/yaml-cpp.git && \
    cd yaml-cpp && \
    cmake . && \
    cmake --build . --config Release -j16 && \
    cmake --install . --prefix /usr/local

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/v3.10 https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver && \
    cmake -DCMAKE_CXX_STANDARD=20 -DENABLE_UNINSTALL=OFF . && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --prefix /usr/local

# Install boost 1.87
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.gz && \
    tar -xzf boost_1_87_0.tar.gz && \
    cd boost_1_87_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 -j 16 && \
    ./b2 install

