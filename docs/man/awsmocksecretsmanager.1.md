---
title: awsmockdynamodb(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockdynamodb AwsMock DynamoDB module
footer: awsmockdynamodb $version$
---

## NAME

```awsmocksecretsmanager``` AwsMock Secrets Manager module

## DESCRIPTION

AWS Secrets Manager helps you manage, retrieve, and rotate database credentials, application credentials, OAuth tokens,
API keys, and other secrets throughout their lifecycles. Many AWS services store and use secrets in Secrets Manager.

Secrets Manager helps you improve your security posture, because you no longer need hard-coded credentials in
application
source code. Storing the credentials in Secrets Manager helps avoid possible compromise by anyone who can inspect your
application or the components. You replace hard-coded credentials with a runtime call to the Secrets Manager service
to retrieve credentials dynamically when you need them.

With Secrets Manager, you can configure an automatic rotation schedule for your secrets. This enables you to replace
long-term secrets with short-term ones, significantly reducing the risk of compromise. Since the credentials are no
longer stored with the application, rotating credentials no longer requires updating your applications and deploying
changes to application clients.

The SecretManager module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command
see the corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal secretsmanager create-secret <name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new secret.

```awslocal dynamodb get-secret-value --secret-id <secretId>```  
&nbsp;&nbsp;&nbsp;&nbsp;returns the decrypted secret value

```awslocal dynamodb put-secret-value --secret-id <secretId> --secret-string <secret-string>```  
&nbsp;&nbsp;&nbsp;&nbsp;put the secret string as new AWSCURRENT, the current secure-string will be set AWSPREVIOUS

```awslocal dynamodb list-secrets```  
&nbsp;&nbsp;&nbsp;&nbsp;lists the available secrets

```awslocal dynamodb rotate-secret --secrets-id <secrets-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;rotates the given secret

```awslocal dynamodb delete-secret --secret-id <secretId>```  
&nbsp;&nbsp;&nbsp;&nbsp;deletes a secret

## EXAMPLES

To create a new secret:

```
awslocal secretmanager create-secret --name test-table
{
    "ARN": "arn:aws:secretsmanager:eu-central-1:000000000000:secret:test-4f5h1d",
    "Name": "test",
    "VersionId": "c7ebfa48-1f4e-47af-830e-261d12b72e35"
}
```

To get the secret value:

```
awslocal secretmanager get-secret-value
{
    "ARN": "arn:aws:secretsmanager:eu-central-1:000000000000:secret:test-secret-0be9c0",
    "Name": "test-secret",
    "VersionId": "4b451a41-f82e-44cc-a242-baae6a4e7ab5"
}
```

To get a secret value:

```
awslocal secretmanager get-secret-value --secret-id <secret-id>
{
    "ARN": "arn:aws:secretsmanager:eu-central-1:000000000000:secret:test-secret-80d083",
    "Name": "test-secret",
    "VersionId": "a6c81243-86c2-4fb8-96c7-a6e407a6a0e0",
    "SecretBinary": "",
    "SecretString": "{\"username\": \"documentdb\", \"password\": \"FooBar\", \"engine\": \"mongo\", \"host\": \"localhost\", \"port\": 27017, \"ssl\": false}",
    "VersionStages": [AWSCURRENT],
    "CreatedDate": "2025-06-08T23:42:31+02:00"
}
```

To list all available secrets:

```
awslocal secretmanager list-secrets
{
    "SecretList": [
        {
            "ARN": "arn:aws:secretsmanager:eu-central-1:000000000000:secret:test-secret-9c654f",
            "Name": "test-secret",
            "Description": "",
            "KmsKeyId": "828ffaa2-a5f4-4876-98d3-959fe8bb0ad7",
            "RotationEnabled": false,
            "RotationLambdaARN": "",
            "RotationRules": {
                "AutomaticallyAfterDays": 126468734819144,
                "Duration": "",
                "ScheduleExpression": ""
            },
            "LastRotatedDate": "2025-03-02T01:00:00+01:00",
            "LastChangedDate": "2025-04-02T01:00:00+01:00",
            "LastAccessedDate": "2025-06-08T23:25:24+02:00",
            "DeletedDate": "2026-01-01T01:00:00+01:00",
            "OwningService": "",
            "CreatedDate": "2025-06-08T23:24:43+02:00",
            "PrimaryRegion": ""
        }
    ]
}
```

To delete a secret:

```
awslocal secretmanager delete-secret --secret-userPoolId test-4f5h1d
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```,
```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksqs(1)```,
```awsmockssm(1)```