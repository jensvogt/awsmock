#!/bin/bash

# Create a secret
awslocal secretsmanager create-secret --name test-secret --secret-string "{\"username\": \"documentdb\", \"password\": \"FooBar\", \"engine\": \"mongo\", \"host\": \"localhost\", \"port\": 27017, \"ssl\": false, \"dbClusterIdentifier\": \"test-db\"}"

# get the ARN
secretId=$(awslocal secretsmanager list-secrets | jq -r '.SecretList[] | select(.Name == "test-secret").ARN')

# Delete the test user
awslocal secretsmanager put-secret-value --secret-id $secretId --secret-string "{\"username\": \"root1\", \"password\": \"password1\", \"engine\": \"mongo\", \"host\": \"localhost\", \"port\": 27017, \"ssl\": false, \"dbname\": \"katalogdaten-db\"}"
