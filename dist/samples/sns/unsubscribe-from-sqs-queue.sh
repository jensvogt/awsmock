#!/bin/bash

alias awslocal="aws --region eu-central-1 --endpoint http://localhost:4566 --profile awsmock"

# Create topic
topicarn=$(awslocal sns create-topic --name test-topic | jq -r '.TopicArn')

# Create SQS queue
queueurl=$(awslocal sqs create-queue --queue-name test-queue | jq -r '.QueueUrl')
queuearn=$(awslocal sqs get-queue-attributes --queue-url $queueurl --attribute-names QueueArn | jq -r '.Attributes.QueueArn')

# Create subscription
subscriptionarn=$(awslocal sns subscribe --topic-arn $topicarn --protocol sqs --notification-endpoint queuearn | jq -r '.SubscriptionArn')

# Unsubscribe topic
awslocal sns unsubscribe --subscription-arn $subscriptionarn

# List subscriptions
awslocal sns list-subscriptions-by-topic --topic-arn $topicarn