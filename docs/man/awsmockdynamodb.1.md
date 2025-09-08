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

```awsmockdynamodb``` AwsMock DynamoDB module

## DESCRIPTION

DynamoDB is a fully managed NoSQL database service provided by AWS. It offers a flexible and highly scalable way to
store and retrieve data, making it suitable for a wide range of applications. DynamoDB provides a fast and scalable
key-value datastore with support for replication, automatic scaling, data encryption at rest, and on-demand backup,
among other capabilities.

AwsMock supports DynamoDB, allowing you to use the DynamoDB APIs in your local environment to manage key-value and
document data models. The supported APIs are available on our API coverage page, which provides information on the
extent of DynamoDB’s integration with AwsMock.

The AwsMock DynamoDB module uses the AWS DynamoDB docker image as backing store. For mor information about the AWS
DynamodDB docker image see
https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/DynamoDBLocal.DownloadingAndRunning.html#docker

The DynamoDB module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see
the corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal dynamodb create-table <tableName>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new DynamoDB table.

```awslocal dynamodb list-tables```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available DynamoDB tables

```awslocal dynamodb delete-table --table-name <table-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;delete a DynamoDb table

```awslocal dynamodb put-item --table-name <table-name> ...```  
&nbsp;&nbsp;&nbsp;&nbsp;puts an item into the table

```awslocal dynamodb get-item --table-name <table-name> ...```  
&nbsp;&nbsp;&nbsp;&nbsp;retrieves an item from the table

```awslocal dynamodb query --table-name <table-name> ...```  
&nbsp;&nbsp;&nbsp;&nbsp;query the database by primary keys

```awslocal dynamodb delete-item --table-name <table-name> ...```  
&nbsp;&nbsp;&nbsp;&nbsp;deletes an item from the table

## EXAMPLES

To create a new table:

```
awslocal dynamodb create-table --table-name test-table --attribute-definitions attributeName=orgaNr,AttributeType=N \
                               --key-schema attributeName=orgaNr,KeyType=HASH \
                               --provisioned-throughput ReadCapacityUnits=1,WriteCapacityUnits=1
{
    "TableDescription": {
        "AttributeDefinitions": [
            {
                "attributeName": "orgaNr",
                "AttributeType": "N"
            }
        ],
        "TableName": "test-table",
        "KeySchema": [
            {
                "attributeName": "orgaNr",
                "KeyType": "HASH"
            }
        ],
        "TableStatus": "ACTIVE",
        "CreationDateTime": "2023-12-21T17:35:58.508000+01:00",
        "ProvisionedThroughput": {
            "LastIncreaseDateTime": "1970-01-01T01:00:00+01:00",
            "LastDecreaseDateTime": "1970-01-01T01:00:00+01:00",
            "NumberOfDecreasesToday": 0,
            "ReadCapacityUnits": 1,
            "WriteCapacityUnits": 1
        },
        "TableSizeBytes": 0,
        "ItemCount": 0,
        "TableArn": "arn:aws:dynamodb:ddblocal:000000000000:table/test-table",
        "DeletionProtectionEnabled": false
    }
}
```

To get a list of all currently available tables:

```
awslocal dynamodb list-tables
{
    "TableNames": [
        "test-table"
    ]
}
```

To delete a lambda function:

```
awslocal lambda delete-function --function-name test-function
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```, ```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```,
```awsmocktransfer(1)```,```awsmocksqs(1)```, ```awsmocksecretsmanager(1)```, ```awsmockssm(1)```, ```awsmockapi(1)```