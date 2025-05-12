#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint --endpoint http://localhost:4566 --profile awsmock"

# Create topic
topicarn=$(awslocal sns create-topic --name test-topic | jq -r '.TopicArn')

# Create SQS queue
queueurl=$(awslocal sqs create-queue --queue-name test-queue | jq -r '.QueueUrl')
queuearn=$(awslocal sqs get-queue-attributes --queue-url $queueurl --attribute-names QueueArn | jq -r '.Attributes.QueueArn')

# Create subscription
awslocal sns subscribe --topic-arn $topicarn --protocol sqs --notification-endpoint $queuearn

# Send message
awslocal sns publish --topic-arn $topicarn --message file://./resources/message.json

# See if message arrived in SQS
numberOfMessages=$(awslocal sqs receive-message --queue-url $queueurl --attribute-names ALL | jq length)

echo "Number of messages: $numberOfMessages"
