# SNS Examples

This directory contains examples for the use of the AwsMock AWS simulation package. Most of the features of AwsMock are
covered in the different samples.

All example need a running awsmock manager listening on port 4566. If you prefer another port, edit the sample bash
script accordingly. To set a port of ```9000``` change the line ```--endpoint http://localhost:4566``` to
```--endpoint http://localhost:9000```.

All examples are cleaned up after execution so that the next example runs without errors.

This directory container the following SQS examples:

- ```create-topic.sh```: creates a new topic ```test-topic```
- ```delete-topic.sh```: deletes the topic ```test-topic```
- ```list-topics.sh```: lists topics
- ```list-subscriptions.sh```: list subscriptions to ```test-topic```
- ```publish-message.sh```: publish a message to ```test-topic```
- ```subscribe-to-sqs-queue.sh```: subscribe a SQS queue to ```test-topic```
- ```unsubscribe-from-sqs-queue.sh```: unsubscribe a SQS queue from ```test-topic```
- ```cleanup.sh```: deletes all created topics and queues

