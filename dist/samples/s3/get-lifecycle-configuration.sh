#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create s3 bucket
awslocal s3 mb s3://test-bucket

# Put lifecycle configuration
awslocal s3api put-bucket-lifecycle-configuration --bucket test-bucket \
  --lifecycle-configuration file://./resources/lifecycle-configuration.json

# Get the bucket lifecycle configuration
awslocal s3api get-bucket-lifecycle-configuration --bucket test-bucket
