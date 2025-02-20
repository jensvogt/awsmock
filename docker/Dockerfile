FROM jensvogt/awsmock-base:latest AS build

# Create build environment
RUN mkdir -p /usr/src
RUN git config --global http.postBuffer 524288000
RUN git config --global http.version HTTP/2

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

RUN apk update && apk add aws-cli boost-program_options boost-filesystem boost-thread boost-url boost-locale plog  \
    yaml-cpp libarchive libmagic

# Make directories
RUN mkdir -p /home/awsmock/data/s3 && \
    mkdir -p /home/awsmock/data/sqs && \
    mkdir -p /home/awsmock/data/sns && \
    mkdir -p /home/awsmock/data/lambda && \
    mkdir -p /home/awsmock/data/transfer && \
    mkdir -p /home/awsmock/data/dynamodb && \
    mkdir -p /home/awsmock/data/tmp && \
    mkdir -p /home/awsmock/init && \
    mkdir -p /home/awsmock/frontend

# Install frontend
COPY --from=buildfrontend /usr/src/awsmock-ui/dist/awsmock-ui/browser/ /home/awsmock/frontend/

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

# Install aws-mock
COPY --from=build /usr/bin/aws /usr/bin/
COPY --from=build /usr/local/bin/awsmockmgr /usr/bin/
COPY --from=build /usr/local/bin/awsmockctl /usr/bin/
COPY --from=build /usr/local/lib/libawsmock* /usr/lib/
COPY --from=build /etc/awsmock.yml /etc/
COPY --from=build /usr/local/bin/awslocal /usr/bin/

# Configure awslocal
RUN aws configure set aws_access_key_id none && \
    aws configure set aws_secret_access_key none && \
    aws configure set default.region eu-central-1 && \
    aws configure set profile.awsmock.region eu-central-1 && \
    aws configure set profile.awsmock.aws_access_key_id none && \
    aws configure set profile.awsmock.aws_secret_access_key none

# Expose ports
# Manager/Controller
EXPOSE 4566-4567 4566-4567
# Prometheus
EXPOSE 9091 9091
# FTP server
EXPOSE 21 21
# FTP passive mode
EXPOSE 6000-6010 6000-6010

VOLUME /var/run/docker.sock /var/run/docker.sock

CMD ["/usr/bin/awsmockmgr"]
