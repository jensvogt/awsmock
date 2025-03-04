---
title: awsmocklambda(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmocklambda AwsMock Lambda module
footer: awsmocklambda $version$
---

## NAME

```awsmocklambda``` AwsMock Lambda module

## DESCRIPTION

AWS Lambda is a Serverless Function as a Service (FaaS) platform that lets you run code in your preferred programming
language on the AWS ecosystem. AWS Lambda automatically scales your code to meet demand and handles server provisioning,
management, and maintenance. AWS Lambda allows you to break down your application into smaller, independent functions
that integrate seamlessly with AWS services.

AwsMock supports lambda, allowing you to use the Lambda API to create, deploy, and test your Lambda functions. The
supported APIs are available on our Lambda coverage page, which provides information on the extent of Lambda’s
integration with AwsMock.

Internally AwsMock will create and start a docker image of the function code using the AWS supplied RIE (Runtime
Interface Emulator) environment. The Lambda Runtime Interface Emulator is a proxy for Lambda’s Runtime and Extensions
APIs, which allows customers to locally test their Lambda function packaged as a container image. It is a lightweight
web-server that converts HTTP requests to JSON events and maintains functional parity with the Lambda Runtime API in
the cloud. It allows you to locally test your functions using familiar tools such as cURL and the Docker CLI (when
testing functions packaged as container images). It also simplifies running your application on additional computes.

You can include the Lambda Runtime Interface Emulator in your container image to have it accept HTTP requests instead
of the JSON events required for deployment to Lambda. This component does not emulate Lambda’s orchestrator, or security
and authentication configurations. You can get started by downloading and installing it on your local machine. When the
Lambda Runtime API emulator is executed, a ```/2015-03-31/functions/function/invocations``` endpoint will be stood up
within the container that you post data to it in order to invoke your function for testing.

Orchestration is done through different docker images. A maximal concurrency of 1000 is supported. Each invocation will
be sent to an idle instance of the lambda function. If no idle function docker container can be found, a new docker
container will be created, up to an upper limit of 1000 lambda function. Each instance of the lambda function docker
container will be cached for 60 min. The lifetime can be set in the awsmock configuration file (
```awsmock.service.lambda.lifetime```)
Default is 3600 seconds.

The lambda functions are executed inside the RIE (Runtime Interface Emulator). Lambdas run as docker container using
port 8080 as REST API port for invocation requests. The internal port 8080 are connected to the host via a port
forwarding, therefore the host port is randomly chosen between 32768 and 65536. You can see the docker host port via the
```docker ps``` command.

To see the output of the lambdas function (if its logs to standard output ), use the docker command:

```
shell> docker ps
CONTAINER ID   IMAGE                                COMMAND                  CREATED          STATUS          PORTS                                         NAMES
ecbb2e08c5f4   python-events:latest                 "/lambda-entrypoint.…"   47 minutes ago   Up 47 minutes   0.0.0.0:62044->8080/tcp, :::62044->8080/tcp   python-events
shell> docker logs -f ecbb2e08c5f4
```

This will show the logs online.

The Lambda module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## Supported runtimes

The following lambda runtimes are supported:

- java8: AWS Java 8 runtime
- java11: AWS Java 11 runtime
- java17: AWS Java 17 runtime
- java21: AWS Java 21 runtime
- python 3.8: AWS Python 3.8 runtime (if possible don't use, it will be deprecated soon)
- python 3.9: AWS Python 3.9 runtime
- python 3.10: AWS Python 3.10 runtime
- python 3.11: AWS Python 3.11 runtime
- python 3.12: AWS Python 3.12 runtime
- provided: Amazon Linux (for C++ lambdas)
- provided.al2023: Amazon Linux 2023 (for C++ lambdas)
- go1.x: AWS Go runtime
- nodejs.16: AWS Node.js 16 runtime (if possible don't use, it will be deprecated soon)
- nodejs.18: AWS Node.js 18 runtime
- nodejs.20: AWS Node.js 20 runtime

For a list of supported runtimes
see [Lambda runtimes](https://docs.aws.amazon.com/lambda/latest/dg/lambda-runtimes.html).
Windows runtimes (.Net6, .Net8, .Net 8) are not supported yet.

## COMMANDS

```awslocal lambda create-function --queue-name <queue-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new lambda function

```awslocal lambda list-functions```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available lambda functions

```awslocal lambda delete-function --function-name <function-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;delete a lambda function

## EXAMPLES

To create a new lambda function:

```
awslocal lambda create-function --function-name test-function --runtime Java17 --timeout 120 --memory-size 2024 \
    --environment "file://./function-env.json" --tags tag=latest,version=1.0 --ephemeral-storage Size=10240 \
    --handler org.springframework.cloud.function.adapter.aws.FunctionInvoker --region eu-central-1 
    --zip-file "fileb://file.zip" --role arn:aws:iam::000000000000:role/lambda-role \
{
    "FunctionName": "test-function",
    "FunctionArn": "arn:aws:lambda:eu-central-1:000000000000:function:test-function",
    "Runtime": "java17",
    "Role": "arn:aws:iam::000000000000:role/lambda-role",
    "Handler": "org.springframework.cloud.function.adapter.aws.FunctionInvoker",
    "CodeSize": 0,
    "Timeout": 3,
    "MemorySize": 128,
    "LastModified": "2023-12-20T11:14:19.074357Z",
    "CodeSha256": "",
    "Environment": {
        "Variables": {
            "AWS_ACCESS_KEY_ID": "none",
            "AWS_ACCOUNT_ID": "000000000000",
            "AWS_REGION": "eu-central-1",
            "AWS_SECRET_ACCESS_KEY": "none",
            "AWS_SESSION_TOKEN": "none",
        },
        "Error": {
            "ErrorCode": "",
            "Message": ""
        }
    },
    "EphemeralStorage": {}
}
```

To get a list of all currently available lambda functions:

```
awslocal lambda list-functions
{
    "FunctionName": "test-function",
    "FunctionArn": "arn:aws:lambda:eu-central-1:000000000000:function:test-function",
    "Runtime": "java17",
    "Role": "arn:aws:iam::000000000000:role/lambda-role",
    "Handler": "org.springframework.cloud.function.adapter.aws.FunctionInvoker",
    "CodeSize": 0,
    "Timeout": 3,
    "MemorySize": 128,
    "LastModified": "2023-12-20T11:14:19.074357Z",
    "CodeSha256": "",
    "Environment": {
        "Variables": {
            "AWS_ACCESS_KEY_ID": "none",
            "AWS_ACCOUNT_ID": "000000000000",
            "AWS_REGION": "eu-central-1",
            "AWS_SECRET_ACCESS_KEY": "none",
            "AWS_SESSION_TOKEN": "none",
        },
        "Error": {
            "ErrorCode": "",
            "Message": ""
        }
    },
    "EphemeralStorage": {}
}
```

To get a single lambda function:

```
awslocal lambda get-lambda --function-name test-function
{
    "FunctionName": "test-function",
    "FunctionArn": "arn:aws:lambda:eu-central-1:000000000000:function:test-function",
    "Runtime": "java17",
    "Role": "arn:aws:iam::000000000000:role/lambda-role",
    "Handler": "org.springframework.cloud.function.adapter.aws.FunctionInvoker",
    "CodeSize": 0,
    "Timeout": 3,
    "MemorySize": 128,
    "LastModified": "2023-12-20T11:14:19.074357Z",
    "CodeSha256": "",
    "Environment": {
        "Variables": {
            "AWS_ACCESS_KEY_ID": "none",
            "AWS_ACCOUNT_ID": "000000000000",
            "AWS_REGION": "eu-central-1",
            "AWS_SECRET_ACCESS_KEY": "none",
            "AWS_SESSION_TOKEN": "none",
        },
        "Error": {
            "ErrorCode": "",
            "Message": ""
        }
    },
    "EphemeralStorage": {}
}
```

To invoke a lambda:

```
aws lambda invoke \
  --function-name python-events \
  --payload fileb://./resources/event.json \
  --log-type Tail \
  --region eu-central-1 \
  --endpoint http://localhost:4566 \
  --profile awsmock \
  out.json
```

See the sample directory for more details on how to invoke a lambda function.

To delete a lambda function:

```
awslocal lambda delete-function --function-name test-function
```

This will stop the docker container and remove the image from the docker environment.

## FRONTEND

The frontend has some more functionalities, which are not included in the AWS CLI. Using the frontend on
```http://localhost:4567```, you can start/stop the lambda function as well as monitor the number of invocation and the
average runtime in milliseconds. To update a lambda function (replacement of the lambda function code), use the update
function in the lambda function list.

## SAMPLES

More example can be found in the AwsMock sample directory (```/usr/local/share/awsmock```).

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/aws-mock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```,
```awsmockssm(1)```