#!/bin/bash

# Create a single user-pool
awslocal cognito-idp create-user-pool --pool-name test-user-pool

# List all available
userPoolId=$(awslocal cognito-idp list-user-pools --max-results 10 | jq -r '.UserPools[0].Id')

# Delete the user-pool
awslocal cognito-idp delete-user-pool --user-pool-id "$userPoolId"

# List all available
awslocal cognito-idp list-user-pools --max-results 10
