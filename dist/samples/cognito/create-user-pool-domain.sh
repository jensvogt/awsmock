#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')

# Create a user-pool domain
awslocal cognito-idp create-user-pool-domain --domain test-domain --user-pool-id "$userPoolId"
