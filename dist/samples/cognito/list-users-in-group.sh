#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create group
awslocal cognito-idp create-group --user-pool-id "$userPoolId" --group-name test-group

# Create user in user pool
awslocal cognito-idp admin-create-user --user-pool-id "$userPoolId" --username test-user

# Add user to group
awslocal cognito-idp admin-add-user-to-group --user-pool-id "$userPoolId" --username test-user --group-name test-group

# List users in group
awslocal cognito-idp list-users-in-group --user-pool-id "$userPoolId" --group-name test-group
