#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Look for server ID
imageId=$(docker images | grep s3-notification-cpp-lambda | awk '{print $3}')
containerId=$(docker ps | grep s3-notification-cpp-lambda | awk '{print $1}')
echo "ImageId: $imageId ContainerId: $containerId"

# Delete server
awslocal lambda delete-function --function-name s3-notification-cpp-lambda

# Delete bucket
awslocal s3 rm s3://lambdas/s3-notification-cpp-lambda.zip
awslocal s3 rb s3://lambdas


