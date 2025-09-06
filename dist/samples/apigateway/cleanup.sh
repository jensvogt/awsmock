#!/bin/bash

# Get the user pool id
keyId=$(awslocal apigateway get-api-keys | jq -r '.items[] | select(.name == "test-key").id')

# Delete the test SPI key
awslocal apigateway delete-api-key --api-key ${keyId}

