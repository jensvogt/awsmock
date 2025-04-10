FROM jensvogt/awsmock-base:latest AS build

# Create build environment
RUN mkdir -p /usr/src
RUN git config --global http.postBuffer 524288000
RUN git config --global http.version HTTP/1.1

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/awsmock && \
    cmake --build . --config Release -j 16 && \
    cmake --install . --config Release

FROM node:21-bookworm AS buildfrontend

RUN apt update && apt-get install -y git

# Install aws-mock-ui
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock-ui.git && \
    cd /usr/src/awsmock-ui && \
    npm install -g @angular/cli && \
    npm install && \
    npm run build --prod

# Stick to alpine:latest
FROM debian:trixie-slim

RUN apt update && apt upgrade -y && apt -y install libarchive-dev libmagic-dev libicu-dev libicu72 libssh-4

# Make directories
RUN mkdir -p /usr/local/awsmock/data/s3 && \
    mkdir -p /usr/local/awsmock/data/sqs && \
    mkdir -p /usr/local/awsmock/data/sns && \
    mkdir -p /usr/local/awsmock/data/lambda && \
    mkdir -p /usr/local/awsmock/data/transfer && \
    mkdir -p /usr/local/awsmock/data/dynamodb && \
    mkdir -p /usr/local/awsmock/data/tmp && \
    mkdir -p /usr/local/awsmock/bin && \
    mkdir -p /usr/local/awsmock/lib && \
    mkdir -p /usr/local/awsmock/etc && \
    mkdir -p /usr/local/awsmock/init && \
    mkdir -p /usr/local/awsmock/frontend

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

# Expose manager/awslocal ports
EXPOSE 4566-4567 4566-4567
# Prometheus ports
EXPOSE 9091 9091
# FTP server ports
EXPOSE 21 21
# FTP passive mode ports
EXPOSE 6000-6100 6000-6100

VOLUME /var/run/docker.sock /var/run/docker.sock

CMD ["/usr/local/awsmock/bin/awsmockmgr"]
