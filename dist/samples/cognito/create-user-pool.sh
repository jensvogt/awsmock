#!/bin/bash

# Create a user-pool
awslocal cognito-idp create-user-pool --pool-name test-user-pool

# List all available
awslocal cognito-idp list-user-pools --max-results 10
