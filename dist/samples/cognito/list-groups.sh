#!/bin/bash

# Create a user pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create group
awslocal cognito-idp create-group --user-pool-id "$userPoolId" --group-name test-group

# List all available groups
awslocal cognito-idp list-groups --user-pool-id "$userPoolId"
