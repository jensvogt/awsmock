
#include <awsmock/service/sqs/SQSHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> SQSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SQS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::SQSClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::SqsCommandType::CREATE_QUEUE: {

                    Dto::SQS::CreateQueueRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.queueUrl = Core::CreateSQSQueueUrl(sqsRequest.queueName);
                        sqsRequest.region = clientCommand.region;
                        sqsRequest.owner = clientCommand.user;

                    } else {

                        std::string queueName = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueName");
                        std::string queueUrl = Core::CreateSQSQueueUrl(queueName);
                        std::vector<Dto::SQS::QueueAttribute> attributes = GetQueueAttributes(clientCommand.payload);
                        std::map<std::string, std::string> tags = GetQueueTags(clientCommand.payload);

                        sqsRequest = {.region = clientCommand.region, .queueName = queueName, .queueUrl = queueUrl, .owner = clientCommand.user, .attributes = attributes, .tags = tags, .requestId = Core::AwsUtils::CreateRequestId()};
                    }

                    Dto::SQS::CreateQueueResponse sqsResponse = _sqsService.CreateQueue(sqsRequest);
                    log_info << "Create queue, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::PURGE_QUEUE: {

                    Dto::SQS::PurgeQueueRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl};
                    }
                    long deleted = _sqsService.PurgeQueue(sqsRequest);
                    log_info << "Purge queue, queueUrl: " << sqsRequest.queueUrl << " count: " << deleted;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_ATTRIBUTES: {

                    Dto::SQS::GetQueueAttributesRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        std::vector<std::string> attributeNames = GetQueueAttributeNames(clientCommand.payload);
                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .attributeNames = attributeNames};
                    }

                    Dto::SQS::GetQueueAttributesResponse sqsResponse = _sqsService.GetQueueAttributes(sqsRequest);
                    log_info << "Get queue attributes, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::SET_QUEUE_ATTRIBUTES: {
                    Dto::SQS::SetQueueAttributesRequest sqsRequest;

                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");

                        int count = Core::HttpUtils::CountQueryParametersByPrefix(clientCommand.payload, "Attribute") / 2;
                        log_trace << "Got attribute count, count: " << count;

                        std::map<std::string, std::string> attributes;
                        for (int i = 1; i <= count; i++) {
                            std::string attributeName = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "Attribute." + std::to_string(i) + ".Name");
                            std::string attributeValue = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "Attribute." + std::to_string(i) + ".Value");
                            attributes[attributeName] = attributeValue;
                        }

                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .attributes = attributes};
                    }

                    Dto::SQS::SetQueueAttributesResponse sqsResponse = _sqsService.SetQueueAttributes(sqsRequest);
                    log_info << "Set queue attributes, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_URL: {
                    Dto::SQS::GetQueueUrlRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueName = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueName");
                        sqsRequest = {.region = clientCommand.region, .queueName = queueName};
                    }
                    Dto::SQS::GetQueueUrlResponse sqsResponse = _sqsService.GetQueueUrl(sqsRequest);
                    log_info << "Get queue url, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_DETAILS: {

                    Dto::SQS::GetQueueDetailsRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::GetQueueDetailsResponse sqsResponse = _sqsService.GetQueueDetails(sqsRequest);
                    log_info << "Get queue details, queueArn: " << sqsRequest.queueArn;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_ARNS: {

                    Dto::SQS::ListQueueArnsResponse sqsResponse = _sqsService.ListQueueArns();
                    log_info << "List queue ARNs";
                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_COUNTERS: {

                    Dto::SQS::ListQueueCountersRequest sqsRequest{};
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::ListQueueCountersResponse sqsResponse = _sqsService.ListQueueCounters(sqsRequest);
                    log_debug << "List queue counters, count: " << sqsResponse.queueCounters.size();

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_TAGS: {

                    Dto::SQS::ListQueueTagsRequest sqsRequest{};
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::ListQueueTagsResponse sqsResponse = _sqsService.ListQueueTags(sqsRequest);
                    log_info << "List queue tags";

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::TAG_QUEUE: {
                    Dto::SQS::TagQueueRequest sqsRequest;

                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        std::string tagKey = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "Tag.Key");
                        std::string tagValue = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "Tag.Key");

                        std::map<std::string, std::string> tags;
                        tags[tagKey] = tagValue;

                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .tags = tags};
                    }
                    _sqsService.TagQueue(sqsRequest);
                    log_info << "Tag queue, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UNTAG_QUEUE: {

                    Dto::SQS::UntagQueueRequest sqsRequest;

                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        std::string tagKey = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "Tag.Key");

                        std::vector<std::string> tags;
                        tags.emplace_back(tagKey);

                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .tags = tags};
                    }
                    _sqsService.UntagQueue(sqsRequest);
                    log_info << "Untag queue, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::LIST_QUEUES: {

                    Dto::SQS::ListQueuesRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    std::string tmp = Core::HttpUtils::GetBodyAsString(request);

                    Dto::SQS::ListQueuesResponse sqsResponse = _sqsService.ListQueues(sqsRequest);
                    log_info << "List queues";

                    if (clientCommand.contentType == "json") {

                        return SendOkResponse(request, sqsResponse.ToJson());
                    }
                    return SendOkResponse(request, sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_ATTRIBUTE_COUNTERS: {

                    Dto::SQS::ListQueueAttributeCountersRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    Dto::SQS::ListQueueAttributeCountersResponse sqsResponse = _sqsService.ListQueueAttributeCounters(sqsRequest);

                    log_info << "List attributes counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.attributeCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_LAMBDA_TRIGGER_COUNTERS: {

                    Dto::SQS::ListLambdaTriggerCountersRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    Dto::SQS::ListLambdaTriggerCountersResponse sqsResponse = _sqsService.ListLambdaTriggerCounters(sqsRequest);

                    log_info << "List lambda trigger counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.lambdaTriggerCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_TAG_COUNTERS: {

                    Dto::SQS::ListTagCountersRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    Dto::SQS::ListTagCountersResponse sqsResponse = _sqsService.ListTagCounters(sqsRequest);

                    log_info << "List tags counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.tagCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::UPDATE_DLQ: {

                    Dto::SQS::UpdateDqlRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    _sqsService.UpdateDql(sqsRequest);

                    log_info << "Queue DQL subscription updated, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_QUEUE: {

                    Dto::SQS::DeleteQueueRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        sqsRequest = {.queueUrl = queueUrl};
                        sqsRequest.region = clientCommand.region;
                        sqsRequest.requestId = Core::AwsUtils::CreateRequestId();
                    }

                    Dto::SQS::DeleteQueueResponse sqsResponse = _sqsService.DeleteQueue(sqsRequest);
                    log_info << "Delete queue, queueUrl: " << sqsRequest.queueUrl;

                    // Empty response
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::SEND_MESSAGE: {

                    Dto::SQS::SendMessageRequest sqsRequest;
                    sqsRequest.region = clientCommand.region;
                    sqsRequest.messageId = Core::AwsUtils::CreateRequestId();
                    sqsRequest.senderId = clientCommand.user;
                    sqsRequest.contentType = clientCommand.contentType;

                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        std::string body = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "MessageBody");
                        std::map<std::string, Dto::SQS::MessageAttribute> attributes = GetMessageAttributes(clientCommand.payload);

                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .body = body, .messageAttributes = attributes, .messageId = Core::AwsUtils::CreateRequestId()};
                    }

                    // Call service
                    Dto::SQS::SendMessageResponse sqsResponse = _sqsService.SendMessage(sqsRequest);
                    log_info << "Send message, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::SEND_MESSAGE_BATCH: {

                    Dto::SQS::SendMessageBatchRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    // Call service
                    Dto::SQS::SendMessageBatchResponse sqsResponse = _sqsService.SendMessageBatch(sqsRequest);
                    log_info << "SQS message batch send, successful: " << sqsResponse.successful.size() << " failed: " << sqsResponse.failed.size();

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::RECEIVE_MESSAGE: {

                    Dto::SQS::ReceiveMessageRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        int maxMessages = Core::HttpUtils::GetIntParameter(clientCommand.payload, "MaxNumberOfMessages", 1, 10, 3);
                        int waitTimeSeconds = Core::HttpUtils::GetIntParameter(clientCommand.payload, "WaitTimeSeconds", 1, 900, 5);
                        int visibility = Core::HttpUtils::GetIntParameter(clientCommand.payload, "VisibilityTimeout", 1, 900, 30);
                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .maxMessages = maxMessages, .visibilityTimeout = visibility, .waitTimeSeconds = waitTimeSeconds, .requestId = Core::AwsUtils::CreateRequestId()};
                    }
                    Dto::SQS::ReceiveMessageResponse sqsResponse = _sqsService.ReceiveMessages(sqsRequest);
                    log_trace << "Receive message, count: " << sqsResponse.messageList.size() << " queueUrl: " << sqsRequest.queueUrl;

                    // Send response
                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::CHANGE_MESSAGE_VISIBILITY: {

                    Dto::SQS::ChangeMessageVisibilityRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        int visibilityTimeout = Core::HttpUtils::GetIntParameter(clientCommand.payload, "VisibilityTimeout", 30, 12 * 3600, 60);
                        std::string receiptHandle = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "ReceiptHandle");

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        sqsRequest = {.region = clientCommand.region, .queueUrl = queueUrl, .receiptHandle = receiptHandle, .visibilityTimeout = visibilityTimeout};
                    }

                    _sqsService.SetVisibilityTimeout(sqsRequest);
                    log_info << "Change visibility, queueUrl: " << sqsRequest.queueUrl << " timeout: " << sqsRequest.visibilityTimeout;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::LIST_MESSAGES: {

                    Dto::SQS::ListMessagesRequest sqsRequest{};
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::ListMessagesResponse sqsResponse = _sqsService.ListMessages(sqsRequest);
                    log_info << "List queue messages, queueArn: " << sqsRequest.queueArn;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_MESSAGE_COUNTERS: {

                    Dto::SQS::ListMessageCountersRequest sqsRequest{};
                    sqsRequest.FromJson(clientCommand.payload);

                    Dto::SQS::ListMessageCountersResponse sqsResponse = _sqsService.ListMessageCounters(sqsRequest);
                    log_debug << "List queue message counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.total;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::REDRIVE_MESSAGES: {

                    Dto::SQS::RedriveMessagesRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    long count = _sqsService.RedriveMessages(sqsRequest);
                    log_info << "Delete message, queueUrl: " << sqsRequest.queueArn << " count: " << count;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UPDATE_MESSAGE: {

                    Dto::SQS::UpdateMessageRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    _sqsService.UpdateMessage(sqsRequest);
                    log_info << "Update message, messageId: " << sqsRequest.messageId;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE: {

                    Dto::SQS::DeleteMessageRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        std::string queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");
                        std::string receiptHandle = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "ReceiptHandle");
                        sqsRequest = {
                                .region = clientCommand.region,
                                .queueUrl = queueUrl,
                                .receiptHandle = receiptHandle};
                    }

                    _sqsService.DeleteMessage(sqsRequest);
                    log_info << "Delete message, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE_BATCH: {

                    Dto::SQS::DeleteMessageBatchRequest sqsRequest;
                    if (clientCommand.contentType == "json") {

                        sqsRequest.FromJson(clientCommand.payload);
                        sqsRequest.region = clientCommand.region;

                    } else {

                        sqsRequest.queueUrl = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "QueueUrl");

                        // Get message count
                        int count = Core::HttpUtils::CountQueryParametersByPrefix(clientCommand.payload, "DeleteMessageBatchRequestEntry") / 2;
                        log_trace << "Got entry count, count: " << count;

                        for (int i = 1; i <= count; i++) {
                            std::string id = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "DeleteMessageBatchRequestEntry." + std::to_string(i) + ".Id");
                            std::string receiptHandle = Core::HttpUtils::GetQueryParameterValueByName(clientCommand.payload, "DeleteMessageBatchRequestEntry." + std::to_string(i) + ".ReceiptHandle");
                            Dto::SQS::DeleteMessageBatchEntry entry = {.id = id, .receiptHandle = receiptHandle};
                            sqsRequest.deleteMessageBatchEntries.emplace_back(entry);
                        }
                    }
                    Dto::SQS::DeleteMessageBatchResponse sqsResponse = _sqsService.DeleteMessageBatch(sqsRequest);
                    log_info << "Delete message batch, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::DELETE_ATTRIBUTE: {

                    Dto::SQS::DeleteAttributeRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    _sqsService.DeleteMessageAttribute(sqsRequest);
                    log_info << "Delete message attribute, messageId: " << sqsRequest.messageId;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    return Core::HttpUtils::BadRequest(request, "Unknown method");
                }

                default:
                    log_error << "Unknown method";
                    return Core::HttpUtils::BadRequest(request, "Unknown method");
            }

        } catch (std::exception &e) {
            return SendInternalServerError(request, e.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendInternalServerError(request, "Unknown exception");
        }
    }

    std::vector<Dto::SQS::QueueAttribute> SQSHandler::GetQueueAttributes(const std::string &payload) {

        std::vector<Dto::SQS::QueueAttribute> queueAttributes;

        const int count = Core::HttpUtils::CountQueryParametersByPrefix(payload, "UserAttribute") / 2;
        log_trace << "Got attribute count, count: " << count;

        for (int i = 1; i <= count; i++) {
            Dto::SQS::QueueAttribute attribute = {
                    .attributeName = Core::HttpUtils::GetQueryParameterValueByName(payload, "UserAttribute." + std::to_string(i) + ".Name"),
                    .attributeValue = Core::HttpUtils::GetQueryParameterValueByName(payload, "UserAttribute." + std::to_string(i) + ".Value")};
            queueAttributes.emplace_back(attribute);
        }
        return queueAttributes;
    }

    std::map<std::string, std::string> SQSHandler::GetQueueTags(const std::string &payload) {

        std::map<std::string, std::string> queueTags;

        const int count = Core::HttpUtils::CountQueryParametersByPrefix(payload, "Tag") / 2;
        log_trace << "Got tags count, count: " << count;

        for (int i = 1; i <= count; i++) {
            std::string key = Core::HttpUtils::GetQueryParameterValueByName(payload, "Tag." + std::to_string(i) + ".Key");
            if (std::string value = Core::HttpUtils::GetQueryParameterValueByName(payload, "Tag." + std::to_string(i) + ".Value"); !key.empty() && !value.empty()) {
                queueTags[key] = value;
            }
        }
        return queueTags;
    }

    std::vector<std::string> SQSHandler::GetQueueAttributeNames(const std::string &payload) {

        const int count = Core::HttpUtils::CountQueryParametersByPrefix(payload, "AttributeName");
        log_trace << "Got attribute names count: " << count;

        std::vector<std::string> attributeNames;
        for (int i = 1; i <= count; i++) {
            attributeNames.emplace_back(Core::HttpUtils::GetQueryParameterValueByName(payload, "AttributeName." + std::to_string(i)));
        }
        return attributeNames;
    }

    std::map<std::string, Dto::SQS::MessageAttribute> SQSHandler::GetMessageAttributes(const std::string &payload) {

        const int attributeCount = Core::HttpUtils::CountQueryParametersByPrefix(payload, "MessageAttribute");
        log_debug << "Got message attribute count: " << attributeCount;

        std::map<std::string, Dto::SQS::MessageAttribute> messageAttributes;
        for (int i = 1; i <= attributeCount / 3; i++) {

            std::string attributeName = Core::HttpUtils::GetQueryParameterValueByName(payload, "MessageAttribute." + std::to_string(i) + ".Name");
            std::string attributeType = Core::HttpUtils::GetQueryParameterValueByName(payload, "MessageAttribute." + std::to_string(i) + ".Value.DataType");

            std::string attributeValue;
            if (attributeType == "String" || attributeType == "Number") {
                attributeValue = Core::HttpUtils::GetQueryParameterValueByName(payload, "MessageAttribute." + std::to_string(i) + ".Value.StringValue");
            }
            const Dto::SQS::MessageAttribute messageAttribute = {.name = attributeName, .stringValue = attributeValue, .type = Dto::SQS::MessageAttributeDataTypeFromString(attributeType)};
            messageAttributes[attributeName] = messageAttribute;
        }
        log_debug << "Extracted message attribute count: " << messageAttributes.size();
        return messageAttributes;
    }

}// namespace AwsMock::Service
