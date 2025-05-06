#!/bin/bash

# Create topic
topicarn=$(aws sns create-topic \
--name test-topic \
--region eu-central-1 \
--endpoint http://localhost:4566 \
--profile awsmock | jq -r '.TopicArn')

# Create SQS queue
queueurl=$(aws sqs create-queue \
--queue-name test-queue \
--region eu-central-1 \
--endpoint http://localhost:4566 \
--profile awsmock | jq -r '.QueueUrl')

# Create subscription
aws sns subscribe \
--topic-arn $topicarn \
--protocol sqs \
--notification-endpoint $queueurl \
--region eu-central-1 \
--endpoint http://localhost:4566 \
--profile awsmock

aws sns list-subscriptions-by-topic \
--topic-arn $topicarn \
--region eu-central-1 \
--endpoint http://localhost:4566 \
--profile awsmock

