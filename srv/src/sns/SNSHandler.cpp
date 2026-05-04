
#include <awsmock/service/sns/SNSHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> SNSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SNS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        try {
            Dto::Common::SNSClientCommand clientCommand;
            clientCommand.FromRequest(request, region, user);
            int format = Core::StringUtils::Contains(clientCommand.contentType, "application/x-www-form-urlencoded") ? FORMAT_XML : FORMAT_JSON;

            switch (clientCommand.command) {
                case Dto::Common::SNSCommandType::CREATE_TOPIC: {
                    Dto::SNS::CreateTopicRequest snsRequest;
                    if (format == FORMAT_XML) {
                        snsRequest.region = region;
                        snsRequest.owner = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Owner");
                        snsRequest.topicName = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Name");
                    } else {
                        snsRequest = Dto::SNS::CreateTopicRequest::FromJson(clientCommand);
                        snsRequest.owner = clientCommand.user;
                        snsRequest.region = clientCommand.region;
                    }
                    Dto::SNS::CreateTopicResponse snsResponse = _snsService.CreateTopic(snsRequest);
                    log_info << "Topic created, name: " << snsRequest.topicName;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_TOPICS: {
                    Dto::SNS::ListTopicsResponse snsResponse = _snsService.ListTopics(clientCommand.region);
                    log_info << "List topics, count: " << snsResponse.topics.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_TOPIC_ARNS: {
                    Dto::SNS::ListTopicArnsResponse snsResponse = _snsService.ListTopicArns(clientCommand.region);
                    log_info << "List topic ARNs";
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_TOPIC_ATTRIBUTES: {
                    std::string topicArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TopicArn");
                    Dto::SNS::GetTopicAttributesRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.topicArn = topicArn;
                    Dto::SNS::GetTopicAttributesResponse snsResponse = _snsService.GetTopicAttributes(snsRequest);

                    log_info << "Get topic attributes, topicArn" << topicArn;
                    return SendResponse(request, http::status::ok, format == FORMAT_XML ? snsResponse.ToXml() : snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_TOPIC_DETAILS: {
                    Dto::SNS::GetTopicDetailsRequest snsRequest = Dto::SNS::GetTopicDetailsRequest::FromJson(clientCommand);
                    Dto::SNS::GetTopicDetailsResponse snsResponse = _snsService.GetTopicDetails(snsRequest);

                    log_info << "Get topic details, topicArn" << snsRequest.topicArn << " " << snsResponse.ToJson();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::PUBLISH: {

                    Dto::SNS::PublishRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.user = clientCommand.user;
                    snsRequest.requestId = clientCommand.requestId;
                    snsRequest.contentType = clientCommand.contentType;
                    snsRequest.topicArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TopicArn");
                    snsRequest.targetArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TargetArn", "");
                    snsRequest.message = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Message", "");
                    snsRequest.messageAttributes = GetMessageAttributes(clientCommand.payload);
                    Dto::SNS::PublishResponse snsResponse = _snsService.Publish(snsRequest);

                    std::map<std::string, std::string> headers;
                    headers["Content-Type"] = "application/xml";
                    headers["Content-Length"] = std::to_string(snsResponse.ToXml().length());
                    headers["amz-sdk-invocation-id"] = snsResponse.requestId;

                    log_info << "Message published, topic: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::SUBSCRIBE: {
                    Dto::SNS::SubscribeRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.user = clientCommand.user;
                    snsRequest.requestId = clientCommand.requestId;
                    snsRequest.topicArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TopicArn");
                    snsRequest.protocol = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Protocol");
                    snsRequest.endpoint = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Endpoint");
                    Dto::SNS::SubscribeResponse snsResponse = _snsService.Subscribe(snsRequest);

                    log_info << "Subscribed to topic, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::UPDATE_SUBSCRIPTION: {
                    Dto::SNS::UpdateSubscriptionRequest snsRequest = Dto::SNS::UpdateSubscriptionRequest::FromJson(clientCommand.payload);
                    Dto::SNS::UpdateSubscriptionResponse snsResponse = _snsService.UpdateSubscription(snsRequest);

                    log_info << "Subscription updated, topicArn: " << snsRequest.topicArn << " subscriptionArn: " << snsResponse.subscriptionArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::UNSUBSCRIBE: {
                    std::string subscriptionArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "SubscriptionArn");
                    Dto::SNS::UnsubscribeRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.user = clientCommand.user;
                    snsRequest.requestId = clientCommand.requestId;
                    snsRequest.subscriptionArn = subscriptionArn;
                    Dto::SNS::UnsubscribeResponse snsResponse = _snsService.Unsubscribe(snsRequest);

                    log_info << "Unsubscribed from topic, subscriptionArn: " << subscriptionArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_SUBSCRIPTIONS_BY_TOPIC: {
                    std::string topicArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TopicArn");

                    Dto::SNS::ListSubscriptionsByTopicRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.topicArn = topicArn;
                    Dto::SNS::ListSubscriptionsByTopicResponse snsResponse = _snsService.ListSubscriptionsByTopic(snsRequest);
                    log_info << "List subscriptions by topic, topicArn: " << topicArn << " count: " << snsResponse.subscriptions.size();
                    std::map<std::string, std::string> headers;
                    headers["Content-Type"] = "application/xml";

                    return SendResponse(request, http::status::ok, snsResponse.ToXml(), headers);
                }

                case Dto::Common::SNSCommandType::TAG_RESOURCE: {
                    std::string resourceArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "Tags.member") / 2;
                    log_trace << "Got tags count, count: " << count;

                    std::map<std::string, std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Tags.member." + std::to_string(i) + ".Key");
                        std::string tagValue = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "Tags.member." + std::to_string(i) + ".Value");
                        tags[tagKey] = tagValue;
                    }
                    Dto::SNS::TagResourceRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.resourceArn = resourceArn;
                    snsRequest.tags = tags;
                    Dto::SNS::TagResourceResponse snsResponse = _snsService.TagResource(snsRequest);

                    log_info << "Topic tagged, resourceArn: " << resourceArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::UNTAG_RESOURCE: {
                    std::string resourceArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "TagKeys.TagKey");
                    log_trace << "Got tags count, count: " << count;

                    std::vector<std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TagKeys.TagKey." + std::to_string(i));
                        tags.emplace_back(tagKey);
                    }
                    Dto::SNS::UntagResourceRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.resourceArn = resourceArn;
                    snsRequest.tags = tags;
                    Dto::SNS::UntagResourceResponse snsResponse = _snsService.UntagResource(snsRequest);

                    log_info << "Topic untagged, resourceArn: " << resourceArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::PURGE_TOPIC: {
                    Dto::SNS::PurgeTopicRequest snsRequest = Dto::SNS::PurgeTopicRequest::FromJson(clientCommand.payload);
                    boost::asio::post(GatewayServer::WorkerPool(), [snsRequest]() {
                        try {
                            const long deleted = SNSService{}.PurgeTopic(snsRequest);
                            log_info << "Topic purged, topicArn: " << snsRequest.topicArn << " count: " << deleted;
                        } catch (const std::exception &e) {
                            log_error << "Purging topic failed: " << e.what();
                        }
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::PURGE_ALL_TOPICS: {
                    boost::asio::post(GatewayServer::WorkerPool(), []() {
                        try {
                            const long purged = SNSService{}.PurgeAllTopics();
                            log_info << "All topic purged, count: " << purged;
                        } catch (const std::exception &e) {
                            log_error << "Purging all topic failed: " << e.what();
                        }
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::DELETE_TOPIC: {
                    Dto::SNS::DeleteTopicRequest snsRequest;
                    if (format == FORMAT_XML) {
                        snsRequest.region = clientCommand.region;
                        snsRequest.user = clientCommand.user;
                        snsRequest.requestId = clientCommand.requestId;
                        snsRequest.topicArn = Core::HttpUtils::GetStringParameterFromBody(clientCommand.payload, "TopicArn");
                    } else {
                        snsRequest = Dto::SNS::DeleteTopicRequest::FromJson(clientCommand.payload);
                    }
                    Dto::SNS::DeleteTopicResponse snsResponse = _snsService.DeleteTopic(snsRequest);
                    log_info << "Topic deleted, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToXml());
                }

                case Dto::Common::SNSCommandType::LIST_MESSAGES: {
                    Dto::SNS::ListMessagesRequest snsRequest = Dto::SNS::ListMessagesRequest::FromJson(clientCommand);
                    Dto::SNS::ListMessagesResponse snsResponse = _snsService.ListMessages(snsRequest);

                    log_info << "List messages, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.messages.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_EVENT_SOURCE: {
                    Dto::SNS::GetEventSourceRequest snsRequest = Dto::SNS::GetEventSourceRequest::FromJson(clientCommand);
                    Dto::SNS::GetEventSourceResponse snsResponse = _snsService.GetEventSource(snsRequest);
                    log_debug << "Get event source, arn: " << snsRequest.eventSourceArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::DELETE_MESSAGE: {
                    Dto::SNS::DeleteMessageRequest snsRequest = Dto::SNS::DeleteMessageRequest::FromJson(clientCommand.payload);
                    boost::asio::post(GatewayServer::WorkerPool(), [snsRequest]() {
                        try {
                            SNSService{}.DeleteMessage(snsRequest);
                            log_info << "Message deleted, messageId: " << snsRequest.messageId;
                        } catch (const std::exception &e) {
                            log_error << "Deleting message failed: " << e.what();
                        }
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::LIST_TOPIC_COUNTERS: {
                    Dto::SNS::ListTopicCountersRequest snsRequest = Dto::SNS::ListTopicCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTopicCountersResponse snsResponse = _snsService.ListTopicCounters(snsRequest);
                    log_debug << "List topic counters, json: " << snsResponse.ToJson();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_MESSAGE_COUNTERS: {
                    Dto::SNS::ListMessageCountersRequest snsRequest = Dto::SNS::ListMessageCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListMessageCountersResponse snsResponse = _snsService.ListMessageCounters(snsRequest);
                    log_debug << "List message counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.messages.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_SUBSCRIPTION_COUNTERS: {
                    Dto::SNS::ListSubscriptionCountersRequest snsRequest = Dto::SNS::ListSubscriptionCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListSubscriptionCountersResponse snsResponse = _snsService.ListSubscriptionCounters(snsRequest);
                    log_debug << "List subscriptions counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.subscriptionCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::ADD_SUBSCRIPTION_COUNTER: {
                    Dto::SNS::AddSubscriptionCounterRequest snsRequest = Dto::SNS::AddSubscriptionCounterRequest::FromJson(clientCommand);
                    _snsService.AddSubscriptionCounter(snsRequest);
                    log_debug << "Add subscription counters, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::GET_SUBSCRIPTION_COUNTER: {
                    Dto::SNS::GetSubscriptionCounterRequest snsRequest = Dto::SNS::GetSubscriptionCounterRequest::FromJson(clientCommand);
                    Dto::SNS::GetSubscriptionCounterResponse snsResponse = _snsService.GetSubscriptionCounter(snsRequest);
                    log_debug << "Get subscription counters, topicArn: " << snsRequest.topicArn << ", subscriptionArn: " << snsRequest.subscriptionArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::UPDATE_SUBSCRIPTION_COUNTER: {
                    Dto::SNS::GetSubscriptionCounterRequest snsRequest = Dto::SNS::GetSubscriptionCounterRequest::FromJson(clientCommand);
                    Dto::SNS::GetSubscriptionCounterResponse snsResponse = _snsService.GetSubscriptionCounter(snsRequest);
                    log_debug << "Update subscription counters, topicArn: " << snsRequest.topicArn << ", subscriptionArn: " << snsRequest.subscriptionArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::DELETE_SUBSCRIPTION_COUNTER: {
                    Dto::SNS::DeleteSubscriptionCounterRequest snsRequest = Dto::SNS::DeleteSubscriptionCounterRequest::FromJson(clientCommand);
                    _snsService.DeleteSubscriptionCounter(snsRequest);
                    log_debug << "Delete subscription counters, topicArn: " << snsRequest.topicArn << ", subscriptionArn: " << snsRequest.subscriptionArn;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::LIST_ATTRIBUTE_COUNTERS: {
                    Dto::SNS::ListAttributeCountersRequest snsRequest = Dto::SNS::ListAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListAttributeCountersResponse snsResponse = _snsService.ListAttributeCounters(snsRequest);
                    log_debug << "List attributes counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.attributeCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::PUBLISH_COUNTER: {
                    Dto::SNS::PublishRequest snsRequest = Dto::SNS::PublishRequest::FromJson(clientCommand);
                    Dto::SNS::PublishResponse snsResponse = _snsService.Publish(snsRequest);
                    log_debug << "List tags counters, topicArn: " << snsRequest.topicArn << ", messageId: " << snsResponse.messageId;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_TAG_COUNTERS: {
                    Dto::SNS::ListTagCountersRequest snsRequest = Dto::SNS::ListTagCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTagCountersResponse snsResponse = _snsService.ListTagCounters(snsRequest);
                    log_debug << "List tags counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.tagCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_MESSAGE_COUNTERS: {
                    Dto::SNS::GetMessageCountersRequest snsRequest = Dto::SNS::GetMessageCountersRequest::FromJson(clientCommand);
                    Dto::SNS::GetMessageCountersResponse snsResponse = _snsService.GetMessageCounters(snsRequest);
                    log_debug << "Get message, messageId: " << snsRequest.messageId;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS: {

                    Dto::SNS::ListDefaultMessageAttributeCountersRequest snsRequest = Dto::SNS::ListDefaultMessageAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListDefaultMessageAttributeCountersResponse snsResponse = _snsService.ListDefaultMessageAttributeCounters(snsRequest);
                    log_info << "List default message attribute counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.attributeCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::ADD_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SNS::AddDefaultMessageAttributeRequest snsRequest = Dto::SNS::AddDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SNS::ListDefaultMessageAttributeCountersResponse snsResponse = _snsService.AddDefaultMessageAttribute(snsRequest);
                    log_info << "Add default message attribute, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::UPDATE_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SNS::UpdateDefaultMessageAttributeRequest snsRequest = Dto::SNS::UpdateDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SNS::ListDefaultMessageAttributeCountersResponse snsResponse = _snsService.UpdateDefaultMessageAttribute(snsRequest);
                    log_info << "Default message attribute updated, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::DELETE_DEFAULT_MESSAGE_ATTRIBUTE: {

                    Dto::SNS::DeleteDefaultMessageAttributeRequest snsRequest = Dto::SNS::DeleteDefaultMessageAttributeRequest::FromJson(clientCommand);
                    Dto::SNS::ListDefaultMessageAttributeCountersResponse snsResponse = _snsService.DeleteDefaultMessageAttribute(snsRequest);
                    log_info << "Delete default message attribute, topicArn: " << snsRequest.topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::RELOAD_ALL_COUNTERS: {

                    _snsService.ReloadAllCounters();
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::RESEND_TOPIC: {

                    Dto::SNS::ResendTopicRequest snsRequest = Dto::SNS::ResendTopicRequest::FromJson(clientCommand);
                    boost::asio::post(GatewayServer::WorkerPool(), [snsRequest]() {
                        try {
                            SNSService{}.ResendTopic(snsRequest);
                            log_info << "Message resend, topicArn: " << snsRequest.topicArn;
                        } catch (const std::exception &e) {
                            log_error << "Resending messages failed: " << e.what();
                        }
                    });
                    return SendResponse(request, http::status::ok);
                }

                default:
                case Dto::Common::SNSCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
                }
            }
        } catch (std::exception &e) {
            log_error << "Exception, error: " << e.what();
            return SendResponse(request, http::status::internal_server_error, e.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendResponse(request, http::status::internal_server_error, "Unknown exception");
        }
    }

    std::map<std::string, Dto::SNS::MessageAttribute> SNSHandler::GetMessageAttributes(const std::string &payload) {

        std::map<std::string, Dto::SNS::MessageAttribute> messageAttributes;
        const int attributeCount = Core::HttpUtils::CountQueryParametersByPrefix(payload, "MessageAttributes");
        log_debug << "Got message attribute count: " << attributeCount;

        if (attributeCount == 0) {
            return messageAttributes;
        }

        for (int i = 1; i <= attributeCount / 2; i++) {
            const std::string attributeName = Core::HttpUtils::GetStringParameterFromBody(payload, "MessageAttributes.entry." + std::to_string(i) + ".Name");

            if (const std::string attributeValue = Core::HttpUtils::GetStringParameterFromBody(payload, "MessageAttributes.entry." + std::to_string(i) + ".Value.StringValue"); !attributeName.empty() && !attributeValue.empty()) {
                Dto::SNS::MessageAttribute attribute;
                attribute.stringValue = attributeValue;
                attribute.dataType = Dto::SNS::MessageAttributeDataTypeFromString("String");
                messageAttributes[attributeName] = attribute;
            }
        }
        log_debug << "Extracted message attribute, count: " << messageAttributes.size();
        return messageAttributes;
    }

    std::map<std::string, std::string> SNSHandler::GetTags(const std::string &payload) {

        std::map<std::string, std::string> tags;
        const int tagCount = Core::HttpUtils::CountQueryParametersByPrefix("/?" + payload, "Tags.member") / 2;
        log_debug << "Got tag count: " << tagCount;

        if (tagCount == 0) {
            return tags;
        }

        for (int i = 1; i <= tagCount; i++) {
            const std::string tagKey = Core::HttpUtils::GetStringParameterFromBody(payload, "Tags.member." + std::to_string(i) + ".Key");
            const std::string tagValue = Core::HttpUtils::GetStringParameterFromBody(payload, "Tags.member." + std::to_string(i) + ".Value");
            tags[tagKey] = tagValue;
        }
        log_trace << "Extracted tags, count: " << tags.size();
        return tags;
    }

}// namespace AwsMock::Service
