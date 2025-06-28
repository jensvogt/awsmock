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

```awsmockssm``` AwsMock SSM module

## DESCRIPTION

AWS Systems Manager unterstützt Sie bei der zentralen Anzeige, Verwaltung und dem Betrieb skalierbarer Knoten in lokalen
und AWS Multicloud-Umgebungen. Mit der
Einführung einer einheitlichen Konsolenoberfläche konsolidiert Systems Manager verschiedene Tools, mit denen Sie
allgemeine Knotenaufgaben in allen AWS-Konten Regionen
erledigen können.

The SSM module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal ssm put-parameter --name test-name```  
&nbsp;&nbsp;&nbsp;&nbsp;adds a new parameter

```awslocal ssm get-parameter --name test-name```  
&nbsp;&nbsp;&nbsp;&nbsp;returns a parameter by name

```awslocal ssm delete-parameter --name test-name```  
&nbsp;&nbsp;&nbsp;&nbsp;deletes a parameter by name

```awslocal ssm describe-parameters```  
&nbsp;&nbsp;&nbsp;&nbsp;returns information about all parameters

## EXAMPLES

To create a new parameter:

```
awslocal ssm put-parameter --name test-name
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