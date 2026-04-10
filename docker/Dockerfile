FROM alpine:latest AS build

#RUN apk update && apt install -y ca-certificates git cmake build-essential gcc-13 g++-13 cmake libicu-dev curl unzip \
#    doxygen pandoc autoconf autoconf-archive automake libtool curl zip unzip tar wget pkg-config ninja-build perl \
#    linux-headers-$(uname -r) && \
#    apt clean && \
#    rm -rf /var/lib/apt/lists/*
RUN apk add --no-cache build-base cmake ninja git perl zip unzip curl tar pkgconfig linux-headers bash python3 autoconf \
     autoconf-archive automake libtool m4 make gettext-dev patch pkgconf ncurses-dev ncurses-terminfo-base doxygen pandoc \
     pkgconfig coreutils bash perl

# vcpkg needs this
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV VCPKG_ROOT=/usr/local/vcpkg
ENV VCPKG_MAX_CONCURRENCY=4
ENV CMAKE_BUILD_PARALLEL_LEVEL=4
ENV ACLOCAL_PATH=/usr/share/aclocal

WORKDIR /usr/local/vcpkg

RUN git clone https://github.com/microsoft/vcpkg.git .
RUN ./bootstrap-vcpkg.sh

# libssh workaround
ENV X_VCPKG_ASSET_SOURCES="clear;x-script,wget -nv -c --tries=30 -O {dst} {url}"

# Create build environment
RUN mkdir -p /usr/src

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    mkdir build && \
    cmake -S . -B build -DBUILD_DOCUMENTATION=ON -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_OVERLAY_TRIPLETS=./dist/vcpkg -DVCPKG_TARGET_TRIPLET=x64-linux-release -DCMAKE_BUILD_TYPE=Release -Wno-dev && \
    cmake --build build

FROM node:24-alpine AS buildfrontend

RUN apk update && apk upgrade && apk add --no-cache git

# Build aws-mock-ui
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock-ui.git && \
    cd /usr/src/awsmock-ui && \
    npm install -g @angular/cli && \
    npm install --force && \
    npm run build --prod

# Stick to alpine:latest
FROM alpine:latest

RUN apk update && apk upgrade && apk add --no-cache unzip libssh libarchive libmagic curl aws-cli
#RUN apt update && apt upgrade -y && apt install -y curl unzip libssh-4 libarchive13 libmagic1

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
    mkdir -p /usr/local/awsmock/etc && \
    mkdir -p /usr/local/awsmock/init && \
    mkdir -p /usr/local/awsmock/log && \
    mkdir -p /usr/local/awsmock/tmp && \
    mkdir -p /usr/local/awsmock/frontend

# Install frontend
COPY --from=buildfrontend /usr/src/awsmock-ui/dist/awsmock-ui/browser/ /usr/local/awsmock/frontend/

# Install aws-mock
COPY --from=build /usr/src/awsmock/build/awsmockmgr /usr/local/bin
COPY --from=build /usr/src/awsmock/build/awsmockctl /usr/local/bin
COPY --from=build /usr/src/awsmock/build/awslocal /usr/local/bin
COPY --from=build /usr/src/awsmock/dist/etc/magic.mgc /usr/local/awsmock/etc
COPY --from=build /usr/src/awsmock/dist/etc/awsmock_linux.json /usr/local/awsmock/etc/awsmock.json
COPY --from=build /usr/src/awsmock/dist/etc/ssh_host_key /usr/local/awsmock/etc/ssh_host_key

# Set library search path
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN ln -s /usr/local/bin/awslocal /usr/bin/awslocal || true
RUN ln -s /usr/local/bin/awsmockctl /usr/bin/awsmockctl || true
RUN ln -s /usr/local/bin/awsmockmgr /usr/bin/awsmockmgr || true

# Expose manager/awslocal vcpkg-ports
EXPOSE 4566-4567 4566-4567
# Prometheus vcpkg-ports
EXPOSE 9091 9091
# FTP server vcpkg-ports
EXPOSE 21 21
EXPOSE 2121 2121
EXPOSE 2222 2222
# FTP passive mode vcpkg-ports
EXPOSE 6000-6100 6000-6100

VOLUME /var/run/docker.sock /var/run/docker.sock

CMD ["/usr/local/bin/awsmockmgr"]
