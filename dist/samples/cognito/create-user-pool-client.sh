#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create user pool client
awslocal cognito-idp create-user-pool-client --user-pool-id "$userPoolId" --client-name test-client --generate-secret
