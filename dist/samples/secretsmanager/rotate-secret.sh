#!/bin/bash

# Create a secret
awslocal secretsmanager create-secret --name test-secret --secret-string "{\"username\": \"root\", \"password\": \"password\", \"engine\": \"mongo\", \"host\": \"host.docker.internal\", \"port\": 27017, \"ssl\": false, \"dbname\": \"admin\"}"

# get the ARN
secretId=$(awslocal secretsmanager list-secrets | jq -r '.SecretList[] | select(.Name == "test-secret").ARN')

# Delete the test user
awslocal secretsmanager rotate-secret --secret-id $secretId --rotation-lambda-arn arn:aws:lambda:eu-central-1:000000000000:function:katalogdaten-db-ssm-lambda --rotate-immediately

