#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create user pool client
awslocal cognito-idp create-user-pool-client --user-pool-id "$userPoolId" --client-name test-client --generate-secret

# List user pool clients
clientId=$(awslocal cognito-idp list-user-pool-clients --user-pool-id "$userPoolId" | jq -r '.UserPoolClients[0].ClientId')

# Describe user pool client
awslocal cognito-idp describe-user-pool-client --user-pool-id "$userPoolId" --client-id "$clientId"

# Delete the user-pool
awslocal cognito-idp delete-user-pool --user-pool-id "$userPoolId"

# List all available
awslocal cognito-idp list-user-pools --max-results 10
