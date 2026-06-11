#!/bin/bash

# Get the user pool id
keyId=$(awslocal apigateway get-api-keys | jq -r '.items[] | select(.name == "test-key").id')
apiId=$(awslocal apigateway get-rest-apis | jq -r '.items[] | select(.name == "test-rest-api").id')

# Delete the test SPI key
if [ -z "${keyId}" ]; then
  awslocal apigateway delete-api-key --api-key "${keyId}"
fi

if [ -z "${apiId}" ]; then
  awslocal apigateway delete-rest-api --api-key "${apiId}"
fi