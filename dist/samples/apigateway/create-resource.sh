#!/bin/bash

# Create REST API
restApiId=$(awslocal apigateway create-rest-api --name test-key --description test-description | jq -r '.id')

# Get a list of
awslocal apigateway create-resource --rest-api-id "${restApiId}" --parent-id "/" --path-part test-resource
