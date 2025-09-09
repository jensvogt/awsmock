# SPDX-License-Identifier: Apache-2.0

"""
Purpose

Demonstrate basic topic operations using AwsMock.
"""

import boto3
import json
import logging
import os
from botocore.exceptions import ClientError

# Logging
logger = logging.getLogger(__name__)

# Default context
input_queue = "protocolizing-queue"
endpoint_url = "http://awsmock:4566"

# Default AWS clients
sqs = boto3.client("sqs", endpoint_url=endpoint_url)


def initialize():
    """
    Set some values from environment variables
    """
    global input_queue, endpoint_url, sqs
    input_queue = os.environ.get("AWS_SQS_INPUT_QUEUE")
    print("InputQueue: ", input_queue)
    endpoint_url = os.environ.get("AWS_AWSMOCK_ENDPOINT")
    sqs = boto3.client("sqs", endpoint_url=endpoint_url)
    print("AwsMock endpoint: ", endpoint_url)


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


def receive_messages():
    """
    Receive messages from the protocolizing queue
    """

    print("Start receiving messages from the protocolizing queue")

    # Get queue URL
    queue_url = get_queue(input_queue)
    print("Queue: ", queue_url)

    while True:
        # Get messages
        response = sqs.receive_message(
            QueueUrl=queue_url,
            MaxNumberOfMessages=10,
            WaitTimeSeconds=20,
            MessageAttributeNames=["All"],
        )
        messages = response.get("Messages", [])

        if not messages:
            print("No messages available.")
            continue

        for msg in messages:
            print("-" * 40)
            print("MessageId:", msg["MessageId"])
            print("Body:", msg["Body"])

            # If the body is JSON, parse it
            try:
                body_data = json.loads(msg["Body"])
                print("Parsed JSON:", body_data)
            except json.JSONDecodeError:
                pass

            # Delete SQS message
            sqs.delete_message(QueueUrl=queue_url, ReceiptHandle=msg["ReceiptHandle"])
            print("Message deleted:", msg["MessageId"])

            print("-" * 40)


if __name__ == "__main__":
    initialize()
    receive_messages()
