#!/bin/bash

# Create a single api key
apiKeyId=$(awslocal apigateway create-api-key --name test-key --description test-description --enabled | jq -r '.id')

# Create API key
awslocal apigateway get-api-keys --name test-key
