# SPDX-License-Identifier: Apache-2.0

"""
Purpose

Demonstrate processing of a SQS message.
"""

import boto3
import json
# snippet-start:[python.example_code.sqs.queue_wrapper_imports]
import logging
from botocore.exceptions import ClientError

logger = logging.getLogger(__name__)
endpoint_url = "http://localhost:4566"
sqs = boto3.client("sqs", endpoint_url=endpoint_url)
sns = boto3.client("sns", endpoint_url=endpoint_url)


# snippet-start:[python.example_code.sqs.GetQueueUrl]

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


# snippet-end:[python.example_code.sqs.GetQueueUrl]

@staticmethod
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


# snippet-start:[python.example_code.sqs.Scenario_ManageQueues]
def receive_messages():
    """
    Receive messages from the xml-processing queue
    """
    print("Start receiving messages from the file-distribution-xml queue")
    queueUrl = get_queue("file-distribution-xml-queue")
    print("Queue: ", queueUrl)
    topic_name = "protocolizing-topic"
    topic = sns.create_topic(Name=topic_name)
    print("Topic: ", topic["TopicArn"])

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
                # Send to SNS topic
                publish_message(topic, "XML file processed.", {"file_origin": "FTP_UPLOAD"})
                # Delete SQS message
                sqs.delete_message(QueueUrl=queueUrl, ReceiptHandle=msg["ReceiptHandle"])
                print("Message deleted:", msg["MessageId"])
            except json.JSONDecodeError:
                pass

            print("-" * 40)


# snippet-end:[python.example_code.sqs.Scenario_ManageQueues]


if __name__ == "__main__":
    receive_messages()
