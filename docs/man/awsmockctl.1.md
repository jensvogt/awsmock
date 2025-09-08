---
title: awsmockctl(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockctl AwsMock Controller
footer: awsmockctl $version$
---

## NAME

```awsmockctl``` AwsMock controller

## SYNOPSIS

awsmockctl [OPTIONS] [COMMAND]

## DESCRIPTION

awsmockctl controls and shows some insides of the AwsMock AWS cloud simulation. Supported AWS modules are: SQS, SNS
S3, Cognito, TransferServer, Lambda. AwsMock is written in C++ and has a module structure, using different modules for
each AWS service plus a API gateway, which routes the requests to the different modules.

Per default, the gateway runs on port 4566 but can be changed in the configuration file. The default configuration file
```/etc/aws-mock.properties```, which can be changed on the command using the ```--config <config-file-path>``` option.

awsmockctl provides a series of commands which control the different modules. It allows to start/stop different modules
as well as showing the current configuration.

## OPTIONS

```--include-objects```:  
adds objects to the export. By default, only the infrastructure elements are export (SQS queues,
SNS topic, S3 buckets, etc.). Settings this option will export also all object (S3 objects, SQS messages, SNS messages,
etc.). Exporting also the objects will result in a rather huge output file, depending on your objects in the different
infrastructure elements.

```--pretty```:  
indent the resulting JSON string to make it more readable. Will increase the output size.

```--config <config-file-path>```:  
starts the controller with a given configuration file. Per default the configuration
file ```/etc/aws-mock.properties``` will
be used. The path should be the absolute path to the configuration file.

```--host <host>```:  
sets the name of the manager host.

```--port <port>```:  
sets the port for the awsmock manager application.

```--help```    
shows the usage screen and exists

```--version```    
shows the current version and exists.

## COMMANDS

```list [applications|lambdas|<application...>|<lambdas...>]```  
lists the currently running AwsMock applications, lambdas, or both, the provided format is:

```
Applications:
  application1           ENABLED RUNNING
  application2           DISABLED STOPPED
  ...
Lambdas:
  lambda1                ENABLED ACTIVE
  lambda2                DISABLED INACTIVE
  ...
```

Without arguments, all applications and lambdas will be listed. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to list only these elements. Separate list elements by spaces.

```enable [applications|lambdas|<application...>|<lambdas...>]```  
enables the given applications, lambdas, or both. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to enable only these elements. Separate list elements by spaces.

```disable [applications|lambdas|<application...>|<lambdas...>]```  
disabled the given applications, lambdas, or both. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to disable only these elements. Separate list elements by spaces

```start [applications|lambdas|<application...>|<lambdas...>]```  
starts the given applications, lambdas, or both. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to start only these elements. Separate list elements by spaces

```stop [applications|lambdas|<application...>|<lambdas...>]```  
stops the given applications, lambdas, or both. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to stop only these elements. Separate list elements by spaces

```restart [applications|lambdas|<application...>|<lambdas...>]```  
restarts the given applications, lambdas, or both. With the ```applications``` argument only the
applications will be listed. With the ```lambdas``` argument only the lambdas will be listed. Additionally, you can
provide a list of applications and/or lambdas to restart only these elements. Separate list elements by spaces

```logs```  
shows the console logs of the AwsMock manager application ```awsmockmgr```.
See ```awsmockmgr(1)``` for details of the manager.

```loglevel```  
sets the log level of the AwsMock manager ```awsmockmgr```. ```<loglevel>``` it can be one of: verbose, debug, info,
warning, error, fatal, or none.

```config```  
shows the configuration of the AwsMock API gateway.

```export [modules] [--include-objects] [--pretty]```  
export the current infrastructure to stdout in JSON format. ```modules``` can be space separated list of AwsMock
modules. If no modules are given or the module list contains ```all```, all modules will be exported. IF oyu include
the ```--include-objects``` also all object will be exported (SQS messages, SNS messages, S3 objects, etc.)

```import```  
import the infrastructure from stdin in JSON format.

```clean```  
cleans the infrastructure. This means all SQS queues plus messages, SNS topics plus messages, S3 buckets with all
objects, transfer servers, and lambda functions will be deleted. ALl modules will be still running, but all AwsMock
objects will be emptied.

## EXAMPLES

To show the current running applications:

```
$$root:>> awsmockctl list
Applications:
  application1         ENABLED RUNNING
  application2         DISABLED STOPPED
Lambdas:
  lambda1              ENABLED ACTIVE
  lambda2              DISABLED INACTIVE
```

Stop all applications:

```
$$root:> awsmockctl stop applications
All applications stopped
$$root:> awsmockctl list
application1         ENABLED STOPPED
application2         DISABLED STOPPED
```

Start all applications:

```
$$root:> awsmockctl start
All applications started
$$root:> awsmockctl list
application1         ENABLED RUNNING
application2         ENABLED RUNNING
```

Stop some applications:

```
$$root:> awsmockctl stop application1 application2
Application application1 stopped
Application application2 stopped
$$root:> awsmockctl list
application1         ENABLED STOPPED
application2         DISABLED STOPPED
```

Stop some lambdas:

```
$$root:> awsmockctl stop lambda1 lambda2
Lambda lambda1 stopped
Lambda lambda2 stopped
$$root:> awsmockctl list
application1         ENABLED STOPPED
application2         DISABLED STOPPED
```

Show the manager logs

```
$$root:> awsmockctl logs
08-11-2023 20:02:58.800 [I] 125 S3ServiceHandler:57 - Requested multipart download range: 9667870720-9673113599
10-11-2023 13:31:57.053 [T] 55 AbstractHandler:559 - Getting header values, name: RequestId
10-11-2023 14:19:51.902 [T] 17 SNSServer:94 - Queue updated, nametextannotation-result-queue
10-11-2023 15:11:42.215 [T] 17 SQSDatabase:266 - Message reset, visibility: 30 updated: 0 queue: http://localhost:4566/000000000000/ftp-file-distribution-image-dlqueue
10-11-2023 16:05:42.347 [T] 17 SQSDatabase:331 - Count messages, region: eu-central-1 url: http://localhost:4566/000000000000/ftp-file-distribution-vlb-preisreferenz-queue result: 0
10-11-2023 16:56:51.429 [T] 17 SNSServer:94 - Queue updated, nametextannotation-result-queue
```

Export the S3 and SQS infrastructure:

```
$$root:> awsmockctl export s3 sqs
{
    "infrastructure": {
      "s3-buckets": [
            {
                "name": "transfer-server",
                "notifications": [
                    {
                        "event": "s3:ObjectCreated:Put",
                        "lambdaArn": "arn:aws:lambda:eu-central-1:000000000000:function:lambda1",
                        "notificationId": "1234567890123",
                        "queueArn": ""
                    },
                    ....
```

Export the S3 and SQS infrastructure to a file ```infrastructure.json```:

```
$$root:> awsmockctl export s3 sqs > infrastructure.json
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockmgr(1)```, ```awsmocksqs(1)```, ```awslocal(1)```, ```awsmocksqs(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksecretsmanager(1)```, ```awsmocksqs(1)```, ```awsmockssm(1)```, ```awsmockapi(1)```