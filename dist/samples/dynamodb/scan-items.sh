#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create a table
awslocal dynamodb create-table \
  --table-name MusicCollection \
  --attribute-definitions file://./resources/attributes.json \
  --key-schema file://./resources/key-schema.json \
  --provisioned-throughput file://./resources/capacity.json \
  --tags file://./resources/tags.json \

# Put an item
awslocal dynamodb put-item \
  --table-name MusicCollection \
  --item file://./resources/put-item.json \
  --return-consumed-capacity TOTAL \
  --return-item-collection-metrics SIZE

# Scan items
awslocal dynamodb scan \
  --table-name MusicCollection
