#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create temp files
tmpfile=$(mktemp -p /tmp).txt
base64 /dev/urandom | head -c 1k > $tmpfile
tmpfile=$(basename $tmpfile)

# Copy to S3 bucket
awslocal s3 cp /tmp/$tmpfile s3://test-bucket/$tmpfile

# List bucket content
awslocal s3api head-object --bucket test-bucket --key $tmpfile

# Cleanup
rm -f /tmp/$tmpfile


