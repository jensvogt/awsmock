---
title: awsmocklkms(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmocklkms AwsMock KMS module
footer: awsmocklkms $version$
---

## NAME

```awsmockkms``` AwsMock KMS module

## DESCRIPTION

AWS Key Management Service (AWS KMS) is a managed service that makes it easy for you to create and control the
cryptographic keys that are used to protect your data. AWS KMS uses hardware security modules (HSM) to protect and
validate your AWS KMS keys under the FIPS 140-2 Cryptographic Module Validation Program.

AWS KMS integrates with most other AWS services that encrypt your data. AWS KMS also integrates with AWS CloudTrail to
log use of your KMS keys for auditing, regulatory, and compliance needs.

You can use the AWS KMS API to create and manage KMS keys and special features, such as custom key stores, and use KMS
keys in cryptographic operations. For detailed information, see the AWS Key Management Service API Reference.

The KMS module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal kms create-key```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new symmetric key (AES256).

```awslocal kms list-keys```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available keys

```awslocal kms describe-key --key-id <key-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;describe the key with the given id

## EXAMPLES

To create a new symmetric key:

```
awslocal kms create-key
{
    "KeyMetadata": {
        "KeyId": "e979f096-732e-4063-9b67-8e38dee44c7b",
        "CreationDate": "1970-01-01T01:00:00+01:00",
        "Enabled": false,
        "Description": "",
        "KeyUsage": "ENCRYPT_DECRYPT",
        "KeyState": "Unavailable",
        "DeletionDate": "1970-01-01T01:00:00+01:00",
        "Origin": "AWS_KMS",
        "KeySpec": "SYMMETRIC_DEFAULT",
        "EncryptionAlgorithms": [],
        "MultiRegion": false
    }
}
```

To get a list of all currently available keys:

```
awslocal kms list-keys
{
    "Keys": [
        {
            "KeyId": "2a5880df-0274-44b3-af36-93b8ee819265",
            "KeyArn": "arn:aws:kms:eu-central-1:000000000000:key/2a5880df-0274-44b3-af36-93b8ee819265"
        },
        {
            "KeyId": "f0914bb8-e1dc-4225-a385-2247b052be85",
            "KeyArn": "arn:aws:kms:eu-central-1:000000000000:key/f0914bb8-e1dc-4225-a385-2247b052be85"
        },
        {
            "KeyId": "f1a2e4e6-f068-42b0-b4ec-b5fb8b7549e5",
            "KeyArn": "arn:aws:kms:eu-central-1:000000000000:key/f1a2e4e6-f068-42b0-b4ec-b5fb8b7549e5"
        },
        {
            "KeyId": "e979f096-732e-4063-9b67-8e38dee44c7b",
            "KeyArn": "arn:aws:kms:eu-central-1:000000000000:key/e979f096-732e-4063-9b67-8e38dee44c7b"
        }
    ]
}

```

To describe a KMS key:

```
awslocal kms describe-key --key-id e979f096-732e-4063-9b67-8e38dee44c7b
{
    "KeyMetadata": {
        "KeyId": "e979f096-732e-4063-9b67-8e38dee44c7b",
        "CreationDate": "2024-04-27T23:31:07+02:00",
        "Enabled": true,
        "Description": "",
        "KeyUsage": "ENCRYPT_DECRYPT",
        "KeyState": "Enabled",
        "DeletionDate": "2024-04-27T23:31:07+02:00",
        "Origin": "AWS_KMS",
        "KeySpec": "SYMMETRIC_DEFAULT",
        "EncryptionAlgorithms": [],
        "MultiRegion": false
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

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksecretsmanager(1)```, ```awsmockssm(1)```, ```awsmockapi(1)```