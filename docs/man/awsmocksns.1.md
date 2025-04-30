---
title: awsmocksns(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmocksns AwsMock SNS module
footer: awsmocksns $version$
---

## NAME

```awsmocksns``` AwsMock SNS module

## DESCRIPTION

Amazon Simple Notification Service (Amazon SNS) is a managed service that provides message delivery from publishers to
subscribers (also known as producers and consumers). Publishers communicate asynchronously with subscribers by sending
messages to a topic, which is a logical access point and communication channel. Clients can subscribe to the SNS topic
and receive published messages using a supported endpoint type, such as Amazon Kinesis Data Firehose, Amazon SQS, AWS
Lambda, HTTP, email, mobile push notifications, and mobile text messages (SMS).

Currently, only SQS is supported as an endpoint type.

The SQS module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal sns create-topic --name <topic-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new topic

```awslocal sns list-topics```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available topics

```awslocal sns subscribe --queue-url <queue-url>```  
&nbsp;&nbsp;&nbsp;&nbsp;subscribe a queue to a topic

```awslocal sns unsubscribe --subscrption-arn <subscript-arn>```  
&nbsp;&nbsp;&nbsp;&nbsp;subscribe a queue to a topic

```awslocal sqs publish --topic-arn <topic-arn> --message <message-body>```  
&nbsp;&nbsp;&nbsp;&nbsp;sends a message to a topic

```awslocal sqs tag-resource --resource-arn <topic-arn> --tags Key=<tagKey>,Value=<tagValue>```  
&nbsp;&nbsp;&nbsp;&nbsp;set resource tags to a topic

```awslocal sns delete-topic --topic-arn <topic-arn>```  
&nbsp;&nbsp;&nbsp;&nbsp;subscribe a queue to a topic

## EXAMPLES

To create a new topic:

```
awslocal sns create-topic --name test-topic
{
    "TopicArn": "arn:aws:sns:eu-central-1:000000000000:test-topic"
}
```

To get a list of all currently available topics:

```
awslocal sns list-topics
{
    "Topics": [
        {
            "TopicArn": "arn:aws:sns:eu-central-1:000000000000:test-topic"
        }
    ]
}
```

To subscribe a SQS Queue to an SNS topic:

```
awslocal sns subscribe --topic-arn arn:aws:sns:eu-central-1:000000000000:test-topic --protocol sqs --notification-endpoint arn:aws:sqs:eu-central-1:000000000000:test-queue
{
    "SubscriptionArn": "arn:aws:sns:eu-central-1:000000000000:test-topic:2f3cef01-3662-4e66-9180-a11e32e256db"
}
```

To unsubscribe a SQS Queue from a SNS topic:

```
awslocal sns unsubscribe --subscription-arn arn:aws:sns:eu-central-1:000000000000:test-topic:2f3cef01-3662-4e66-9180-a11e32e256db
```

To publish a message to a topic

```
awslocal sns publish --topic-arn arn:aws:sns:eu-central-1:000000000000:test-topic --message hello
{
    "MessageId": "fa040546-311d-45c9-871d-24fcde52bb39"
}
```

To tag a topic

```
awslocal sns tag-resource --resource-arn arn:aws:sns:eu-central-1:000000000000:test-topic --tags=Key=Team,Value=Alpha
```

To delete a topic:

```
awslocal sns delete-topic --topic-arn arn:aws:sns:eu-central-1:000000000000:test-topic
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/aws-mock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocklambda(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```,
```awsmockssm(1)```