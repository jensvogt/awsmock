---
title: awsmocksqs(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmocksqs AwsMock SQS module
footer: awsmocksqs $version$
---

## NAME

```awsmocksqs``` AwsMock SQS module

## DESCRIPTION

Simple Queue Service (SQS) is a managed messaging service offered by AWS. It allows you to decouple different components
of your applications by enabling asynchronous communication through message queues. SQS allows you to reliably send,
store,
and receive messages with support for standard and FIFO queues.

AwsMock allows you to use the SQS APIs in your local environment to integrate and decouple distributed systems via
hosted queues. The supported APIs are available on our API coverage page, which provides information on the extent of
SQS’s integration with AwsMock.

The SQS module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal sqs list-queues```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available queues

```awslocal sqs get-queue-attributes --queue-url <queue-url>```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all attributes of a given queue

```awslocal sqs send-message --queue-url <queue-url> --message-body <message-body>```  
&nbsp;&nbsp;&nbsp;&nbsp;sends a message to a queue

```awslocal sqs receive-message --queue-url <queue-url>```  
&nbsp;&nbsp;&nbsp;&nbsp;receives a message from a queue

```awslocal sqs purge-queue --queue-url <queue-url>```    
&nbsp;&nbsp;&nbsp;&nbsp;purge a queue

```awslocal sqs get-queue-attributes --queue-url <queue-url> --attribute-names <attribute-names>```    
&nbsp;&nbsp;&nbsp;&nbsp;shows the named queue attributes

```awslocal sqs set-queue-attributes --queue-url <queue-url> --attributes <attributes>```    
&nbsp;&nbsp;&nbsp;&nbsp;sets the named queue attributes

```awslocal sqs tag-queue --queue-url <queue-url> --tags <Key=Value>```    
&nbsp;&nbsp;&nbsp;&nbsp;tags the queue

```awslocal sqs delete-queue --queue-url <queue-url>```    
&nbsp;&nbsp;&nbsp;&nbsp;deletes the queue

## EXAMPLES

To create a new queue:

```
awslocal sqs create-queue --queue-name test-queue
{
    "QueueUrl": "http://localhost:4566/000000000000/test-queue"
}
```

To get a list of all currently available queues:

```
awslocal sqs list-queues
{
    "QueueUrls": [
        "http://localhost:4566/000000000000/test-dlqueue",
        "http://localhost:4566/000000000000/test-queue-queue",
        "http://localhost:4566/000000000000/test-result-dlqueue",
        "http://localhost:4566/000000000000/test-result-queue",
        ...
}
```

To get a list of all SQS queue attributes:

```
awslocal sqs get-queue-attributes --queue-url http://localhost:4566/000000000000/test-queue --attribute-name All
{
    "Attributes": {
        "ApproximateNumberOfMessages": "0",
        "ApproximateNumberOfMessagesDelayed": "0",
        "ApproximateNumberOfMessagesNotVisible": "0",
        "CreatedTimestamp": "1702377771",
        "DelaySeconds": "0",
        "LastModifiedTimestamp": "1702377771",
        "MaximumMessageSize": "262144",
        "MessageRetentionPeriod": "345600",
        "Policy": "",
        "QueueArn": "arn:aws:sqs:eu-central-1:000000000000:test-queue",
        "ReceiveMessageWaitTimeSeconds": "20",
        "VisibilityTimeout": "30"
    }
}
```

To send a message to a queue:

```
awslocal sqs send-message --queue-url http://localhost:4566/000000000000/test-queue --message-body file://message.json
```

To receive a message from a queue:

```
awslocal sqs receive-message --queue-url http://localhost:4566/000000000000/test-queue
{
    "Messages": [
        {
            "MessageId": "2ef14ec8-aec3-42b9-af71-b9861c431f81",
            "ReceiptHandle": "wXJklVJTXrT0ulgVHLN8baHrE1yGBClZbpee6Wg1T.....
            "MD5OfBody": "beaa0032306f083e847cbf86a09ba9b2",
            "Body": "test-message",
            "MD5OfMessageAttributes": "d41d8cd98f00b204e9800998ecf8427e"
        }
    ]
}
```

To change the visibility of a message:

```
awslocal sqs change-message-visibility --queue-url http://localhost:4566/000000000000/test-queue-queue --receipt-handle file://receipt-handle --visibility-timeout 120
```

To tag a queue (Key=Key1, Value=Value1):

```
awslocal sqs tag-queue --queue-url http://localhost:4566/000000000000/test-queue-queue --tags Key1=Value1
```

To delete a queue:

```
awslocal sqs delete-queue --queue-url "http://localhost:4566/000000000000/test-queue-queue
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksecretsmanager(1)```, ```awsmocksqs(1)```, ```awsmockssm(1)```, ```awsmockapi(1)```