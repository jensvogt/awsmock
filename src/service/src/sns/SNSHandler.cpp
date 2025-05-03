
#include <awsmock/service/sns/SNSHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> SNSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SNS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        try {

            Dto::Common::SNSClientCommand clientCommand;
            clientCommand.FromRequest(request, region, user);

            switch (clientCommand.command) {

                case Dto::Common::SNSCommandType::CREATE_TOPIC: {

                    std::string name = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Name");
                    log_debug << "Topic name: " << name;

                    Dto::SNS::CreateTopicRequest snsRequest = {.region = clientCommand.region, .topicName = name, .owner = clientCommand.user};
                    Dto::SNS::CreateTopicResponse snsResponse = _snsService.CreateTopic(snsRequest);

                    log_info << "Topic created, name: " << name;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_TOPICS: {

                    Dto::SNS::ListTopicsResponse snsResponse = _snsService.ListTopics(clientCommand.region);

                    log_info << "List topics";
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::GET_TOPIC_ATTRIBUTES: {

                    std::string topicArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TopicArn");

                    Dto::SNS::GetTopicAttributesResponse snsResponse = _snsService.GetTopicAttributes({.region = clientCommand.region, .topicArn = topicArn});

                    log_info << "Get topic attributes, topicArn" << topicArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::GET_TOPIC_DETAILS: {

                    Dto::SNS::GetTopicDetailsRequest snsRequest;
                    snsRequest.FromJson(clientCommand.payload);

                    Dto::SNS::GetTopicDetailsResponse snsResponse = _snsService.GetTopicDetails(snsRequest);

                    log_info << "Get topic details, topicArn" << snsRequest.topicArn;
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::PUBLISH: {

                    std::string topicArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TopicArn");
                    std::string targetArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TargetArn");
                    std::string message = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Message");
                    std::map<std::string, Dto::SNS::MessageAttribute> messageAttributes = GetMessageAttributes(clientCommand.payload);

                    Dto::SNS::PublishRequest snsRequest = {.region = clientCommand.region, .topicArn = topicArn, .targetArn = targetArn, .message = message, .messageAttributes = messageAttributes, .requestId = clientCommand.requestId};
                    Dto::SNS::PublishResponse snsResponse = _snsService.Publish(snsRequest);
                    log_trace << "SNS PUBLISH, request: " << snsRequest.ToString();

                    std::map<std::string, std::string> headers;
                    headers["Content-Type"] = "application/xml";
                    headers["Content-Length"] = std::to_string(snsResponse.ToXml().length());
                    headers["amz-sdk-invocation-id"] = snsResponse.requestId;

                    log_info << "Message published, topic: " << topicArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::SUBSCRIBE: {

                    std::string topicArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TopicArn");
                    std::string protocol = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Protocol");
                    std::string endpoint = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Endpoint");

                    Dto::SNS::SubscribeResponse snsResponse = _snsService.Subscribe({.region = clientCommand.region, .topicArn = topicArn, .protocol = protocol, .endpoint = endpoint, .owner = clientCommand.user});

                    log_info << "Subscribed to topic, topicArn: " << topicArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::UPDATE_SUBSCRIPTION: {

                    Dto::SNS::UpdateSubscriptionRequest snsRequest;
                    snsRequest.FromJson(clientCommand.payload);

                    Dto::SNS::UpdateSubscriptionResponse snsResponse = _snsService.UpdateSubscription(snsRequest);

                    log_info << "Subscription updated, topicArn: " << snsRequest.topicArn << " subscriptionArn: " << snsResponse.subscriptionArn;
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::UNSUBSCRIBE: {

                    std::string subscriptionArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "SubscriptionArn");

                    Dto::SNS::UnsubscribeResponse snsResponse = _snsService.Unsubscribe({.region = clientCommand.region, .subscriptionArn = subscriptionArn});

                    log_info << "Unsubscribed from topic, subscriptionArn: " << subscriptionArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_SUBSCRIPTIONS_BY_TOPIC: {
                    std::string topicArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TopicArn");

                    Dto::SNS::ListSubscriptionsByTopicResponse snsResponse = _snsService.ListSubscriptionsByTopic({.region = clientCommand.region, .topicArn = topicArn});
                    log_info << "List subscriptions by topic, topicArn: " << topicArn << " count: " << snsResponse.subscriptions.size();
                    std::map<std::string, std::string> headers;
                    headers["Content-Type"] = "application/xml";

                    return SendOkResponse(request, snsResponse.ToXml(), headers);
                }

                case Dto::Common::SNSCommandType::TAG_RESOURCE: {

                    std::string resourceArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "Tags.Tag") / 2;
                    log_trace << "Got tags count, count: " << count;

                    std::map<std::string, std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Tags.Tag." + std::to_string(i) + ".Key");
                        std::string tagValue = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "Tags.Tag." + std::to_string(i) + ".Value");
                        tags[tagKey] = tagValue;
                    }
                    Dto::SNS::TagResourceRequest snsRequest = {.region = clientCommand.region, .resourceArn = resourceArn, .tags = tags};
                    Dto::SNS::TagResourceResponse snsResponse = _snsService.TagResource(snsRequest);

                    log_info << "Topic tagged, resourceArn: " << resourceArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::UNTAG_RESOURCE: {

                    std::string resourceArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "TagKeys.TagKey");
                    log_trace << "Got tags count, count: " << count;

                    std::vector<std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TagKeys.TagKey." + std::to_string(i));
                        tags.emplace_back(tagKey);
                    }
                    Dto::SNS::UntagResourceRequest snsRequest = {.region = clientCommand.region, .resourceArn = resourceArn, .tags = tags};
                    Dto::SNS::UntagResourceResponse snsResponse = _snsService.UntagResource(snsRequest);

                    log_info << "Topic untagged, resourceArn: " << resourceArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::PURGE_TOPIC: {

                    Dto::SNS::PurgeTopicRequest snsRequest;
                    snsRequest.FromJson(clientCommand.payload);
                    log_debug << "Purge topic, topicArn: " << snsRequest.topicArn;

                    long deleted = _snsService.PurgeTopic(snsRequest);

                    log_info << "Topic purged, topicArn: " << snsRequest.topicArn << " count: " << deleted;
                    return SendOkResponse(request);
                }

                case Dto::Common::SNSCommandType::DELETE_TOPIC: {

                    std::string topicArn = Core::HttpUtils::GetStringParameterFromPayload(clientCommand.payload, "TopicArn");
                    log_debug << "Topic ARN: " << topicArn;

                    Dto::SNS::DeleteTopicResponse snsResponse = _snsService.DeleteTopic(clientCommand.region, topicArn);

                    log_info << "Topic deleted, topicArn: " << topicArn;
                    return SendOkResponse(request, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_MESSAGES: {

                    Dto::SNS::ListMessagesRequest snsRequest;
                    snsRequest.FromJson(clientCommand.payload);
                    snsRequest.region = region;

                    log_debug << "List messages, payload: " << clientCommand.payload;
                    log_debug << "List messages, topicArn: " << snsRequest.topicArn;

                    Dto::SNS::ListMessagesResponse snsResponse = _snsService.ListMessages(snsRequest);

                    log_info << "List messages, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.messageList.size();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::DELETE_MESSAGE: {

                    Dto::SNS::DeleteMessageRequest snsRequest;
                    snsRequest.FromJson(clientCommand.payload);

                    _snsService.DeleteMessage(snsRequest);

                    log_info << "Message deleted, messageId: " << snsRequest.messageId;
                    return SendOkResponse(request, "{}");
                }

                case Dto::Common::SNSCommandType::LIST_TOPIC_COUNTERS: {

                    Dto::SNS::ListTopicCountersRequest snsRequest = Dto::SNS::ListTopicCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTopicCountersResponse snsResponse = _snsService.ListTopicCounters(snsRequest);
                    log_trace << "List topic counters, json: " << snsResponse.ToJson();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_MESSAGE_COUNTERS: {

                    Dto::SNS::ListMessageCountersRequest snsRequest = Dto::SNS::ListMessageCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListMessageCountersResponse snsResponse = _snsService.ListMessageCounters(snsRequest);
                    log_trace << "List message counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.messages.size();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_SUBSCRIPTION_COUNTERS: {

                    Dto::SNS::ListSubscriptionCountersRequest snsRequest = Dto::SNS::ListSubscriptionCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListSubscriptionCountersResponse snsResponse = _snsService.ListSubscriptionCounters(snsRequest);
                    log_trace << "List subscriptions counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.subscriptionCounters.size();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_ATTRIBUTE_COUNTERS: {

                    Dto::SNS::ListAttributeCountersRequest snsRequest = Dto::SNS::ListAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListAttributeCountersResponse snsResponse = _snsService.ListAttributeCounters(snsRequest);
                    log_trace << "List attributes counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.attributeCounters.size();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_TAG_COUNTERS: {

                    Dto::SNS::ListTagCountersRequest snsRequest = Dto::SNS::ListTagCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTagCountersResponse snsResponse = _snsService.ListTagCounters(snsRequest);
                    log_trace << "List tags counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.tagCounters.size();
                    return SendOkResponse(request, snsResponse.ToJson());
                }

                default:
                case Dto::Common::SNSCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    return SendBadRequestError(request, "Unknown method");
                }
            }

        } catch (std::exception &e) {
            log_error << "Exception, error: " << e.what();
            return SendInternalServerError(request, e.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendInternalServerError(request, "Unknown exception");
        }
    }

    std::map<std::string, Dto::SNS::MessageAttribute> SNSHandler::GetMessageAttributes(const std::string &payload) {

        const int attributeCount = Core::HttpUtils::CountQueryParametersByPrefix("/?" + payload, "MessageAttributes");
        log_debug << "Got message attribute count: " << attributeCount;

        std::map<std::string, Dto::SNS::MessageAttribute> messageAttributes;
        for (int i = 1; i <= attributeCount / 3; i++) {

            std::string attributeName = Core::HttpUtils::GetStringParameterFromPayload(payload, "MessageAttributes.entry." + std::to_string(i) + ".Name");
            std::string attributeType = Core::HttpUtils::GetStringParameterFromPayload(payload, "MessageAttributes.entry." + std::to_string(i) + ".Value.DataType");

            std::string attributeValue;
            if (Core::StringUtils::StartsWith(attributeType, "String") || Core::StringUtils::StartsWith(attributeType, "Number")) {
                attributeValue = Core::HttpUtils::GetStringParameterFromPayload(payload, "MessageAttributes.entry." + std::to_string(i) + ".Value.StringValue");
            }
            messageAttributes[attributeName] = {.name = attributeName, .stringValue = attributeValue, .type = Dto::SNS::MessageAttributeDataTypeFromString(attributeType)};
        }
        log_debug << "Extracted message attribute count: " << messageAttributes.size();
        return messageAttributes;
    }
}// namespace AwsMock::Service
