#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create bucket
awslocal s3 mb s3://lambdas

# Copy image to bucket
awslocal s3 cp ./resources/s3-notification-cpp-lambda.zip s3://lambdas/s3-notification-cpp-lambda.zip

# Create lambda function
awslocal lambda create-function \
  --function-name s3-notification-cpp-lambda \
  --runtime provided.al2023 \
  --timeout 120 \
  --memory-size 128 \
  --tags tag=latest,version=latest \
  --ephemeral-storage Size=2024 \
  --handler s3-notification-lambda \
  --region eu-central-1 \
  --code file://./resources/code.json \
  --role arn:aws:iam::000000000000:role/lambda-role \
  --cli-read-timeout 0