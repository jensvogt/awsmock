#!/bin/bash

# Get the user pool id
secretId=$(awslocal secretsmanager list-secrets | jq -r '.SecretList[] | select(.Name == "test-secret").ARN')

# Delete the test user pool
awslocal secretsmanager delete-secret --secret-id $secretId

