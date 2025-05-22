#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create user in user pool
awslocal cognito-idp admin-create-user --user-pool-id "$userPoolId" --username test-user
