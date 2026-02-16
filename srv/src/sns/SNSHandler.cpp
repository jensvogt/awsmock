
#include <awsmock/service/sns/SNSHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> SNSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SNS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        try {
            Dto::Common::SNSClientCommand clientCommand;
            clientCommand.FromRequest(request, region, user);

            switch (clientCommand.command) {
                case Dto::Common::SNSCommandType::CREATE_TOPIC: {
                    Dto::SNS::CreateTopicRequest snsRequest;
                    if (Core::StringUtils::Contains(clientCommand.contentType, "x-www-form-urlencoded")) {
                        snsRequest.region = region;
                        snsRequest.owner = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Owner");
                        snsRequest.topicName = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Name");
                    } else {
                        snsRequest = Dto::SNS::CreateTopicRequest::FromJson(clientCommand.payload);
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
                    std::string topicArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TopicArn");

                    Dto::SNS::GetTopicAttributesRequest snsRequest;
                    snsRequest.region = clientCommand.region;
                    snsRequest.topicArn = topicArn;
                    Dto::SNS::GetTopicAttributesResponse snsResponse = _snsService.GetTopicAttributes(snsRequest);

                    log_info << "Get topic attributes, topicArn" << topicArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
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
                    snsRequest.contentType = clientCommand.contentType;
                    snsRequest.topicArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TopicArn");
                    snsRequest.targetArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TargetArn");
                    snsRequest.message = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Message");
                    snsRequest.messageAttributes = GetMessageAttributes(clientCommand.payload);
                    snsRequest.requestId = clientCommand.requestId;
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
                    snsRequest.topicArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TopicArn");
                    snsRequest.protocol = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Protocol");
                    snsRequest.endpoint = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Endpoint");
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
                    std::string subscriptionArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "SubscriptionArn");
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
                    std::string topicArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TopicArn");

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
                    std::string resourceArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "Tags.member") / 2;
                    log_trace << "Got tags count, count: " << count;

                    std::map<std::string, std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Tags.member." + std::to_string(i) + ".Key");
                        std::string tagValue = Core::HttpUtils::GetStringParameter(clientCommand.payload, "Tags.member." + std::to_string(i) + ".Value");
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
                    std::string resourceArn = Core::HttpUtils::GetStringParameter(clientCommand.payload, "ResourceArn");
                    log_debug << "Resource ARN: " << resourceArn;

                    int count = Core::HttpUtils::CountQueryParametersByPrefix("/?" + clientCommand.payload, "TagKeys.TagKey");
                    log_trace << "Got tags count, count: " << count;

                    std::vector<std::string> tags;
                    for (int i = 1; i <= count; i++) {
                        std::string tagKey = Core::HttpUtils::GetStringParameter(clientCommand.payload, "TagKeys.TagKey." + std::to_string(i));
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
                    long deleted = _snsService.PurgeTopic(snsRequest);

                    log_info << "Topic purged, topicArn: " << snsRequest.topicArn << " count: " << deleted;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::PURGE_ALL_TOPICS: {
                    boost::asio::post(_ioc, [self = shared_from_this()] {
                        const long purged = self->_snsService.PurgeAllTopics();
                        log_info << "All topic purged, count: " << purged;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::DELETE_TOPIC: {
                    Dto::SNS::DeleteTopicRequest snsRequest = Dto::SNS::DeleteTopicRequest::FromJson(clientCommand.payload);
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

                case Dto::Common::SNSCommandType::DELETE_MESSAGE: {
                    Dto::SNS::DeleteMessageRequest snsRequest = Dto::SNS::DeleteMessageRequest::FromJson(clientCommand.payload);
                    boost::asio::post(_ioc, [snsRequest, self = shared_from_this()] {
                        self->_snsService.DeleteMessage(snsRequest);
                        log_info << "Message deleted, messageId: " << snsRequest.messageId;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::SNSCommandType::LIST_TOPIC_COUNTERS: {
                    Dto::SNS::ListTopicCountersRequest snsRequest = Dto::SNS::ListTopicCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTopicCountersResponse snsResponse = _snsService.ListTopicCounters(snsRequest);
                    log_trace << "List topic counters, json: " << snsResponse.ToJson();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_MESSAGE_COUNTERS: {
                    Dto::SNS::ListMessageCountersRequest snsRequest = Dto::SNS::ListMessageCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListMessageCountersResponse snsResponse = _snsService.ListMessageCounters(snsRequest);
                    log_trace << "List message counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.messages.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_SUBSCRIPTION_COUNTERS: {
                    Dto::SNS::ListSubscriptionCountersRequest snsRequest = Dto::SNS::ListSubscriptionCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListSubscriptionCountersResponse snsResponse = _snsService.ListSubscriptionCounters(snsRequest);
                    log_trace << "List subscriptions counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.subscriptionCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_ATTRIBUTE_COUNTERS: {
                    Dto::SNS::ListAttributeCountersRequest snsRequest = Dto::SNS::ListAttributeCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListAttributeCountersResponse snsResponse = _snsService.ListAttributeCounters(snsRequest);
                    log_trace << "List attributes counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.attributeCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::PUBLISH_COUNTER: {
                    Dto::SNS::PublishRequest snsRequest = Dto::SNS::PublishRequest::FromJson(clientCommand);
                    Dto::SNS::PublishResponse snsResponse = _snsService.Publish(snsRequest);
                    log_trace << "List tags counters, topicArn: " << snsRequest.topicArn << ", messageId: " << snsResponse.messageId;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::LIST_TAG_COUNTERS: {
                    Dto::SNS::ListTagCountersRequest snsRequest = Dto::SNS::ListTagCountersRequest::FromJson(clientCommand);
                    Dto::SNS::ListTagCountersResponse snsResponse = _snsService.ListTagCounters(snsRequest);
                    log_trace << "List tags counters, topicArn: " << snsRequest.topicArn << " count: " << snsResponse.tagCounters.size();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_EVENT_SOURCE: {
                    Dto::SNS::GetEventSourceRequest snsRequest = Dto::SNS::GetEventSourceRequest::FromJson(clientCommand);
                    Dto::SNS::GetEventSourceResponse snsResponse = _snsService.GetEventSource(snsRequest);
                    log_info << "Get event source, arn: " << snsRequest.eventSourceArn;
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
                }

                case Dto::Common::SNSCommandType::GET_MESSAGE_COUNTERS: {
                    Dto::SNS::GetMessageCountersRequest snsRequest = Dto::SNS::GetMessageCountersRequest::FromJson(clientCommand);
                    Dto::SNS::GetMessageCountersResponse snsResponse = _snsService.GetMessageCounters(snsRequest);
                    log_info << "Get message, messageId: " << snsRequest.messageId << ", content: " << snsResponse.ToJson();
                    return SendResponse(request, http::status::ok, snsResponse.ToJson());
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
        const int attributeCount = Core::HttpUtils::CountQueryParametersByPrefix(payload, "MessageAttributes");
        log_debug << "Got message attribute count: " << attributeCount;

        std::map<std::string, Dto::SNS::MessageAttribute> messageAttributes;
        for (int i = 1; i <= attributeCount / 2; i++) {
            const std::string attributeName = Core::HttpUtils::GetStringParameter(payload, "MessageAttributes.entry." + std::to_string(i) + ".Name");

            if (const std::string attributeValue = Core::HttpUtils::GetStringParameter(payload, "MessageAttributes.entry." + std::to_string(i) + ".Value.StringValue"); !attributeName.empty() && !attributeValue.empty()) {
                Dto::SNS::MessageAttribute attribute;
                attribute.stringValue = attributeValue;
                attribute.dataType = Dto::SNS::MessageAttributeDataTypeFromString("String");
                messageAttributes[attributeName] = attribute;
            }
        }
        log_debug << "Extracted message attribute count: " << messageAttributes.size();
        return messageAttributes;
    }
}// namespace AwsMock::Service
