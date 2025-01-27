//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/sns/SNSService.h>

namespace AwsMock::Service {

    Dto::SNS::CreateTopicResponse SNSService::CreateTopic(const Dto::SNS::CreateTopicRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "create_topic");
        log_trace << "Create topic request: " << request.ToString();

        // Check existence
        if (_snsDatabase.TopicExists(request.region, request.topicName)) {
            log_warning << "SNS topic '" + request.topicName + "' exists already";
            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByName(request.region, request.topicName);
            log_debug << "Got topic: " << topic.topicArn;
            return {
                    .region = topic.region,
                    .name = topic.topicName,
                    .owner = topic.owner,
                    .topicArn = topic.topicArn};
        }

        try {
            // Update database
            const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
            const std::string topicArn = Core::AwsUtils::CreateSNSTopicArn(request.region, accountId, request.topicName);
            Database::Entity::SNS::Topic topic = {.region = request.region, .topicName = request.topicName, .owner = request.owner, .topicArn = topicArn};
            topic = _snsDatabase.CreateTopic(topic);
            log_trace << "SNS topic created: " << topic.ToString();

            return {.region = topic.region, .name = topic.topicName, .owner = topic.owner, .topicArn = topic.topicArn};

        } catch (Core::DatabaseException &exc) {
            log_error << "SNS create topic failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SNS::ListTopicsResponse SNSService::ListTopics(const std::string &region) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_topics");
        log_trace << "List all topics request, region: " << region;

        try {

            const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(region);
            auto listTopicsResponse = Dto::SNS::ListTopicsResponse(topicList);
            log_trace << "SNS list topics response: " << listTopicsResponse.ToXml();

            return listTopicsResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListTopicCountersResponse SNSService::ListTopicCounters(const Dto::SNS::ListTopicCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_topic_counters");
        log_trace << "List all topics counters request, request: " << request.ToString();

        try {

            Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(request.prefix, request.pageSize, request.pageIndex, request.sortColumns, request.region);

            Dto::SNS::ListTopicCountersResponse listTopicResponse;
            listTopicResponse.total = _snsDatabase.CountTopics(request.region, request.prefix);
            for (const auto &topic: topicList) {
                Dto::SNS::TopicCounter counter;
                counter.topicName = topic.topicName;
                counter.topicArn = topic.topicArn;
                counter.topicUrl = topic.topicUrl;
                counter.availableMessages = topic.topicAttribute.availableMessages;
                counter.size = topic.size;
                counter.created = topic.created;
                counter.modified = topic.modified;
                listTopicResponse.topicCounters.emplace_back(counter);
            }
            log_trace << "SNS list topic counters response: " << listTopicResponse.ToJson();
            return listTopicResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    long SNSService::PurgeTopic(const Dto::SNS::PurgeTopicRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "purge_topic");
        log_trace << "Purge topic request, topicArn: " << request.topicArn;

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_warning << "Topic does not exist, topicArn: " << request.topicArn;
            throw Core::NotFoundException("Topic does not exist, topicArn" + request.topicArn);
        }

        try {
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            log_debug << "Got topic: " << topic.topicArn;

            // Update database
            const long deleted = _snsDatabase.PurgeTopic(topic);
            log_debug << "SNS topic prune, deleted: " << deleted;

            // Adjust topic counters
            AdjustTopicCounters(topic);

            return deleted;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS purge topic failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::DeleteTopicResponse SNSService::DeleteTopic(const std::string &region, const std::string &topicArn) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "delete_topic");
        log_trace << "Delete topic request, region: " << region << " topicArn: " << topicArn;

        // Check existence
        if (!_snsDatabase.TopicExists(topicArn)) {
            log_warning << "Topic does not exist, arn: " << topicArn;
            throw Core::NotFoundException("Topic does not exist");
        }

        Dto::SNS::DeleteTopicResponse response;
        try {

            // Update database
            _snsDatabase.DeleteTopic({.region = region, .topicArn = topicArn});

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS delete topic failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        return response;
    }

    Dto::SNS::PublishResponse SNSService::Publish(const Dto::SNS::PublishRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "publish");
        log_trace << "Publish message request: " << request.ToString();

        // Check topic/target ARN
        if (request.targetArn.empty() && request.topicArn.empty()) {
            log_error << "Either topicARN or targetArn must exist";
            throw Core::ServiceException("Either topicARN or targetArn must exist");
        }

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "Topic does not exist: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists");
        }

        try {
            Database::Entity::SNS::Message message;

            Database::Entity::SNS::Topic topic;
            if (!request.topicArn.empty()) {
                topic = _snsDatabase.GetTopicByArn(request.topicArn);
            } else if (!request.topicArn.empty()) {
                topic = _snsDatabase.GetTopicByTargetArn(request.targetArn);
            }

            // Update database
            std::string messageId = Core::AwsUtils::CreateMessageId();
            message = {.region = request.region,
                       .topicArn = request.topicArn,
                       .targetArn = request.targetArn,
                       .message = request.message,
                       .messageId = messageId,
                       .size = static_cast<long>(request.message.length())};
            for (const auto &[fst, snd]: request.messageAttributes) {
                Database::Entity::SNS::MessageAttribute attribute = {.attributeName = fst, .attributeValue = snd.stringValue, .attributeType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.type))};
                message.messageAttributes.emplace_back(attribute);
            }
            message = _snsDatabase.CreateMessage(message);

            // Check subscriptions
            CheckSubscriptions(request);

            // Adjust message counters
            AdjustTopicCounters(topic);

            return {.messageId = message.messageId, .requestId = request.requestId};

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS create message failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::SubscribeResponse SNSService::Subscribe(const Dto::SNS::SubscribeRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "subscribe");
        log_trace << "Subscribe request: " << request.ToString();

        try {

            // Check topic/target ARN
            if (request.topicArn.empty()) {
                log_error << "Topic ARN missing";
                throw Core::ServiceException("Topic ARN missing");
            }

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                log_error << "SNS topic does not exists";
                throw Core::ServiceException("SNS topic does not exists");
            }

            // Create new subscription
            const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            const std::string subscriptionArn = Core::AwsUtils::CreateSNSSubscriptionArn(request.region, accountId, topic.topicName);

            if (const Database::Entity::SNS::Subscription subscription = {.protocol = request.protocol, .endpoint = request.endpoint}; !topic.HasSubscription(subscription)) {

                // Add subscription
                topic.subscriptions.push_back({.protocol = request.protocol,
                                               .endpoint = request.endpoint,
                                               .subscriptionArn = subscriptionArn});

                // Save to database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }

            return {.subscriptionArn = subscriptionArn};

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UpdateSubscriptionResponse SNSService::UpdateSubscription(const Dto::SNS::UpdateSubscriptionRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "update_subscription");
        log_trace << "Update subscription request: " << request.ToString();

        // Check topic/target ARN
        if (request.topicArn.empty()) {
            log_error << "Topic ARN missing";
            throw Core::ServiceException("Topic ARN missing");
        }

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists";
            throw Core::ServiceException("SNS topic does not exists");
        }

        try {

            // Get topic
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            // Create new subscription
            if (!topic.HasSubscription(request.subscriptionArn)) {
                log_error << "SNS topic subscription does not exists";
                throw Core::ServiceException("SNS topic subscription does not exists");
            }

            const int index = topic.GetSubscriptionIndex(request.subscriptionArn);
            topic.subscriptions.at(index).endpoint = request.endpoint;
            topic.subscriptions.at(index).protocol = request.protocol;

            // Save to database
            topic = _snsDatabase.UpdateTopic(topic);
            log_debug << "Subscription updated, topic: " << topic.ToString();

            return {.subscriptionArn = request.subscriptionArn};

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UnsubscribeResponse SNSService::Unsubscribe(const Dto::SNS::UnsubscribeRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "unsubscribe");
        log_trace << "Unsubscribe request: " << request.ToString();

        try {

            // Check topic/target ARN
            if (request.subscriptionArn.empty()) {
                log_error << "Subscription ARN missing";
                throw Core::ServiceException("Subscription ARN missing");
            }

            // Create new subscription

            for (Database::Entity::SNS::TopicList topics = _snsDatabase.GetTopicsBySubscriptionArn(request.subscriptionArn); auto &topic: topics) {

                // Remove subscription
                const auto count = std::erase_if(topic.subscriptions, [request](const auto &item) {
                    return item.subscriptionArn == request.subscriptionArn;
                });
                log_debug << "Subscription removed, count" << count;

                // Save to database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }

            return {.subscriptionArn = request.subscriptionArn};

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListSubscriptionsByTopicResponse SNSService::ListSubscriptionsByTopic(const Dto::SNS::ListSubscriptionsByTopicRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_subscriptions");
        log_trace << "List subscriptions request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListSubscriptionsByTopicResponse response;
            for (const auto &[protocol, endpoint, subscriptionArn]: topic.subscriptions) {
                Dto::SNS::Subscription subscription = {.topicArn = request.topicArn, .protocol = protocol, .subscriptionArn = subscriptionArn, .endpoint = endpoint};
                response.subscriptions.emplace_back(subscription);
                response.nextToken = subscription.id;
            }
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get topic subscriptions failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListSubscriptionCountersResponse SNSService::ListSubscriptionCounters(const Dto::SNS::ListSubscriptionCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_subscription_counters");
        log_trace << "List subscription counters request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListSubscriptionCountersResponse response;
            response.total = topic.subscriptions.size();
            for (const auto &[protocol, endpoint, subscriptionArn]: topic.subscriptions) {
                const std::string id = subscriptionArn.substr(subscriptionArn.rfind(':') + 1);
                Dto::SNS::Subscription subscription = {.id = id, .topicArn = request.topicArn, .protocol = protocol, .subscriptionArn = subscriptionArn, .endpoint = endpoint};
                response.subscriptionCounters.emplace_back(subscription);
            }
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get subscription counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListAttributeCountersResponse SNSService::ListAttributeCounters(const Dto::SNS::ListAttributeCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_attribute_counters");
        log_trace << "List attribute counters request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListAttributeCountersResponse response;
            response.total = 11;
            Dto::SNS::AttributeCounter attributeCounter;
            attributeCounter = {.attributeKey = "availableMessages", .attributeValue = std::to_string(topic.topicAttribute.availableMessages)};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "archivePolicy", .attributeValue = topic.topicAttribute.archivePolicy};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "beginningArchiveTime", .attributeValue = Core::DateTimeUtils::ToISO8601(topic.topicAttribute.beginningArchiveTime)};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "contentBasedDeduplication", .attributeValue = Core::StringUtils::ToString(topic.topicAttribute.contentBasedDeduplication)};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "deliveryPolicy", .attributeValue = topic.topicAttribute.deliveryPolicy};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "displayName", .attributeValue = topic.topicAttribute.displayName};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "fifoTopic", .attributeValue = Core::StringUtils::ToString(topic.topicAttribute.fifoTopic)};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "kmsMasterKeyId", .attributeValue = topic.topicAttribute.kmsMasterKeyId};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "policy", .attributeValue = topic.topicAttribute.policy};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "signatureVersion", .attributeValue = topic.topicAttribute.signatureVersion};
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter = {.attributeKey = "tracingConfig", .attributeValue = topic.topicAttribute.tracingConfig};
            response.attributeCounters.emplace_back(attributeCounter);

            auto endArray = response.attributeCounters.begin() + request.pageSize * (request.pageIndex + 1);
            if (request.pageSize * (request.pageIndex + 1) > 11) {
                endArray = response.attributeCounters.end();
            }
            response.attributeCounters = std::vector(response.attributeCounters.begin() + request.pageSize * request.pageIndex, endArray);
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get attribute counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListTagCountersResponse SNSService::ListTagCounters(const Dto::SNS::ListTagCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_tag_counters");
        log_trace << "List tag counters request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListTagCountersResponse response;
            response.total = topic.tags.size();
            for (const auto &[fst, snd]: topic.tags) {
                Dto::SNS::TagCounter tagCounter = {.tagKey = fst, .tagValue = snd};
                response.tagCounters.emplace_back(tagCounter);
            }
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get tag counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetTopicAttributesResponse SNSService::GetTopicAttributes(const Dto::SNS::GetTopicAttributesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "get_topic_attributes");
        log_trace << "Get topic attributes request: " << request.ToString();

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists");
            }

            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            return {
                    .region = topic.region,
                    .topicArn = topic.topicArn,
                    .owner = topic.owner};

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get topic attributes failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetTopicDetailsResponse SNSService::GetTopicDetails(const Dto::SNS::GetTopicDetailsRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "get_topic_details");
        log_trace << "Get topic details request: " << request.ToString();

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
                throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
            }

            const long messageCount = _snsDatabase.CountMessages(request.topicArn);
            const long messagesSize = _snsDatabase.CountMessagesSize(request.topicArn);

            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            return {
                    .region = topic.region,
                    .topicName = topic.topicName,
                    .topicArn = topic.topicArn,
                    .messageCount = messageCount,
                    .size = messagesSize,
                    .owner = topic.owner,
                    .created = topic.created,
                    .modified = topic.modified,
            };

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get topic attributes failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SNSService::CheckSubscriptions(const Dto::SNS::PublishRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "check_subscriptions");
        log_trace << "Check subscriptions request: " << request.ToString();

        if (const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn); !topic.subscriptions.empty()) {

            for (const auto &it: topic.subscriptions) {

                if (Core::StringUtils::ToLower(it.protocol) == SQS_PROTOCOL) {

                    SendSQSMessage(it, request);
                    log_debug << "Message send to SQS queue, queueArn: " << it.endpoint;
                }
            }
        }
    }

    Dto::SNS::TagResourceResponse SNSService::TagResource(const Dto::SNS::TagResourceRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "tag_topic");
        log_trace << "Tag topic request: " << request.ToString();

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.resourceArn)) {
                log_error << "SNS topic does not exists, topicArn: " << request.resourceArn;
                throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.resourceArn);
            }

            // Get the topic
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.resourceArn);

            // Set tags and update database
            topic.tags = request.tags;
            topic = _snsDatabase.UpdateTopic(topic);
            log_info << "SNS tags updated, count: " << topic.tags.size();

            constexpr Dto::SNS::TagResourceResponse response;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS tag resource failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UntagResourceResponse SNSService::UntagResource(const Dto::SNS::UntagResourceRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "untag_topic");
        log_trace << "Untag topic request: " << request.ToString();

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.resourceArn)) {
                log_error << "SNS topic does not exists, topicArn: " << request.resourceArn;
                throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.resourceArn);
            }

            // Get the topic
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.resourceArn);

            // Set tags and update database
            int count = 0;
            for (const auto &it: request.tags) {
                count += std::erase_if(topic.tags, [it](const auto &item) {
                    auto const &[key, value] = item;
                    return key == it;
                });
            }
            topic = _snsDatabase.UpdateTopic(topic);
            log_info << "SNS tags updated, topicArn: " << topic.topicArn << " count: " << count;

            constexpr Dto::SNS::UntagResourceResponse response;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS untag resource failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SNSService::SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request) const {
        log_debug << "Send to SQS queue, queueUrl: " << subscription.endpoint;

        // Get queue by ARN
        const Database::Entity::SQS::Queue sqsQueue = _sqsDatabase.GetQueueByArn(subscription.endpoint);
        log_debug << "Found queue, queueUrl: " << sqsQueue.name;

        // Create a SQS notification request
        const Dto::SNS::SqsNotificationRequest sqsNotificationRequest = {
                .type = "Notification",
                .messageId = Core::AwsUtils::CreateMessageId(),
                .topicArn = request.topicArn,
                .message = request.message,
                .timestamp = Core::DateTimeUtils::UnixTimestamp(system_clock::now())};

        // Wrap it in a SQS message request
        Dto::SQS::SendMessageRequest sendMessageRequest = {
                .region = request.region,
                .queueUrl = sqsQueue.queueUrl,
                .queueArn = sqsQueue.queueArn,
                .body = sqsNotificationRequest.ToJson(),
                .messageId = Core::AwsUtils::CreateMessageId(),
                .requestId = Core::AwsUtils::CreateRequestId(),
        };

        for (const auto &[fst, snd]: request.messageAttributes) {
            const Dto::SQS::MessageAttribute messageAttribute = {.name = fst, .stringValue = snd.stringValue, .numberValue = snd.numberValue, .binaryValue = snd.binaryValue, .type = Dto::SQS::MessageAttributeDataTypeFromString(MessageAttributeDataTypeToString(snd.type))};
            sendMessageRequest.messageAttributes[fst] = messageAttribute;
        }

        const Dto::SQS::SendMessageResponse response = _sqsService.SendMessage(sendMessageRequest);
        log_trace << "SNS SendMessage response: " << response.ToString();
    }

    Dto::SNS::ListMessagesResponse SNSService::ListMessages(const Dto::SNS::ListMessagesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_messages");
        log_trace << "List all messages request, region: " << request.region << " topicArn: " << request.topicArn;

        try {

            const long total = _snsDatabase.CountMessages(request.topicArn);

            const Database::Entity::SNS::MessageList messageList = _snsDatabase.ListMessages(request.region, request.topicArn, request.pageSize, request.pageIndex);

            Dto::SNS::ListMessagesResponse listMessageResponse = Dto::SNS::Mapper::map(request, messageList);
            listMessageResponse.total = total;
            log_trace << "SNS list messages, response: " << listMessageResponse.ToJson();

            return listMessageResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListMessageCountersResponse SNSService::ListMessageCounters(const Dto::SNS::ListMessageCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "list_message_counters");
        log_trace << "List message counters request, region: " << request.region << " topicArn: " << request.topicArn;

        try {

            const long total = _snsDatabase.CountMessages(request.topicArn);

            const Database::Entity::SNS::MessageList messageList = _snsDatabase.ListMessages(request.region, request.topicArn, request.pageSize, request.pageIndex, request.sortColumns);

            Dto::SNS::ListMessageCountersResponse listMessageCountersResponse = Dto::SNS::Mapper::map(messageList);
            listMessageCountersResponse.total = total;
            log_trace << "SNS list messages, response: " << listMessageCountersResponse.ToJson();

            return listMessageCountersResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SNSService::DeleteMessage(const Dto::SNS::DeleteMessageRequest &request) const {
        Monitoring::MetricServiceTimer measure(SNS_SERVICE_TIMER, "method", "delete_message");
        log_trace << "Delete a message request, messageId: " << request.messageId;

        if (!_snsDatabase.MessageExists(request.messageId)) {
            log_error << "SNS message does not exists, messageId: " << request.messageId;
            throw Core::ServiceException("SNS message does not exists, messageId: " + request.messageId);
        }

        try {

            _snsDatabase.DeleteMessage(request.messageId);
            log_trace << "SNS message deleted, messageId: " << request.messageId;

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            // Adjust topic counters
            AdjustTopicCounters(topic);
            log_trace << "SNS topci counter adjusted, topicArn: " << request.topicArn;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SNSService::AdjustTopicCounters(Database::Entity::SNS::Topic &topic) const {
        topic.topicAttribute.availableMessages = _snsDatabase.CountMessages(topic.topicArn);
        topic.size = _snsDatabase.GetTopicSize(topic.topicArn);
        topic = _snsDatabase.UpdateTopic(topic);
        log_debug << "Topic counters updated, queue: " << topic.topicArn;
    }

}// namespace AwsMock::Service
