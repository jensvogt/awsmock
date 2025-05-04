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

## Start an example

Start awsmock as a docker container using

```docker-compose up ./docker```
In order to start one of the examples, you need to change the directory to the given example root directory. Depending
on
your installation directory this will be either ```/usr/shares/awsmock/samples/s3```
or ```/usr/local shares/awsmock/samples/s3``` for the S3 samples.

Start the example with one of the supplied bash scripts:

```
/usr/shares/awsmock/samples/s3
./create-bucket.sh
``` 