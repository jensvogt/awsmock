FROM debian:stable-slim AS build

RUN apt update && apt install -y ca-certificates git cmake build-essential gcc-13 g++-13 cmake libicu-dev curl unzip \
    doxygen pandoc automake autoconf libarchive-dev libmagic-dev python3 python3-dev libssh-dev wget libtool
RUN apt clean

# Create build environment
RUN mkdir -p /usr/src
RUN mkdir -p /usr/extern

# Install jwt-cpp
RUN cd /usr/src && \
    git clone https://github.com/Thalhammer/jwt-cpp.git && \
    cd jwt-cpp && \
    mkdir build && \
    cd build && \
    cmake -S .. -B . -DCMAKE_INSTALL_PREFIX=/usr/extern && \
    cmake --build . --parallel $(nproc) && \
    cmake --install .

# 1️⃣ Install Mongo C Driver first
RUN cd /usr/src && \
    git clone https://github.com/mongodb/mongo-c-driver.git && \
    cd mongo-c-driver/build && \
    cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_SHM_COUNTERS=OFF \
             -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DENABLE_PIC=ON \
             -DCMAKE_INSTALL_PREFIX=/usr/extern && \
    cmake --build . && \
    cmake --install .

# Install mongo-cxx-driver
RUN cd /usr/src && \
    git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git && \
    cd mongo-cxx-driver/build && \
    cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
             -DCMAKE_INSTALL_PREFIX=/usr/extern \
             -DMONGOCXX_ENABLE_STATIC=ON -DCMAKE_PREFIX_PATH=/usr/extern && \
    cmake --build . --config Release -j$(($(nproc)/2)) && \
    cmake --install .

# Install boost 1.83
RUN cd /usr/src && \
    wget -nv https://archives.boost.io/release/1.83.0/source/boost_1_83_0.tar.gz && \
    tar -xzf boost_1_83_0.tar.gz && \
    cd boost_1_83_0 && \
    ./bootstrap.sh --prefix=/usr/local && \
    ./b2 install -j$(($(nproc)/2)) > /dev/null

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    mkdir build && \
    cd build && \
    cmake -S .. -B . -DBUILD_DOCUMENTATION=ON -DBSONCXX_INCLUDE_DIR=/usr/extern/include/bsoncxx/v_noabi \
                -DMONGOCXX_INCLUDE_DIR=/usr/extern/include/mongocxx/v_noabi -DEXTERNAL_INCLUDE_DIR=usr/extern/include \
                -DEXTERNAL_LIB_DIR=/usr/extern/lib && \
    cmake --build . --parallel $(($(nproc)/2))

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
COPY --from=build /usr/src/awsmock/build/awsmockmgr /usr/local/awsmock/bin
COPY --from=build /usr/src/awsmock/build/awsmockctl /usr/local/awsmock/bin
COPY --from=build /usr/src/awsmock/build/awslocal /usr/local/awsmock/bin
COPY --from=build /usr/src/awsmock/dist/etc/awsmock_linux.json /usr/local/awsmock/etc/awsmock.json

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
