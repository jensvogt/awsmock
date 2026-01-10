#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Delete objects
awslocal s3 rm s3://test-bucket --recursive

# Delete bucket
awslocal s3 rb s3://test-bucket --force


