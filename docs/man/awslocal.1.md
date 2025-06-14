---
title: awslocal(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awslocal AwsMock local command
footer: awslocal $version$
---

## NAME

```awslocal``` AwsMock local command.

## DESCRIPTION

```awslocal``` is a short bash script, which can be used to simplify the command line execution of AWS commands against
the AwsMock AWS simulation. It sets the endpoint and the profile to be used for the AwsMock command line commands.
Credentials are normally not used during the work with AwsMock. Nevertheless, you can give a AWS profile name in the
bash script in order to provide AWS login credentials.

```awslocal``` is per default installed in ```/usr/bin``` (if you
use the docker image, it will be in ```/usr/bin``` inside the docker image). If you need it locally, copy it from the
docker image.

## EXAMPLES

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

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awsmocksqs(1)```, ```awsmocks3(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```,
```awsmockssm(1)```