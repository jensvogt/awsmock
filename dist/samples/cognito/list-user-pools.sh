#!/bin/bash

# List all available user pools
awslocal cognito-idp list-user-pools --max-result 10
