#!/bin/bash

# Create bucket
awslocal s3 mb s3://test-bucket

# Create lambda function
awslocal lambda create-function \
  --function-name python-events \
  --runtime python3.8 \
  --timeout 120 \
  --memory-size 2024 \
  --tags tag=latest,version=latest \
  --environment file://./resources/python-lambda-env.json \
  --ephemeral-storage Size=2024 \
  --handler lambda_function.lambda_handler \
  --zip-file fileb://./resources/python-events.zip \
  --role arn:aws:iam::000000000000:role/lambda-role
