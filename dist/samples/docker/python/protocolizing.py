# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Purpose

Demonstrate basic queue operations in Amazon Simple Queue Service (Amazon SQS).
Learn how to create, get, and remove standard, FIFO, and dead-letter queues.
Usage is shown in the test/test_queue_wrapper.py file.
"""

import boto3
# snippet-start:[python.example_code.sqs.queue_wrapper_imports]
import logging
from botocore.exceptions import ClientError

logger = logging.getLogger(__name__)
endpoint_url = "http://localhost:4566"
sqs = boto3.client("sqs", endpoint_url=endpoint_url)


# snippet-start:[python.example_code.sqs.GetQueueUrl]


def get_queue(name):
    """
    Gets an SQS queue by name.

    :param name: The name that was used to create the queue.
    :return: A Queue object.
    """
    try:
        response = sqs.get_queue_url(QueueName=name)
        logger.info("Got queue '%s' with URL=%s", name, response["QueueUrl"])
    except ClientError as error:
        logger.exception("Couldn't get queue named %s.", name)
        raise error
    else:
        return response["QueueUrl"]


# snippet-end:[python.example_code.sqs.GetQueueUrl]


# snippet-start:[python.example_code.sqs.Scenario_ManageQueues]
def receive_messages():
    """
    Receive messages from the protocolizing queue
    """
    print("Start receiving messages from the protocolizing queue")
    queueUrl = get_queue("protocolizing-queue")
    print("Queue: ", queueUrl)

    while True:
        response = sqs.receive_message(
            QueueUrl=queueUrl,
            MaxNumberOfMessages=10,
            WaitTimeSeconds=20,
            MessageAttributeNames=["All"],
        )

        messages = response.get("Messages", [])

        if not messages:
            print("No messages available.")
            continue

        for msg in messages:
            print("MessageId:", msg["MessageId"])
            print("Body:", msg["Body"])

            # If the body is JSON, parse it
            try:
                body_data = json.loads(msg["Body"])
                print("Parsed JSON:", body_data)
            except json.JSONDecodeError:
                pass

            print("-" * 40)


# snippet-end:[python.example_code.sqs.Scenario_ManageQueues]


if __name__ == "__main__":
    receive_messages()
