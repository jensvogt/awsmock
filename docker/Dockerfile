FROM debian:stable-slim AS build

RUN apt update && apt install -y ca-certificates git cmake build-essential gcc-13 g++-13 cmake libicu-dev curl unzip \
    doxygen pandoc automake autoconf libarchive-dev libmagic-dev python3 python3-dev libssh-dev wget libtool
RUN apt clean

# Create build environment
RUN mkdir -p /usr/src

# Install jwt-cpp
RUN cd /usr/src && \
    git clone https://github.com/Thalhammer/jwt-cpp.git && \
    cd jwt-cpp && \
    cmake . && \
    cmake --install .

# 1️⃣ Install Mongo C Driver first
RUN cd /usr/src && \
    git clone https://github.com/mongodb/mongo-c-driver.git && \
    cd mongo-c-driver && \
    mkdir cmake-build && cd cmake-build && \
    cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
      -DENABLE_SASL=OFF \
      -DENABLE_STATIC=ON \
      -DENABLE_TESTS=OFF \
      -DENABLE_EXAMPLES=OFF && \
    cmake . && \
    cmake --build . -j$(($(nproc)/2)) && \
    cmake --install .

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver && \
    cmake . -DCMAKE_CXX_STANDARD=17 && \
    cmake --build . --config Release -j$(($(nproc)/2)) && \
    cmake --install . --prefix /usr/local

# Install boost 1.89
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/release/1.83.0/source/boost_1_89_0.tar.gz && \
    tar -xzf boost_1_89_0.tar.gz && \
    cd boost_1_89_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 install -j$(($(nproc)/2))

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

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/awsmock && \
    cmake --build . --config Release -j$(($(nproc)/2)) && \
    cmake --install . --config Release

FROM node:22-bookworm AS buildfrontend

RUN apt update && apt-get install -y git

# Install aws-mock-ui
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock-ui.git && \
    cd /usr/src/awsmock-ui && \
    npm install -g @angular/cli && \
    npm install --force && \
    npm run build --prod

# Stick to alpine:latest
FROM debian:stable-slim

RUN apt update && apt upgrade && apt install -y curl unzip libssh-4 libarchive13 libmagic1

# Make directories
RUN mkdir -p /usr/local/awsmock/data/s3 && \
    mkdir -p /usr/local/awsmock/data/sqs && \
    mkdir -p /usr/local/awsmock/data/sns && \
    mkdir -p /usr/local/awsmock/data/lambda && \
    mkdir -p /usr/local/awsmock/data/transfer && \
    mkdir -p /usr/local/awsmock/data/dynamodb && \
    mkdir -p /usr/local/awsmock/data/backup && \
    mkdir -p /usr/local/awsmock/data/tmp && \
    mkdir -p /usr/local/awsmock/data/application && \
    mkdir -p /usr/local/awsmock/bin && \
    mkdir -p /usr/local/awsmock/lib && \
    mkdir -p /usr/local/awsmock/etc && \
    mkdir -p /usr/local/awsmock/init && \
    mkdir -p /usr/local/awsmock/log && \
    mkdir -p /usr/local/awsmock/tmp && \
    mkdir -p /usr/local/awsmock/frontend

# Install AWS CLI (v2, latest)
RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip" \
 && unzip -qq awscliv2.zip \
 && ./aws/install \
 && rm -rf awscliv2.zip aws

# Just in case, ensure it's available at /usr/local/bin/aws
RUN ln -s /usr/local/bin/aws /usr/bin/aws || true

# Install frontend
COPY --from=buildfrontend /usr/src/awsmock-ui/dist/awsmock-ui/browser/ /usr/local/awsmock/frontend/

# Install aws-mock
COPY --from=build /usr/local/awsmock/bin/awsmockmgr /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awsmockctl /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awslocal /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/lib/* /usr/local/awsmock/lib
COPY --from=build /usr/local/awsmock/etc/* /usr/local/awsmock/etc
COPY --from=build /usr/local/lib/* /usr/local/lib

# Set library search path
ENV LD_LIBRARY_PATH=/usr/local/awsmock/lib:/usr/local/lib
RUN ln -s /usr/local/awsmock/bin/awslocal /usr/bin/awslocal || true
RUN ln -s /usr/local/awsmock/bin/awsmockctl /usr/bin/awsmockctl || true
RUN ln -s /usr/local/awsmock/bin/awsmockmgr /usr/bin/awsmockmgr || true

# Expose manager/awslocal ports
EXPOSE 4566-4567 4566-4567
# Prometheus ports
EXPOSE 9091 9091
# FTP server ports
EXPOSE 21 21
EXPOSE 2121 2121
EXPOSE 2222 2222
# FTP passive mode ports
EXPOSE 6000-6100 6000-6100

VOLUME /var/run/docker.sock /var/run/docker.sock

CMD ["/usr/local/awsmock/bin/awsmockmgr"]
