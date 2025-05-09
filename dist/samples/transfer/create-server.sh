#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create transfer server
serverId=$(awslocal transfer create-server --protocols ftp | jq -r '.ServerId')
echo "ServerID: $serverId"

# STart transfer server
awslocal transfer start-server --server-id $serverId
