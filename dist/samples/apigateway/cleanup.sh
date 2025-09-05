#!/bin/bash

# Get the user pool id
userPoolId=$(awslocal apigateway list-keys 10 | jq -r '.UserPools[] | select(.Name == "test-key").Id')

# Delete the test user
awslocal cognito-idp admin-delete-user --user-pool-id "$userPoolId" --username test-user

# Delete the test group
awslocal cognito-idp delete-group --user-pool-id "$userPoolId" --group-name test-group

# Delete the test user pool
awslocal cognito-idp delete-user-pool --user-pool-id "$userPoolId"

