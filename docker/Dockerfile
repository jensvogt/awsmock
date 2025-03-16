FROM jensvogt/awsmock-base:latest AS build

# Create build environment
RUN mkdir -p /usr/src
RUN git config --global http.postBuffer 524288000
RUN git config --global http.version HTTP/1.1

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    cmake . && \
    cmake --build . --config Release -j 16 && \
    cmake --install .

# Cleanup
RUN rm -rf /usr/src

FROM node:21-alpine AS buildfrontend

RUN apk update && apk add git

# Install aws-mock-ui
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock-ui.git && \
    cd /usr/src/awsmock-ui && \
    npm install -g @angular/cli && \
    npm install && \
    npm run build --prod

# Stick to alpine:latest
FROM alpine:latest

RUN apk update && apk add plog yaml-cpp libarchive libmagic

#aws-cli boost-program_options boost-filesystem boost-thread boost-url boost-json boost-locale

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

# Install Mongo C/C++ driver
COPY --from=build /usr/local/lib/libbson-1.0.so.0.0.0 /usr/lib/
RUN ln -s /usr/lib/libbson-1.0.so.0.0.0 /usr/lib/libbson-1.0.so.0
RUN ln -s /usr/lib/libbson-1.0.so.0 /usr/lib/libbson-1.0.so
COPY --from=build /usr/local/lib/libmongoc-1.0.so.0.0.0 /usr/lib/
RUN ln -s /usr/lib/libmongoc-1.0.so.0.0.0 /usr/lib/libmongoc-1.0.so.0
RUN ln -s /usr/lib/libmongoc-1.0.so.0 /usr/lib/libmongoc-1.0.so
COPY --from=build /usr/local/lib/libmongocxx.so.4.0.0 /usr/lib/
RUN ln -s /usr/lib/libmongocxx.so.4.0.0 /usr/lib/libmongocxx.so._noabi
COPY --from=build /usr/local/lib/libbsoncxx.so.4.0.0 /usr/lib/
RUN ln -s /usr/lib/libbsoncxx.so.4.0.0 /usr/lib/libbsoncxx.so._noabi
RUN ln -s /usr/lib/libbsoncxx.so._noabi /usr/lib/libbsoncxx.so

# Install prometheus
COPY --from=build /usr/local/lib/libprometheus-cpp-core.so.1.3.0 /usr/lib/
RUN ln -s /usr/lib/libprometheus-cpp-core.so.1.3.0 /usr/lib/libprometheus-cpp-core.so.1.3
RUN ln -s /usr/lib/libprometheus-cpp-core.so.1.3 /usr/lib/libprometheus-cpp-core.so
COPY --from=build /usr/local/lib/libprometheus-cpp-pull.so.1.3.0 /usr/lib/
RUN ln -s /usr/lib/libprometheus-cpp-pull.so.1.3.0 /usr/lib/libprometheus-cpp-pull.so.1.3
RUN ln -s /usr/lib/libprometheus-cpp-pull.so.1.3 /usr/lib/libprometheus-cpp-pull.so
COPY --from=build /usr/local/lib/libprometheus-cpp-push.so.1.3.0 /usr/lib/
RUN ln -s /usr/lib/libprometheus-cpp-push.so.1.3.0 /usr/lib/libprometheus-cpp-push.so.1.3
RUN ln -s /usr/lib/libprometheus-cpp-push.so.1.3 /usr/lib/libprometheus-cpp-push.so

# Install Boost libraries
COPY --from=build /usr/local/lib/libboost_json.so.1.87.0 /usr/local/lib/libboost_json.so.1.87.0
COPY --from=build /usr/local/lib/libboost_thread.so.1.87.0 /usr/local/lib/libboost_thread.so.1.87.0
COPY --from=build /usr/local/lib/libboost_program_options.so.1.87.0 /usr/local/lib/libboost_program_options.so.1.87.0
COPY --from=build /usr/local/lib/libboost_locale.so.1.87.0 /usr/local/lib/libboost_locale.so.1.87.0
COPY --from=build /usr/local/lib/libboost_url.so.1.87.0 /usr/local/lib/libboost_url.so.1.87.0
COPY --from=build /usr/local/lib/libboost_filesystem.so.1.87.0 /usr/local/lib/libboost_filesystem.so.1.87.0

# Install aws-mock
COPY --from=build /usr/local/awsmock/bin/awsmockmgr /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awsmockctl /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/bin/awslocal /usr/local/awsmock/bin
COPY --from=build /usr/local/awsmock/lib/libawsmock* /usr/local/awsmock/lib
COPY --from=build /usr/local/awsmock/etc/awsmock.yml /usr/local/awsmock/etc

# Set library search path
ENV LD_LIBRARY_PATH /usr/local/awsmock/lib:$LD_LIBRARY_PATH

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
