# Examples

This directory contains examples for the use of the AwsMock AWS simulation package. Most of the features of AwsMock are
covered in the different samples.

All example need a running awsmock manager listening on port 4566. If you prefer
another port edit the sample bash script accordingly. To set a port of 9000 change the line
```--endpoint http://localhost:4566``` to ```--endpoint http://localhost:9000```

The following AwsMock modules are covered:

- [S3](./s3/README.md): Amazon simple storage service
- [SQS](./sqs/README.md): Amazon simple queue service
- [SNS](./sns/README.md): Amazon simple notification service
- [Lambda](lambda/README.md): Amazon lambda service
- [DynamoDB](./dynamodb/README.md): Amazon Dynamo database service
- [Cognito](./cognito/README.md): Amazon Cognito Identity management
- [KMS](./kms/README.md): Amazon key management service
- [Transfer](./transfer/README.md): Amazon transfer family
- [SSM](./ssm/README.md): Amazon systems manager

if your AwsMock manager runs in a docker container, make sure the docker container is reachable from the host, and the
port can be pinged.

## Start an AwsMock as docker container

Start awsmock as a docker container using:

```
cd docker
docker-compose up -d
```

This will start the docker container as well as a mongodb instance. The configuration will be taken from
```docker/etc/awsmock.json```. If you need to customize the behaviour of AwsMock, make the necessary changes in this
configuration file.

## Start an example

To start one of the examples, you need to switch into the container and change the directory to the given example root
directory. Start the example with one of the supplied bash scripts:

```
docker exec -it <container-id> bash
cd /usr/local/awsmock/samples/s3
./create-bucket.sh
``` 