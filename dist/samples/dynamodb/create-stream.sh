#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint http://localhost:4566 --profile awsmock"

# Create a table
awslocal dynamodb create-table \
  --table-name MusicCollection \
  --attribute-definitions file://./resources/attributes.json \
  --key-schema file://./resources/key-schema.json \
  --provisioned-throughput file://./resources/capacity.json \
  --tags file://./resources/tags.json \
  --stream-specification file://./resources/stream-specification.json

# Put an item
awslocal dynamodb put-item \
  --table-name MusicCollection \
  --item file://./resources/put-item.json \
  --return-consumed-capacity TOTAL \
  --return-item-collection-metrics SIZE
