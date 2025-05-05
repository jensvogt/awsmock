
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
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.queueUrl = Core::CreateSQSQueueUrl(sqsRequest.queueName);
                    sqsRequest.region = clientCommand.region;
                    sqsRequest.owner = clientCommand.user;

                    Dto::SQS::CreateQueueResponse sqsResponse = _sqsService.CreateQueue(sqsRequest);
                    log_info << "Create queue, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, clientCommand.contentType == "application/json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::PURGE_QUEUE: {

                    Dto::SQS::PurgeQueueRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    long deleted = _sqsService.PurgeQueue(sqsRequest);
                    log_info << "Purge queue, queueUrl: " << sqsRequest.queueUrl << " count: " << deleted;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_ATTRIBUTES: {

                    Dto::SQS::GetQueueAttributesRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::GetQueueAttributesResponse sqsResponse = _sqsService.GetQueueAttributes(sqsRequest);
                    log_info << "Get queue attributes, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "application/json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::SET_QUEUE_ATTRIBUTES: {

                    Dto::SQS::SetQueueAttributesRequest sqsRequest = Dto::SQS::SetQueueAttributesRequest::FromJson(clientCommand);
                    _sqsService.SetQueueAttributes(sqsRequest);
                    log_info << "Set queue attributes, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_URL: {

                    Dto::SQS::GetQueueUrlRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::GetQueueUrlResponse sqsResponse = _sqsService.GetQueueUrl(sqsRequest);
                    log_info << "Get queue url, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, clientCommand.contentType == "application/json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
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

                    return SendOkResponse(request, clientCommand.contentType == "application/json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_COUNTERS: {

                    Dto::SQS::ListQueueCountersRequest sqsRequest = Dto::SQS::ListQueueCountersRequest::FromJson(clientCommand);
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

                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    _sqsService.TagQueue(sqsRequest);
                    log_info << "Tag queue, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UNTAG_QUEUE: {

                    Dto::SQS::UntagQueueRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

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
                    log_info << "List queues, count: " << sqsResponse.total;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_ATTRIBUTE_COUNTERS: {

                    Dto::SQS::ListQueueAttributeCountersRequest sqsRequest = Dto::SQS::ListQueueAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListQueueAttributeCountersResponse sqsResponse = _sqsService.ListQueueAttributeCounters(sqsRequest);

                    log_info << "List attributes counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.attributeCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_LAMBDA_TRIGGER_COUNTERS: {

                    Dto::SQS::ListLambdaTriggerCountersRequest sqsRequest = Dto::SQS::ListLambdaTriggerCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListLambdaTriggerCountersResponse sqsResponse = _sqsService.ListLambdaTriggerCounters(sqsRequest);

                    log_info << "List lambda trigger counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.lambdaTriggerCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_TAG_COUNTERS: {

                    Dto::SQS::ListQueueTagCountersRequest sqsRequest = Dto::SQS::ListQueueTagCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListQueueTagCountersResponse sqsResponse = _sqsService.ListTagCounters(sqsRequest);

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
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::DeleteQueueResponse sqsResponse = _sqsService.DeleteQueue(sqsRequest);
                    log_info << "Delete queue, queueUrl: " << sqsRequest.queueUrl;

                    // Empty response
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::SEND_MESSAGE: {

                    Dto::SQS::SendMessageRequest sqsRequest = Dto::SQS::SendMessageRequest::FromJson(clientCommand);
                    Dto::SQS::SendMessageResponse sqsResponse = _sqsService.SendMessage(sqsRequest);
                    log_info << "Send message, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, sqsResponse.ToJson());
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

                    Dto::SQS::ReceiveMessageRequest sqsRequest = Dto::SQS::ReceiveMessageRequest::FromJson(clientCommand);
                    Dto::SQS::ReceiveMessageResponse sqsResponse = _sqsService.ReceiveMessages(sqsRequest);
                    log_trace << "Receive message, count: " << sqsResponse.messageList.size() << " queueUrl: " << sqsRequest.queueUrl;

                    // Send response
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::CHANGE_MESSAGE_VISIBILITY: {

                    Dto::SQS::ChangeMessageVisibilityRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

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

                    Dto::SQS::ListMessageCountersRequest sqsRequest = Dto::SQS::ListMessageCountersRequest::FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;
                    sqsRequest.user = clientCommand.user;
                    sqsRequest.requestId = clientCommand.requestId;

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

                case Dto::Common::SqsCommandType::RESEND_MESSAGE: {

                    Dto::SQS::ResendMessageRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);

                    _sqsService.ResendMessage(sqsRequest);
                    log_info << "Resend message, messageId: " << sqsRequest.messageId;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE: {

                    Dto::SQS::DeleteMessageRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    _sqsService.DeleteMessage(sqsRequest);
                    log_info << "Delete message, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE_BATCH: {

                    Dto::SQS::DeleteMessageBatchRequest sqsRequest;
                    sqsRequest.FromJson(clientCommand.payload);
                    sqsRequest.region = clientCommand.region;

                    Dto::SQS::DeleteMessageBatchResponse sqsResponse = _sqsService.DeleteMessageBatch(sqsRequest);
                    log_info << "Delete message batch, queueUrl: " << sqsRequest.queueUrl;

                    return SendOkResponse(request, clientCommand.contentType == "application/json" ? sqsResponse.ToJson() : sqsResponse.ToXml());
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
                    .attributeName = Core::HttpUtils::GetStringParameter(payload, "UserAttribute." + std::to_string(i) + ".Name"),
                    .attributeValue = Core::HttpUtils::GetStringParameter(payload, "UserAttribute." + std::to_string(i) + ".Value")};
            queueAttributes.emplace_back(attribute);
        }
        return queueAttributes;
    }

    std::map<std::string, std::string> SQSHandler::GetQueueTags(const std::string &payload) {
        std::map<std::string, std::string> queueTags;

        const int count = Core::HttpUtils::CountQueryParametersByPrefix(payload, "Tag") / 2;
        log_trace << "Got tags count, count: " << count;

        for (int i = 1; i <= count; i++) {
            std::string key = Core::HttpUtils::GetStringParameter(payload, "Tag." + std::to_string(i) + ".Key");
            if (std::string value = Core::HttpUtils::GetStringParameter(payload, "Tag." + std::to_string(i) + ".Value"); !key.empty() && !value.empty()) {
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
            attributeNames.emplace_back(Core::HttpUtils::GetStringParameter(payload, "AttributeName." + std::to_string(i)));
        }
        return attributeNames;
    }

    std::map<std::string, Dto::SQS::MessageAttribute> SQSHandler::GetMessageAttributes(const std::string &payload) {
        const int attributeCount = Core::HttpUtils::CountQueryParametersByPrefix(payload, "MessageAttribute");
        log_debug << "Got message attribute count: " << attributeCount;

        std::map<std::string, Dto::SQS::MessageAttribute> messageAttributes;
        for (int i = 1; i <= attributeCount / 3; i++) {
            std::string name = Core::HttpUtils::GetStringParameter(payload, "MessageAttribute." + std::to_string(i) + ".Name");
            std::string dataTape = Core::HttpUtils::GetStringParameter(payload, "MessageAttribute." + std::to_string(i) + ".Value.DataType");

            std::string stringValue;
            if (dataTape == "String" || dataTape == "Number") {
                stringValue = Core::HttpUtils::GetStringParameter(payload, "MessageAttribute." + std::to_string(i) + ".Value.StringValue");
            }
            Dto::SQS::MessageAttribute messageAttribute;
            messageAttribute.stringValue = stringValue;
            messageAttribute.dataType = Dto::SQS::MessageAttributeDataTypeFromString(dataTape);
            messageAttributes[name] = messageAttribute;
        }
        log_debug << "Extracted message attribute count: " << messageAttributes.size();
        return messageAttributes;
    }
}// namespace AwsMock::Service
