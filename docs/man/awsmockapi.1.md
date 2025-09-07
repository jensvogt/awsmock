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

```awsmockapi``` AwsMock API Gateway module

## DESCRIPTION

Amazon API Gateway is an AWS service for creating, publishing, maintaining, monitoring, and securing REST, HTTP, and
WebSocket APIs at any scale. API developers can create APIs that access AWS or other web services, as well as data
stored in the AWS Cloud. As an API Gateway API developer, you can create APIs for use in your own client applications.
Or you can make your APIs available to third-party app developers.

## COMMANDS

```awslocal apigateway create-api-key --name test-key --description test-description```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new API key

```awslocal apigateway get-api-keys```  
&nbsp;&nbsp;&nbsp;&nbsp;shows a list of all available API keys```

```awslocal apigateway create-rest--api --name test-rest-api --description test-description```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new REST API```

## EXAMPLES

To create a new API key:

```
awslocal apigateway create-api-key --name test-key --description test-description
{
    "Version": 1
}
```

To get a list of all currently available parameters:

```
awslocal ssm describe-parameters
{
    "Version": 1
}
{
    "Parameters": [
        {
            "ARN": "arn:aws:ssm:eu-central-1:000000000000:",
            "Type": "String"
        },
        {
            "Name": "test-name",
            "ARN": "arn:aws:ssm:eu-central-1:000000000000:test-name",
            "Type": "String"
        }
    ]
}
```

To delete a parameter:

```
awslocal ssm delete-parameter --name test-name
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```