#!/bin/bash

# Create a secret
awslocal secretsmanager create-secret --name test-secret1 --secret-string "{\"username\": \"root\", \"password\": \"password\", \"engine\": \"mongo\", \"host\": \"localhost\", \"port\": 27017, \"ssl\": false, \"dbname\": \"katalogdaten-db\"}"

# Delete the test user
awslocal secretsmanager list-secrets
