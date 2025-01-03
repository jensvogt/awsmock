FROM jensvogt/awsmock-base:latest AS build

# Create build environment
RUN mkdir -p /usr/src
RUN git config --global http.postBuffer 157286400

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    cmake . && \
    cmake --build . --config Release -j 16 && \
    cmake --install .

# Install aws-mock-ui
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock-ui.git && \
    cd /usr/src/awsmock-ui && \
    npm install -g @angular/cli && \
    npm install && \
    npm run build --prod && \
    cp -R dist/awsmock-ui/browser/ /tmp/frontend

# Cleanup
RUN rm -rf /usr/src

# Stick to alpine:latest
FROM alpine:latest

RUN apk update && apk add aws-cli boost-program_options boost-filesystem boost-program_options boost-filesystem boost-thread boost-url plog yaml-cpp libarchive libmagic

# Fix timezones
ENV TZ=Europe/Berlin
RUN apk add -U tzdata
RUN cp /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Make dirctories
RUN mkdir -p /home/awsmock/data/s3 && \
    mkdir -p /home/awsmock/data/sqs && \
    mkdir -p /home/awsmock/data/sns && \
    mkdir -p /home/awsmock/data/lambda && \
    mkdir -p /home/awsmock/data/transfer && \
    mkdir -p /home/awsmock/data/dynamodb && \
    mkdir -p /home/awsmock/data/tmp && \
    mkdir -p /home/awsmock/frontend

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

# Install aws-mock
COPY --from=build /usr/bin/aws /usr/bin/
COPY --from=build /usr/local/bin/awsmockmgr /usr/bin/
COPY --from=build /usr/local/bin/awsmockctl /usr/bin/
COPY --from=build /usr/local/lib/libawsmock* /usr/lib/
COPY --from=build /etc/awsmock.yml /etc/
COPY --from=build /usr/local/bin/awslocal /usr/bin/
COPY --from=build /tmp/frontend/* /home/awsmock/frontend/

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
