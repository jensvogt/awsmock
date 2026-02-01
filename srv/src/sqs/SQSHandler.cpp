

#include <awsmock/service/sqs/SQSHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> SQSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SQS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::SQSClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::SqsCommandType::CREATE_QUEUE: {

                    Dto::SQS::CreateQueueRequest sqsRequest = Dto::SQS::CreateQueueRequest::FromJson(clientCommand);
                    Dto::SQS::CreateQueueResponse sqsResponse = _sqsService.CreateQueue(sqsRequest);
                    log_info << "Create queue, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::PURGE_QUEUE: {

                    Dto::SQS::PurgeQueueRequest sqsRequest = Dto::SQS::PurgeQueueRequest::FromJson(clientCommand);
                    const long purged = _sqsService.PurgeQueue(sqsRequest);
                    log_info << "Purge queue, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl) << " count: " << purged;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::PURGE_ALL_QUEUES: {

                    const long purged = _sqsService.PurgeAllQueues();
                    log_info << "Purge all queues, count: " << purged;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_ATTRIBUTES: {

                    Dto::SQS::GetQueueAttributesRequest sqsRequest = Dto::SQS::GetQueueAttributesRequest::FromJson(clientCommand);
                    Dto::SQS::GetQueueAttributesResponse sqsResponse = _sqsService.GetQueueAttributes(sqsRequest);
                    log_info << "Get queue attributes, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::SET_QUEUE_ATTRIBUTES: {

                    Dto::SQS::SetQueueAttributesRequest sqsRequest = Dto::SQS::SetQueueAttributesRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, sqsRequest] {
                        _sqsService.SetQueueAttributes(sqsRequest);
                        log_info << "Set queue attributes, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);
                    });
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_URL: {

                    Dto::SQS::GetQueueUrlRequest sqsRequest = Dto::SQS::GetQueueUrlRequest::FromJson(clientCommand);
                    Dto::SQS::GetQueueUrlResponse sqsResponse = _sqsService.GetQueueUrl(sqsRequest);
                    log_info << "Get queue url, queueName: " << sqsRequest.queueName;

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::GET_QUEUE_DETAILS: {

                    Dto::SQS::GetQueueDetailsRequest sqsRequest = Dto::SQS::GetQueueDetailsRequest::FromJson(clientCommand);
                    Dto::SQS::GetQueueDetailsResponse sqsResponse = _sqsService.GetQueueDetails(sqsRequest);
                    log_info << "Get queue details, queueArn: " << Core::AwsUtils::ConvertSQSQueueArnToName(sqsRequest.queueArn);

                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_ARNS: {

                    Dto::SQS::ListQueueArnsResponse sqsResponse = _sqsService.ListQueueArns();
                    log_info << "List queue ARNs, count: " << sqsResponse.queueArns.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_COUNTERS: {

                    Dto::SQS::ListQueueCountersRequest sqsRequest = Dto::SQS::ListQueueCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListQueueCountersResponse sqsResponse = _sqsService.ListQueueCounters(sqsRequest);
                    log_debug << "List queue counters, count: " << sqsResponse.queueCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_QUEUE_TAGS: {

                    Dto::SQS::ListQueueTagsRequest sqsRequest = Dto::SQS::ListQueueTagsRequest::FromJson(clientCommand);
                    Dto::SQS::ListQueueTagsResponse sqsResponse = _sqsService.ListQueueTags(sqsRequest);
                    log_info << "List queue tags";
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::TAG_QUEUE: {

                    Dto::SQS::TagQueueRequest sqsRequest = Dto::SQS::TagQueueRequest::FromJson(clientCommand);
                    _sqsService.TagQueue(sqsRequest);
                    log_info << "Tag queue, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UNTAG_QUEUE: {

                    Dto::SQS::UntagQueueRequest sqsRequest = Dto::SQS::UntagQueueRequest::FromJson(clientCommand);
                    _sqsService.UntagQueue(sqsRequest);
                    log_info << "Untag queue, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);

                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::LIST_QUEUES: {

                    Dto::SQS::ListQueuesRequest sqsRequest = Dto::SQS::ListQueuesRequest::FromJson(clientCommand);
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

                    Dto::SQS::UpdateDqlRequest sqsRequest = Dto::SQS::UpdateDqlRequest::FromJson(clientCommand);
                    _sqsService.UpdateDql(sqsRequest);
                    log_info << "Queue DQL subscription updated, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UPDATE_QUEUE: {

                    Dto::SQS::UpdateQueueRequest sqsRequest = Dto::SQS::UpdateQueueRequest::FromJson(clientCommand);
                    _sqsService.UpdateQueue(sqsRequest);
                    log_info << "Update queue, queueArn: " << Core::AwsUtils::ConvertSQSQueueArnToName(sqsRequest.queueArn);
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_QUEUE: {

                    Dto::SQS::DeleteQueueRequest sqsRequest = Dto::SQS::DeleteQueueRequest::FromJson(clientCommand);
                    Dto::SQS::DeleteQueueResponse sqsResponse = _sqsService.DeleteQueue(sqsRequest);
                    log_info << "Delete queue, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::SEND_MESSAGE: {

                    Dto::SQS::SendMessageRequest sqsRequest = Dto::SQS::SendMessageRequest::FromJson(clientCommand);
                    Dto::SQS::SendMessageResponse sqsResponse = _sqsService.SendMessage(sqsRequest);
                    log_info << "Send message, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::SEND_MESSAGE_BATCH: {

                    Dto::SQS::SendMessageBatchRequest sqsRequest = Dto::SQS::SendMessageBatchRequest::FromJson(clientCommand);
                    Dto::SQS::SendMessageBatchResponse sqsResponse = _sqsService.SendMessageBatch(sqsRequest);
                    log_info << "SQS message batch send, successful: " << sqsResponse.successful.size() << " failed: " << sqsResponse.failed.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::RECEIVE_MESSAGE: {

                    Dto::SQS::ReceiveMessageRequest sqsRequest = Dto::SQS::ReceiveMessageRequest::FromJson(clientCommand);
                    Dto::SQS::ReceiveMessageResponse sqsResponse = _sqsService.ReceiveMessages(sqsRequest);
                    log_trace << "Receive message, count: " << sqsResponse.messageList.size() << " queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl);
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::CHANGE_MESSAGE_VISIBILITY: {

                    Dto::SQS::ChangeMessageVisibilityRequest sqsRequest = Dto::SQS::ChangeMessageVisibilityRequest::FromJson(clientCommand);
                    _sqsService.SetMessageVisibilityTimeout(sqsRequest);
                    log_info << "Change visibility, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl) << " timeout: " << sqsRequest.visibilityTimeout;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::LIST_MESSAGES: {

                    Dto::SQS::ListMessagesRequest sqsRequest = Dto::SQS::ListMessagesRequest::FromJson(clientCommand);
                    Dto::SQS::ListMessagesResponse sqsResponse = _sqsService.ListMessages(sqsRequest);
                    log_info << "List queue messages, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_MESSAGE_COUNTERS: {

                    Dto::SQS::ListMessageCountersRequest sqsRequest = Dto::SQS::ListMessageCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListMessageCountersResponse sqsResponse = _sqsService.ListMessageCounters(sqsRequest);
                    log_info << "List queue message counters, queueArn: " << sqsRequest.queueArn << ", count: " << sqsResponse.messages.size() << ", total: " << sqsResponse.total;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_MESSAGE_ATTRIBUTE_COUNTERS: {

                    Dto::SQS::ListMessageAttributeCountersRequest sqsRequest = Dto::SQS::ListMessageAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListMessageAttributeCountersResponse sqsResponse = _sqsService.ListMessageAttributeCounters(sqsRequest);
                    log_info << "List message attribute counters, messageId: " << sqsRequest.messageId << " count: " << sqsResponse.messageAttributeCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS: {

                    Dto::SQS::ListDefaultMessageAttributeCountersRequest sqsRequest = Dto::SQS::ListDefaultMessageAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SQS::ListDefaultMessageAttributeCountersResponse sqsResponse = _sqsService.ListDefaultMessageAttributeCounters(sqsRequest);
                    log_info << "List default message attribute counters, queueArn: " << sqsRequest.queueArn << " count: " << sqsResponse.attributeCounters.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::ADD_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SQS::AddDefaultMessageAttributeRequest sqsRequest = Dto::SQS::AddDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SQS::ListDefaultMessageAttributeCountersResponse sqsResponse = _sqsService.AddDefaultMessageAttribute(sqsRequest);
                    log_info << "Add default message attribute, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::UPDATE_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SQS::UpdateDefaultMessageAttributeRequest sqsRequest = Dto::SQS::UpdateDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SQS::ListDefaultMessageAttributeCountersResponse sqsResponse = _sqsService.UpdateDefaultMessageAttribute(sqsRequest);
                    log_info << "Default message attribute updated, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::DELETE_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SQS::DeleteDefaultMessageAttributeRequest sqsRequest = Dto::SQS::DeleteDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SQS::ListDefaultMessageAttributeCountersResponse sqsResponse = _sqsService.DeleteDefaultMessageAttribute(sqsRequest);
                    log_info << "Delete default message attribute, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::REDRIVE_MESSAGES: {

                    Dto::SQS::RedriveMessagesRequest sqsRequest = Dto::SQS::RedriveMessagesRequest::FromJson(clientCommand);
                    const long count = _sqsService.RedriveMessages(sqsRequest);
                    log_info << "Delete message, queueUrl: " << sqsRequest.queueArn << " count: " << count;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::UPDATE_MESSAGE: {

                    Dto::SQS::UpdateMessageRequest sqsRequest = Dto::SQS::UpdateMessageRequest::FromJson(clientCommand);
                    boost::asio::spawn(_ioc, [this, sqsRequest](boost::asio::yield_context) {
                        _sqsService.UpdateMessage(sqsRequest);
                        log_info << "Update message, messageId: " << sqsRequest.messageId; }, boost::asio::detached);
                    _ioc.poll();
                    _ioc.restart();
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::RESEND_MESSAGE: {

                    Dto::SQS::ResendMessageRequest sqsRequest = Dto::SQS::ResendMessageRequest::FromJson(clientCommand);
                    boost::asio::spawn(_ioc, [this, sqsRequest](boost::asio::yield_context) {
                        _sqsService.ResendMessage(sqsRequest);
                        log_info << "Resend message, messageId: " << sqsRequest.messageId; }, boost::asio::detached);
                    _ioc.poll();
                    _ioc.restart();
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_EVENT_SOURCE: {

                    Dto::SQS::GetEventSourceRequest sqsRequest = Dto::SQS::GetEventSourceRequest::FromJson(clientCommand);
                    Dto::SQS::GetEventSourceResponse sqsResponse = _sqsService.GetEventSource(sqsRequest);
                    log_info << "Get event source, arn: " << Core::AwsUtils::ConvertSQSQueueArnToName(sqsRequest.eventSourceArn);
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE: {

                    Dto::SQS::DeleteMessageRequest sqsRequest = Dto::SQS::DeleteMessageRequest::FromJson(clientCommand);
                    boost::asio::spawn(_ioc, [this, sqsRequest](boost::asio::yield_context) {
                        _sqsService.DeleteMessage(sqsRequest);
                        log_info << "Delete message, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl); }, boost::asio::detached);
                    _ioc.poll();
                    _ioc.restart();
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_MESSAGE_BATCH: {

                    Dto::SQS::DeleteMessageBatchRequest sqsRequest = Dto::SQS::DeleteMessageBatchRequest::FromJson(clientCommand);
                    Dto::SQS::DeleteMessageBatchResponse sqsResponse = _sqsService.DeleteMessageBatch(sqsRequest);
                    log_info << "Delete message batch, queueUrl: " << Core::AwsUtils::ConvertSQSQueueUrlToName(sqsRequest.queueUrl) << ", count: " << sqsRequest.entries.size();
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::ADD_ATTRIBUTE: {

                    Dto::SQS::AddAttributeRequest sqsRequest = Dto::SQS::AddAttributeRequest::FromJson(clientCommand);
                    _sqsService.AddMessageAttribute(sqsRequest);
                    log_info << "Add message attribute, messageId: " << sqsRequest.messageId << ", name: " << sqsRequest.name;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::DELETE_ATTRIBUTE: {

                    Dto::SQS::DeleteAttributeRequest sqsRequest = Dto::SQS::DeleteAttributeRequest::FromJson(clientCommand);
                    _sqsService.DeleteMessageAttribute(sqsRequest);
                    log_info << "Delete message attribute, messageId: " << sqsRequest.messageId << ", name: " << sqsRequest.name;
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::GET_MESSAGE_COUNTERS: {

                    Dto::SQS::GetMessageCountersRequest sqsRequest = Dto::SQS::GetMessageCountersRequest::FromJson(clientCommand);
                    Dto::SQS::GetMessageCountersResponse sqsResponse = _sqsService.GetMessageCounters(sqsRequest);
                    log_info << "Get message, messageId: " << sqsRequest.messageId;
                    return SendOkResponse(request, sqsResponse.ToJson());
                }

                case Dto::Common::SqsCommandType::EXPORT_MESSAGES: {

                    Dto::SQS::ExportMessagesRequest sqsRequest = Dto::SQS::ExportMessagesRequest::FromJson(clientCommand);
                    std::string bsonMessages = _sqsService.ExportMessages(sqsRequest);
                    log_info << "Export messages, queueArn: " << sqsRequest.queueArn;
                    return SendOkResponse(request, bsonMessages);
                }

                case Dto::Common::SqsCommandType::IMPORT_MESSAGES: {

                    Dto::SQS::ImportMessagesRequest sqsRequest = Dto::SQS::ImportMessagesRequest::FromJson(clientCommand.payload);
                    boost::asio::spawn(_ioc, [this, sqsRequest](boost::asio::yield_context) {
                        _sqsService.ImportMessages(sqsRequest);
                        log_info << "Import messages"; }, boost::asio::detached);
                    _ioc.poll();
                    _ioc.restart();
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::RELOAD_COUNTERS: {

                    Dto::SQS::ReloadCountersRequest sqsRequest = Dto::SQS::ReloadCountersRequest::FromJson(clientCommand.payload);
                    boost::asio::spawn(_ioc, [this, sqsRequest](boost::asio::yield_context) {
                        _sqsService.ReloadCounters(sqsRequest);
                        log_info << "Reload counters"; }, boost::asio::detached);
                    _ioc.poll();
                    _ioc.restart();
                    return SendOkResponse(request);
                }

                case Dto::Common::SqsCommandType::RELOAD_ALL_COUNTERS: {

                    Dto::SQS::ReloadCountersRequest sqsRequest = Dto::SQS::ReloadCountersRequest::FromJson(clientCommand.payload);
                    boost::asio::post(_ioc, [this] {
                        _sqsService.ReloadAllCounters();
                    });
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
            log_error << e.what();
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
            Dto::SQS::QueueAttribute attribute;
            attribute.attributeName = Core::HttpUtils::GetStringParameter(payload, "UserAttribute." + std::to_string(i) + ".Name"),
            attribute.attributeValue = Core::HttpUtils::GetStringParameter(payload, "UserAttribute." + std::to_string(i) + ".Value");
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
