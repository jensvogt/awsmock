# SPDX-License-Identifier: Apache-2.0

"""
Purpose:
Demonstrate processing of a SQS message.
"""

import json
import logging
import os

import boto3
from botocore.exceptions import ClientError

# Logging
logger = logging.getLogger(__name__)

# Default context
endpoint_url = os.environ.get('AWS_AWSMOCK_ENDPOINT')
input_queue = os.environ.get('AWS_SQS_INPUT_QUEUE')
protocolizing_topic = os.environ.get('AWS_SNS_PROTOCOLIZING_TOPIC')

# Default AWS clients
sqs = boto3.client("sqs", endpoint_url=endpoint_url)
sns = boto3.client("sns", endpoint_url=endpoint_url)


def initialize():
    """
    Print some values from the environment variables
    """
    print("AwsMock endpoint: ", endpoint_url)
    print("InputQueue: ", input_queue)
    print("ProtocolizingTopic: ", input_queue)


def get_queue(name):
    """
    Gets an SQS queue by name.

    :param name: The name that was used to create the queue.
    :return: the queue URL.
    """
    try:
        response = sqs.get_queue_url(QueueName=name)
        logger.info("Got queue '%s' with URL=%s", name, response["QueueUrl"])
    except ClientError as error:
        logger.exception("Couldn't get queue named %s.", name)
        raise error
    else:
        return response["QueueUrl"]


def publish_message(topic, message, attributes):
    """
    Publishes a message, with attributes, to a topic. Subscriptions can be filtered
    based on message attributes so that a subscription receives messages only
    when specified attributes are present.

    :param topic: The topic to publish to.
    :param message: The message to publish.
    :param attributes: The key-value attributes to attach to the message. Values
                       must be either `str` or `bytes`.
    :return: The ID of the message.
    """
    try:
        att_dict = {}
        for key, value in attributes.items():
            if isinstance(value, str):
                att_dict[key] = {"DataType": "String", "StringValue": value}
            elif isinstance(value, bytes):
                att_dict[key] = {"DataType": "Binary", "BinaryValue": value}
        response = sns.publish(TopicArn=topic["TopicArn"], Message=message, MessageAttributes=att_dict)
        message_id = response["MessageId"]
        logger.info("Published message with attributes %s to topic %s.", attributes, topic["TopicArn"])
    except ClientError:
        logger.exception("Couldn't publish message to topic %s.", topic.arn)
        raise
    else:
        return message_id


def receive_messages():
    """
    Receive messages from the JSON processing queue
    Example:
    xml_string =
    <library>
        <book id="1">
            <title>Clean Code</title>
            <author>Robert C. Martin</author>
        </book>
        <book id="2">
            <title>Effective Python</title>
            <author>Brett Slatkin</author>
        </book>
    </library>
    """
    print("Start receiving messages from the file-distribution-json queue")

    # Get the SQS queue
    queue_url = get_queue(input_queue)
    print("Queue: ", queue_url)

    # Get the protocolizing topic
    topic = sns.create_topic(Name=protocolizing_topic)
    print("Topic: ", topic["TopicArn"])

    while True:
        # Receive messages from the JSON processing queue
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
            print("MessageId: ", msg["MessageId"])
            print("Body: ", msg["Body"])

            # If the body is JSON, parse it
            try:
                root = ET.fromstring(msg["Body"])
                for book in root.findall("book"):
                    print("Parsed XML: ", book.find("title").text)

            except json.JSONDecodeError:
                pass

            # Send to SNS topic
            publish_message(topic, "JSON message processed.", {"file_origin": "FTP_UPLOAD"})
            # Delete SQS message
            sqs.delete_message(QueueUrl=queue_url, ReceiptHandle=msg["ReceiptHandle"])
            print("Message deleted:", msg["MessageId"])
            print("-" * 40)


if __name__ == "__main__":
    initialize()
    receive_messages()
