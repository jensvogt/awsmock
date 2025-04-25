# awsmock

awsmock is a high performance, persistent AWS simulation. Currently, the following services are supported:

- [S3](docs/man/awsmocks3.1.md): Amazon Simple Storage Service
- [SQS](docs/man/awsmocksqs.1.md): Amazon Simple Queue Service
- [SNS](docs/man/awsmocksns.1.md): Amazon Simple Notification Service
- [Lambda](docs/man/awsmocklambda.1.md): Amazon Lambda service
- [Cognito](docs/man/awsmockcognito.1.md): Amazon Cognito identity service
- [DynamoDB](docs/man/awsmockdynamodb.1.md): Amazon Dynamo database service
- [Transfer](docs/man/awsmocktransfer.1.md): Amazon transfer family, supporting FTP
- [SecretsManager](docs/man/awsmocktransfer.1.md): Amazon secrets manager service
- [KMS](docs/man/awsmocktransfer.1.md): Amazon key management service
- [SSM](docs/man/awsmockssm.1.md): Amazon Systems management

awsmock is written in C++ with an optional MongoDB persistence layer. If needed all objects can be stored in the
database. The simulation reacts to all AWS commands sent to default port 4566. For a list of supported commands see
below tables for the different AWS services.

AwsMock can also be used without a persistence layer (for instances used as a test-container). In this cas the
configuration and all objects are stored in an in-memory database. As soon as the AwsMock manager goes down all
infrastructures and runtime objects are lost.

For control over the behaviour of the AwsMock simulation a ```awsmockctl``` command line tool is available. This tool
can be used to start/stop/restart the different AwsMock modules, as well as configure some internal stuff needed by the
AwsMock manager.

## S3 module

Data is stored as objects within resources called “buckets,” and a single object can be up to 5 terabytes in size.
Objects can be accessed through S3 Access Points or directly through the bucket hostname. With cost-effective storage
classes and easy-to-use management features, you can optimize costs, organize data, and configure fine-tuned access
controls to meet specific business, organizational, and compliance requirements.

AwsMock S3 mock simulates a real AWS S3 bucket and its object. They can be copied, added, moved, deleted any some more
functions. S3 objects are stored on disk, and their metadata is part of AwsMock Mongo DB collection. Therefore the
MongoDB
has two collections: Buckets and Objects. Buckets contain the bucket metadata and teh object collection the object data.

See [AwsMock S3 supported commands](docs/man/awsmocks3.1.md) in the supplied man page.

## SQS module

Simple Queue Service (SQS) is a managed messaging service offered by AWS. It allows you to decouple different components
of your applications by enabling asynchronous communication through message queues. SQS allows you to reliably send,
store,
and receive messages with support for standard and FIFO queues.

AwsMock allows you to use the SQS APIs in your local environment to integrate and decouple distributed systems via
hosted queues. The supported APIs are available on our API coverage page, which provides information on the extent of
SQS’s integration with AwsMock.

See [AwsMock SQS supported commands](docs/man/awsmocksqs.1.md) for a list of supported commands.

## SNS module

Amazon Simple Notification Service (Amazon SNS) is a managed service that provides message delivery from publishers to
subscribers (also known as producers and consumers). Publishers communicate asynchronously with subscribers by sending
messages to a topic, which is a logical access point and communication channel. Clients can subscribe to the SNS topic
and receive published messages using a supported endpoint type, such as Amazon Kinesis Data Firehose, Amazon SQS,
AWS Lambda, HTTP, email, mobile push notifications, and mobile text messages (SMS).

Currently only SQS is supported as an endpoint type.

See [AwsMock SNS supported commands](docs/man/awsmocksns.1.md) for a list of supported commands.

## Lambdas module

AWS Lambda is a Serverless Function as a Service (FaaS) platform that lets you run code in your preferred programming
language on the AWS ecosystem. AWS Lambda automatically scales your code to meet demand and handles server provisioning,
management, and maintenance. AWS Lambda allows you to break down your application into smaller, independent functions
that integrate seamlessly with AWS services.

AwsMock supports Lambdas, allowing you to use the Lambda API to create, deploy, and test your Lambda functions. The
supported APIs are available on our Lambda coverage page, which provides information on the extent of Lambda’s
integration with AwsMock.

See [AwsMock Lambda supported commands](docs/man/awsmocklambda.1.md) for a list of supported commands.

## Transfer family

AWS Transfer Family is a secure transfer service that enables you to transfer files into and out of AWS storage
services. Transfer Family is part of the AWS Cloud platform. AWS Transfer Family offers fully managed support for the
transfer of files over SFTP, AS2, FTPS, and FTP directly into and out of Amazon S3 or Amazon EFS. You can seamlessly
migrate, automate, and monitor your file transfer workflows by maintaining existing client-side configurations for
authentication, access, and firewalls—so nothing changes for your customers, partners, and internal teams, or their
applications.

AwsMock transfer manager supports an FTP server, which writes the incoming files to a S3 transfer bucket. This transfer
manager support fully supports the FTP protocol (FTPS and SFTP are currently not supported). Uploading a file to the
FTP server will automatically create a S3 object in the bucket ```transfer-server```. The key of the S3 object will be
the full path of the FTP file, including the uploaded directory.

## KMS

AWS Key Management Service (AWS KMS) is a managed service that makes it easy for you to create and control the
cryptographic keys that are used to protect your data. AWS KMS uses hardware security modules (HSM) to protect and
validate your AWS KMS keys under the FIPS 140-2 Cryptographic Module Validation Program. China (Beijing) and China
(Ningxia) Regions do not support the FIPS 140-2 Cryptographic Module Validation Program. AWS KMS uses OSCCA certified
HSMs to protect KMS keys in China Regions.

AWS KMS integrates with most other AWS services that encrypt your data. AWS KMS also integrates with AWS CloudTrail to
log use of your KMS keys for auditing, regulatory, and compliance needs.

You can use the AWS KMS API to create and manage KMS keys and special features, such as custom key stores, and use KMS
keys in cryptographic operations. For detailed information, see the AWS Key Management Service API Reference.

See [AwsMock KMS supported commands](docs/man/awsmockkms.1.md) for a list of supported commands.

## Secrets Manager

AWS Secrets Manager helps you manage, retrieve, and rotate database credentials, application credentials, OAuth tokens,
API keys, and other secrets throughout their lifecycles. Many AWS services store and use secrets in Secrets Manager.

Secrets Manager helps you improve your security posture, because you no longer need hard-coded credentials in
application
source code. Storing the credentials in Secrets Manager helps avoid possible compromise by anyone who can inspect your
application or the components. You replace hard-coded credentials with a runtime call to the Secrets Manager service to
retrieve credentials dynamically when you need them.

With Secrets Manager, you can configure an automatic rotation schedule for your secrets. This enables you to replace
long-term secrets with short-term ones, significantly reducing the risk of compromise. Since the credentials are no
longer stored with the application, rotating credentials no longer requires updating your applications and deploying
changes to application clients.

See [AwsMock SecretsManager supported commands](docs/man/awsmocksecretsmanager.1.md) for a list of supported commands.

## Systems Manager

AWS Systems Manager is the operations hub for your AWS applications and resources and a secure end-to-end management
solution for hybrid and multicloud environments that enables secure operations at scale. Currently only the parameter
store is implemented.

Parameter Store provides secure, hierarchical storage for configuration data and secrets management. You can store
data such as passwords, database strings, Amazon Elastic Compute Cloud (Amazon EC2) instance IDs and Amazon Machine
Image (AMI) IDs, and license codes as parameter values. You can store values as plain text or encrypted data. You can
then reference values by using the unique name you specified when you created the parameter.

See [AwsMock SystemsManager supported commands](docs/man/awsmocksecretsmanager.1.md) for a list of supported commands.

## Getting started

### Building AwsMock

Building of the AwsMock executables is CMake based. Supported platforms are Linux, MacOS und Windows. Windows has some
operating system based limitations.

### Building on Linux

#### Minimum Requirements:

- GNU Compiler Collection (GCC) 4.9 or later or Clang 3.3 or later (Linux)
- jwt-cpp, prometheus-cpp, boost
- MongoDB C and CXX driver (see [MongoDB C++ driver](https://www.mongodb.com/docs/drivers/cxx/))
- Development releases of boost, libz, libssl, libcrypto, libarchive, libmagic, libssh
- 4GB of RAM.

#### Building the manager from source:

Building AwsMock from scratch is a rather time-consuming procedure. For a first glance at AwsMock, better use the
provided docker image. Nevertheless, if you need to compile it by your own, here are the instructions.

To create an out-of-source build:

1. Install CMake and the relevant build tools for your platform. Ensure these are available in your executable path.
2. Download the source tree
   ```
   git clone https://github.com/jensvogt/awsmock.git
   cd awsmock
   ```
3. Build the project:
   ```
   cmake . -DCMAKE_BUILD_TYPE=<build-type> -DCMAKE_PREFIX_PATH=<path-to-install>
   cmake --build . --config=<build-type> -j<parallel-threads>
   ```
4. Install the executables, libraries, and man pages:
   ```
   cmake --install . --config=Debug
   ```
5. Start the manager:
   ```
   awsmockmgr --deamon --loglevel debug
   ```

As already said, this can be a time-consuming procedure, depending on your machine and the environment.

#### Building the frontend from source:

IN order to build the frontend, you need Node.js >16.0. The sourcecode is located at
```http://github.com/jensvogt/awsmock-ui```. To build the frontend part, use:

```
npm run build --prod
```

and copy the result to ```$HOME/awsmock/frontend```.

### Using the docker image

Using the provided docker image is much simpler (assuming Docker is already installed).

To start the docker image:

1. Pull the docker image:

  ```
  docker pull jensvogt/awsmock:latest
  ```

2. Start the container

  ```  
  docker run -p 4566-4567:4566-4567 -p 21:21 -p 6000-6100:6000:6100 -e AWSMOCK_MONGODB_ACTIVE=false -v /var/run/docker.sock:/var/run/docker.sock jensvogt/awsmock:latest
  ```

This invocation will run with the in-memory database, as the alpine image does not have an own MongoDb instance. Port
```4566``` (gateway) and ```4567``` (frontend)
should be reachable. ```-e AWSMOCK_MONGODB_ACTIVE=false``` is needed to use the in-memory database and
```-v /var/run/docker.sock:/var/run/docker.sock``` for the communication with the host's docker daemon (lambdas,
dynamodb).
Ports ```6000-6100``` are use for the passive mode of the FTP server.

If you have problems with the docker daemon connection and you see errors like:

```
2024-06-01 12:28:46.116 ERROR [1] [Core::DomainSocket::SendJson:18] Could not connect to docker UNIX domain socket, error: Connection refused
```

you probably need to run it under root:

```
sudo docker run -p 4566-4567:4566-4567 -p 6000-6010:6000:6010 -e AWSMOCK_MONGODB_ACTIVE=false -v /var/run/docker.sock:/var/run/docker.sock jensvogt/awsmock:latest
```

or you can give the current user access to the docker domain socket:

```
sudo chmod 666 /var/run/docker.sock
```

### Using the docker image with a MongoDB backend

As MongoDB is not supported on alpine, no MongoDB is included in the docker image. Nevertheless, you can connect
to an external MongoDB instance running as a docker container. In ```/usr/local/share/awsmock/docker``` is an example of
a docker-compose file.

To connect a MongoDB instance, use the provided docker-compose file:

```
  cd /usr/local/share/awsmock/docker
  docker compose up
```

This will start a mongo DB instance an awsmock docker image. Remote access to the MongoDB image must be configured
separately. See for
instance: [Getting MongoDB on Linux to Listen to Remote Connections](https://www.baeldung.com/linux/mongodb-remote-connections).

### Running the Manager on the host

In order to run the manager on the host system (Docker ist still needed for the lambda functions and DynamoDB), you need
to make sure the manager application has access to the docker daemon REST API. For this, put your user id into the
docker
group;

```
sudo usermod -a -G docker <userName>
```

this will add ```<userName>``` to the docker group and the manager can access the docker daemon REST APIs. After that
you
can start the manager as normal foreground process:

```
/usr/local/bin/awsmockmgr --loglevel debug
```

It will use the configuration file: ```/etc/awsmock.yml```. Logging output will be written to the console. To stop the
process just type ```<CRTL>-C```. THe frontend will be served by default from the
```$HOME/awsmock/frontend``` directory.
To customize that, change the corresponding attribute in the configuration file.

## Examples

There are a couple of example shell scripts available. Depending on your installation directory, there in
```/usr/share/awsmock``` or in ```/usr/local/share/awsmock```. Each module has its own directory with bash scripts and
resource files. The examples cover almost all AwsMock features. All examples are pretty self-explaining.

Some of them require the jq utility (JSON queries). You can install jq using the usual package manager of your Linux
distribution. For Windows, jq can be downloaded from [jq download site](https://jqlang.github.io/jq/download/).

Simply change to the directory and execute one of the example scripts:

```
cd /usr/local/share/awsmock/s3
./create-bucket.sh
```

## Windows support

The distribution contains Windows ports of ```awslocal``` and ```awsmockctl``` in the ```./dist/bin``` directory. These
executables allow controlling awsmock docker image from a Windows machine.

```
c:\Program Files\awsmock\bin\awsmockctl status
```

### Windows native

AwsMock can be used natively on Windows. The base directory for the Windows port is usually
```C:\Program Files (x86)\awsmock```. Below the base directory are secondary directory like ```$AWS_ROOT\bin```, or
```$AWS_ROOT\log``` for the log files. Start the Windows executable by using:

```
"C:\Program Files (x86)\awsmock\bin\awsmockmgr.exe" --foreground
```

Log files will be written to ```$AWS_ROOT\log```. The frontend is available at ```http://localhost:4567```.

### Windows service

AwsMock can be installed as a Windows service. In order to install the AwsMock manager as a Windows service, use
(assuming the root directory of AwsMock is ```C:\Program Files (x86)\awsmock```) in a Windows CMD shell:

```
"C:\Program Files (x86)\awsmock\bin\awsmockmgr.exe" --install
```

### Compiling on Windows

AwsMock uses the MSVC C++ compiler (Microsoft Visual Studio 2019). In order to compile AwsMock on Windows install the
prerequisites:

```
prometheus-cpp
jwt-cpp
mongo-cxx-driver
libarchive
libmagic
openssl (v3)
boost (v1.88)
// Documentation (optional)
doxygen 
pandoc
graphviz
```

See the detailed installation instructions coming with the different packages.

## MacOS support

On macOS the manager can be run as a normal process. A macOS port can be installed using the normal development setup.
Start the manager as ```awsmockmgr```. The frontend is available at ```http://localhost:4567```

### Compiling on MacOS

In order to compile AwsMock on macOS, install the prerequisites (if not already existing):

```
brew install prometheus-cpp
brew install jwt-cpp
brew install mongo-cxx-driver
brew install libarchive
brew install libmagic
brew install openssl (v3)
brew install boost (v1.88)
// Documentation (optional)
brew install doxygen 
brew install pandoc
brew install graphviz
```

Download the backend source code

```

git clone https://github.com/jensvogt/awsmock
cd awsmock
cmake .
cmake --build . -j 16
sudo make install

```

This will compile and install the backend executables into ```/usr/local/awsmock/bin``` and the libraries in
```/usr/local/awsmock/lib```. The configuration file should be installed as ```/usr/local/awsmock/etc/awsmock.json```:
```cp dist/etc/awsmock.json /etc```

Download the frontend code

```
git clone https://github.com/jensvogt/awsmock-ui
cd awsmock-ui
npm install
npm run build --prod

```

## Configuration

To configure your infrastructure, you use the provided web interface on ```http://localhost:4567```.
This is an Angular application hosted by awsmock. In there you can import/export JSON files, which are simply dumps of
MongoDB collection. This way it is straightforward to load different infrastructure scenarios rapidly.

## Documentation

C++ documentation is available at [CodeDocs](https://codedocs.xyz/jensvogt/awsmock/)