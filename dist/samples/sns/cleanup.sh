#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Delete topic
topicarn=$(awslocal sns create-topic --name test-topic | jq -r '.TopicArn')
awslocal sns delete-topic --topic-arn $topicarn

# Delete queue
queueurl=$(awslocal sqs create-queue --queue-name test-queue | jq -r '.QueueUrl')
awslocal sqs delete-queue --queue-url $queueurl

