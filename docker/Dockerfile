FROM jensvogt/awsmock-base:latest AS build

# Create build environment
RUN mkdir -p /usr/src

# Install aws-mock
WORKDIR /usr/src
RUN git clone https://github.com/jensvogt/awsmock.git && \
    cd /usr/src/awsmock && \
    cmake . && \
    cmake --build . --config Release -j 16 && \
    cmake --install .

# Cleanup
RUN rm -rf /usr/src

# Stick to alpine 3.19 as 3.20 has not support for aws-cli (missing python3.12 compat)
FROM alpine:latest

RUN apk update && apk add aws-cli boost-program_options boost-filesystem boost-thread curl

# Fix timezones
ENV TZ=Europe/Berlin
RUN apk add -U tzdata
RUN cp /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Make dirctories
RUN mkdir -p /home/awsmock/data/s3 && \
    mkdir -p /home/awsmock/data/lambda && \
    mkdir -p /home/awsmock/data/transfer && \
    mkdir -p /home/awsmock/data/dynamodb && \
    mkdir -p /home/awsmock/data/tmp

# Runtime

COPY --from=build /usr/lib/libstdc++.so.6 /usr/lib/
COPY --from=build /usr/lib/libgcc_s.so.1 /usr/lib/
COPY --from=build /usr/lib/libicuuc.so.74 /usr/lib/
COPY --from=build /usr/lib/libcurl.so.4 /usr/lib/
COPY --from=build /usr/lib/libarchive.so.13 /usr/lib/
COPY --from=build /usr/lib/libzstd.so.1 /usr/lib/
COPY --from=build /usr/lib/libexpat.so.1 /usr/lib/
COPY --from=build /usr/lib/liblzma.so.5 /usr/lib/
COPY --from=build /usr/lib/liblz4.so.1 /usr/lib/
COPY --from=build /usr/lib/libbz2.so.1 /usr/lib/
COPY --from=build /usr/lib/libcares.so.2 /usr/lib/
COPY --from=build /usr/lib/libnghttp2.so.14 /usr/lib/
COPY --from=build /usr/lib/libidn2.so.0 /usr/lib/
COPY --from=build /usr/lib/libbrotlidec.so.1 /usr/lib/
COPY --from=build /usr/lib/libicudata.so.74 /usr/lib/
COPY --from=build /usr/lib/libunistring.so.5 /usr/lib/
COPY --from=build /usr/lib/libbrotlicommon.so.1 /usr/lib
COPY --from=build /lib/libacl.so.1.1.2302 /lib/
RUN ln -s /lib/libacl.so.1.1.2302 /lib/libacl.so.1

# Install poco
COPY --from=build /usr/local/lib/libPocoPrometheus.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoNet.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoJSON.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoXML.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoZip.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoUtil.so.103 /usr/lib/
COPY --from=build /usr/local/lib/libPocoFoundation.so.103 /usr/lib/
RUN ln -s /usr/lib/libPocoPrometheus.so.103 /usr/lib/libPocoPrometheus.so
RUN ln -s /usr/lib/libPocoNet.so.103 /usr/lib/libPocoNet.so
RUN ln -s /usr/lib/libPocoJSON.so.103 /usr/lib/libPocoJSON.so
RUN ln -s /usr/lib/libPocoXML.so.103 /usr/lib/libPocoXML.so
RUN ln -s /usr/lib/libPocoZip.so.103 /usr/lib/libPocoZip.so
RUN ln -s /usr/lib/libPocoUtil.so.103 /usr/lib/libPocoUtil.so
RUN ln -s /usr/lib/libPocoFoundation.so.103 /usr/lib/libPocoFoundation.so

# Install Mongo C/C++ driver
COPY --from=build /usr/local/lib/libbson-1.0.so.0.0.0 /usr/lib/
RUN ln -s /usr/lib/libbson-1.0.so.0.0.0 /usr/lib/libbson-1.0.so.0
RUN ln -s /usr/lib/libbson-1.0.so.0 /usr/lib/libbson-1.0.so
COPY --from=build /usr/local/lib/libmongoc-1.0.so.0.0.0 /usr/lib/
RUN ln -s /usr/lib/libmongoc-1.0.so.0.0.0 /usr/lib/libmongoc-1.0.so.0
RUN ln -s /usr/lib/libmongoc-1.0.so.0 /usr/lib/libmongoc-1.0.so
COPY --from=build /usr/local/lib/libmongocxx.so.3.12.0-pre /usr/lib/
RUN ln -s /usr/lib/libmongocxx.so.3.12.0-pre /usr/lib/libmongocxx.so._noabi
COPY --from=build /usr/local/lib/libbsoncxx.so.3.12.0-pre /usr/lib/
RUN ln -s /usr/lib/libbsoncxx.so.3.12.0-pre /usr/lib/libbsoncxx.so._noabi
RUN ln -s /usr/lib/libbsoncxx.so._noabi /usr/lib/libbsoncxx.so

# Install boost
COPY --from=build /usr/lib/libboost_program_options.so.1.84.0 /usr/lib/
COPY --from=build /usr/lib/libboost_thread.so.1.84.0 /usr/lib/
COPY --from=build /usr/lib/libboost_filesystem.so.1.84.0 /usr/lib/
RUN ln -s /usr/lib/libboost_program_options.so.1.84.0 /usr/lib/libboost_program_options.so
RUN ln -s /usr/lib/libboost_thread.so.1.84.0 /usr/lib/libboost_thread.so
RUN ln -s /usr/lib/libboost_filesystem.so.1.84.0 /usr/lib/libboost_filesystem.so

# Install aws-mock
COPY --from=build /usr/bin/aws /usr/bin/
COPY --from=build /usr/local/bin/awsmockmgr /usr/bin/
COPY --from=build /usr/local/bin/awsmockctl /usr/bin/
COPY --from=build /usr/local/lib/libawsmock* /usr/lib/
COPY --from=build /etc/awsmock.properties /etc/
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
