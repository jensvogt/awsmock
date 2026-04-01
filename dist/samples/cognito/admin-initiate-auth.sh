#!/bin/bash

# Create a user-pool
userPoolId=$(awslocal cognito-idp create-user-pool --pool-name test-user-pool | jq -r '.UserPool.Id')
echo $userPoolId

# Create user in user pool
awslocal cognito-idp admin-create-user --user-pool-id "$userPoolId" --username test-user@email.com

# Create user pool client
clientId=$(awslocal cognito-idp create-user-pool-client --user-pool-id "$userPoolId" --client-name test-client --generate-secret | jq -r '.UserPoolClient.ClientId')
echo $clientId

awslocal cognito-idp initiate-auth --auth-flow USER_PASSWORD_AUTH --client-id $clientId --auth-parameters USERNAME=test-user@email.com,PASSWORD=yourpassword
