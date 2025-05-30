#!/bin/bash

# Create queue
queueurl=$(awslocal sqs create-queue --queue-name test-queue | jq -r '.QueueUrl')

# Send message
awslocal sqs send-message --queue-url $queueurl --message-body file://./resources/message.json --message-attributes file://./resources/messageAttributes.json

# See if message arrived in SQS
numberOfMessages=$(awslocal sqs get-queue-attributes --queue-url $queueurl --attribute-names ALL | jq -r '.Attributes.ApproximateNumberOfMessages')

echo "Number of messages: $numberOfMessages"
