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
    cmake --build . --config Release -j 8 && \
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
FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade -y && apt -y install libarchive-dev libmagic-dev libicu-dev libssh-4 libquadmath0 curl unzip groff less

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
RUN rm -rf /lib/x86_64-linux-gnu/libmagic* || true

# Install frontend
COPY --from=buildfrontend /usr/src/awsmock-ui/dist/awsmock-ui/browser/ /usr/local/awsmock/frontend/

# Install aws-mock
COPY --from=build /usr/local/awsmock/bin/awsmockmgr /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awsmockctl /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awslocal /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/lib/* /usr/local/awsmock/lib
COPY --from=build /usr/local/awsmock/etc/* /usr/local/awsmock/etc
COPY --from=build /usr/local/lib/* /usr/local/lib
COPY --from=build /lib/x86_64-linux-gnu/libicudata.so.72 /lib/x86_64-linux-gnu/
COPY --from=build /lib/x86_64-linux-gnu/libicui18n.so.72 /lib/x86_64-linux-gnu/
COPY --from=build /lib/x86_64-linux-gnu/libicuuc.so.72 /lib/x86_64-linux-gnu/

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
