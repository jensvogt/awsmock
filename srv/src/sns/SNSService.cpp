//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/sns/SNSService.h>

namespace AwsMock::Service {

    Dto::SNS::CreateTopicResponse SNSService::CreateTopic(const Dto::SNS::CreateTopicRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "create_topic");
        log_trace << "Create topic request: " << request.ToString();

        // Check existence
        if (_snsDatabase.TopicExists(request.region, request.topicName)) {

            log_warning << "SNS topic '" + request.topicName + "' exists already";
            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByName(request.region, request.topicName);
            log_debug << "Got topic: " << topic.topicArn;

            Dto::SNS::CreateTopicResponse response;
            response.region = topic.region,
            response.topicName = topic.topicName,
            response.owner = topic.owner,
            response.topicArn = topic.topicArn;
            return response;
        }

        try {
            // Update database
            const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
            const std::string topicArn = Core::AwsUtils::CreateSNSTopicArn(request.region, accountId, request.topicName);
            Database::Entity::SNS::Topic topic = {.region = request.region, .topicName = request.topicName, .owner = request.owner, .topicArn = topicArn};
            topic = _snsDatabase.CreateTopic(topic);
            log_trace << "SNS topic created: " << topic.ToString();

            Dto::SNS::CreateTopicResponse response;
            response.region = topic.region,
            response.topicName = topic.topicName,
            response.owner = topic.owner,
            response.topicArn = topic.topicArn;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SNS create topic failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SNS::ListTopicsResponse SNSService::ListTopics(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_topics");
        log_trace << "List all topics request, region: " << region;

        try {

            const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(region);
            // TODO: Write mapper
            Dto::SNS::ListTopicsResponse listTopicsResponse;
            for (const auto &it: topicList) {
                listTopicsResponse.topics.emplace_back(it.topicArn);
            }
            log_trace << "SNS list topics response: " << listTopicsResponse.ToXml();

            return listTopicsResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListTopicArnsResponse SNSService::ListTopicArns(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_topics");
        log_trace << "List all topic ARNs request, region: " << region;

        try {

            const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(region);

            Dto::SNS::ListTopicArnsResponse listTopicArnsResponse;
            for (const auto &it: topicList) {
                listTopicArnsResponse.topicArns.emplace_back(it.topicArn);
            }
            log_trace << "SNS list topic ARNs response: " << listTopicArnsResponse.ToJson();

            return listTopicArnsResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topic ARNs request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListTopicCountersResponse SNSService::ListTopicCounters(const Dto::SNS::ListTopicCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_topic_counters");
        log_trace << "List all topics counters request, request: " << request.ToString();

        try {

            const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns), request.region);
            Dto::SNS::ListTopicCountersResponse listTopicResponse = Dto::SNS::Mapper::map(request, topicList);
            listTopicResponse.total = _snsDatabase.CountTopics(request.region, request.prefix);
            log_trace << "SNS list topic counters response: " << listTopicResponse.ToJson();
            return listTopicResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetEventSourceResponse SNSService::GetEventSource(const Dto::SNS::GetEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "get_event_source");
        log_trace << "Get event source request, snsRequest: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.eventSourceArn)) {
            log_warning << "Topic does not exists, arn: " << request.eventSourceArn;
            throw Core::NotFoundException("Topic does not exists, arn: " + request.eventSourceArn);
        }

        try {
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.eventSourceArn);
            log_debug << "Topic returned, topic: " << topic.topicName;

            Dto::SNS::GetEventSourceResponse response;
            response.lambdaConfiguration.arn = topic.topicArn;
            response.lambdaConfiguration.enabled = true;
            response.lambdaConfiguration.uuid = topic.oid;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get event source failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    long SNSService::PurgeTopic(const Dto::SNS::PurgeTopicRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "purge_topic");
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
            return deleted;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS purge topic failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    long SNSService::PurgeAllTopics() const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "purge_all_topic");
        log_trace << "Purge all topics request";

        try {

            long deleted = 0;
            for (const auto &topic: _snsDatabase.ListTopics()) {
                Dto::SNS::PurgeTopicRequest request;
                request.topicArn = topic.topicArn;
                request.region = topic.region;
                deleted += PurgeTopic(request);
            }
            return deleted;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS purge topic failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::DeleteTopicResponse SNSService::DeleteTopic(const Dto::SNS::DeleteTopicRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "delete_topic");
        log_trace << "Delete topic request, region: " << request.region << " topicArn: " << request.topicArn;

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_warning << "Topic does not exist, arn: " << request.topicArn;
            throw Core::NotFoundException("Topic does not exist");
        }

        Dto::SNS::DeleteTopicResponse response;
        try {

            // Update database
            _snsDatabase.DeleteTopic({.region = request.region, .topicArn = request.topicArn});

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS delete topic failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        return response;
    }

    Dto::SNS::PublishResponse SNSService::Publish(const Dto::SNS::PublishRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "publish");
        log_trace << "Publish message request: " << request.ToString();

        // Check topic/target ARN
        if (request.topicArn.empty() && request.targetArn.empty()) {
            log_error << "Either topicArn or targetArn must exist";
            throw Core::ServiceException("Either topicArn or targetArn must exist");
        }

        // Check existence
        if (!request.topicArn.empty() && !_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }
        if (!request.targetArn.empty() && !_snsDatabase.TopicExists(request.targetArn)) {
            log_error << "SNS targetArn does not exists, targetArn: " << request.targetArn;
            throw Core::ServiceException("SNS targetArn does not exists, targetArn: " + request.targetArn);
        }

        try {
            // Get the topic by topic ARN or target ARN
            Database::Entity::SNS::Topic topic = !request.topicArn.empty() ? _snsDatabase.GetTopicByArn(request.topicArn) : _snsDatabase.GetTopicByTargetArn(request.targetArn);

            // Update database
            //message = Dto::SNS::Mapper::map(request, topic);
            Database::Entity::SNS::Message message = {.region = request.region,
                                                      .topicArn = request.topicArn,
                                                      .targetArn = request.targetArn,
                                                      .message = request.message,
                                                      .messageId = Core::AwsUtils::CreateMessageId(),
                                                      .contentType = request.contentType,
                                                      .size = static_cast<long>(request.message.length())};

            // Attributes
            if (!request.messageAttributes.empty()) {
                for (const auto &[fst, snd]: request.messageAttributes) {
                    const Database::Entity::SNS::MessageAttribute attribute = {.stringValue = snd.stringValue, .dataType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.dataType))};
                    message.messageAttributes[fst] = attribute;
                }
            }

            // Save message
            message = _snsDatabase.CreateMessage(message);

            CheckSubscriptions(request, topic, message);

            Dto::SNS::PublishResponse response;
            response.requestId = request.requestId;
            response.messageId = message.messageId;
            response.region = request.region;
            response.messageId = message.messageId;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS create message failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::SubscribeResponse SNSService::Subscribe(const Dto::SNS::SubscribeRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "subscribe");
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

            // Create a new subscription
            const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            const std::string subscriptionArn = Core::AwsUtils::CreateSNSSubscriptionArn(request.region, accountId, topic.topicName);

            if (const Database::Entity::SNS::Subscription subscription = {.protocol = request.protocol, .endpoint = request.endpoint}; !topic.HasSubscription(subscription)) {

                // Add subscription
                topic.subscriptions.push_back({.protocol = request.protocol, .endpoint = request.endpoint, .subscriptionArn = subscriptionArn});

                // Save to the database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }

            Dto::SNS::SubscribeResponse response;
            response.requestId = request.requestId;
            response.subscriptionArn = subscriptionArn;
            response.region = request.region;
            response.user = request.user;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UpdateSubscriptionResponse SNSService::UpdateSubscription(const Dto::SNS::UpdateSubscriptionRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "update_subscription");
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

            // Save to the database
            topic = _snsDatabase.UpdateTopic(topic);
            log_debug << "Subscription updated, topic: " << topic.ToString();

            Dto::SNS::UpdateSubscriptionResponse response;
            response.requestId = request.requestId;
            response.subscriptionArn = request.subscriptionArn;
            response.region = request.region;
            response.user = request.user;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UnsubscribeResponse SNSService::Unsubscribe(const Dto::SNS::UnsubscribeRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "unsubscribe");
        log_trace << "Unsubscribe request: " << request.ToString();

        try {

            // Check topic/target ARN
            if (request.subscriptionArn.empty()) {
                log_error << "Subscription ARN missing";
                throw Core::ServiceException("Subscription ARN missing");
            }

            // Create a new subscription
            for (Database::Entity::SNS::TopicList topics = _snsDatabase.GetTopicsBySubscriptionArn(request.subscriptionArn); auto &topic: topics) {

                // Remove subscription
                const auto count = std::erase_if(topic.subscriptions, [request](const auto &item) {
                    return item.subscriptionArn == request.subscriptionArn;
                });
                log_debug << "Subscription removed, count" << count;

                // Save to the database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }
            Dto::SNS::UnsubscribeResponse response;
            response.requestId = request.requestId;
            response.subscriptionArn = request.subscriptionArn;
            response.region = request.region;
            response.user = request.user;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListSubscriptionsByTopicResponse SNSService::ListSubscriptionsByTopic(const Dto::SNS::ListSubscriptionsByTopicRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_subscriptions");
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
                Dto::SNS::Subscription subscription;
                subscription.topicArn = request.topicArn;
                subscription.protocol = protocol;
                subscription.subscriptionArn = subscriptionArn;
                subscription.endpoint = endpoint;
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
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_subscription_counters");
        log_trace << "List subscription counters request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListSubscriptionCountersResponse response;
            response.total = static_cast<long>(topic.subscriptions.size());
            for (const auto &[protocol, endpoint, subscriptionArn]: topic.subscriptions) {
                const std::string id = subscriptionArn.substr(subscriptionArn.rfind(':') + 1);
                Dto::SNS::SubscriptionCounter subscription;
                subscription.id = id;
                subscription.topicArn = request.topicArn;
                subscription.protocol = protocol;
                subscription.subscriptionArn = subscriptionArn;
                subscription.endpoint = endpoint;
                response.subscriptionCounters.emplace_back(subscription);
            }
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get subscription counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListAttributeCountersResponse SNSService::ListAttributeCounters(const Dto::SNS::ListAttributeCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_attribute_counters");
        log_trace << "List topic attribute counters request: " << request.ToString();

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
            attributeCounter.attributeKey = "availableMessages";
            attributeCounter.attributeValue = std::to_string(topic.topicAttribute.availableMessages);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "archivePolicy";
            attributeCounter.attributeValue = topic.topicAttribute.archivePolicy;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "beginningArchiveTime";
            attributeCounter.attributeValue = Core::DateTimeUtils::ToISO8601(topic.topicAttribute.beginningArchiveTime);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "contentBasedDeduplication";
            attributeCounter.attributeValue = Core::StringUtils::ToString(topic.topicAttribute.contentBasedDeduplication);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "deliveryPolicy";
            attributeCounter.attributeValue = topic.topicAttribute.deliveryPolicy;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "displayName";
            attributeCounter.attributeValue = topic.topicAttribute.displayName;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "fifoTopic";
            attributeCounter.attributeValue = Core::StringUtils::ToString(topic.topicAttribute.fifoTopic);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "kmsMasterKeyId";
            attributeCounter.attributeValue = topic.topicAttribute.kmsMasterKeyId;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "policy";
            attributeCounter.attributeValue = topic.topicAttribute.policy;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "signatureVersion";
            attributeCounter.attributeValue = topic.topicAttribute.signatureVersion;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "tracingConfig";
            attributeCounter.attributeValue = topic.topicAttribute.tracingConfig;
            response.attributeCounters.emplace_back(attributeCounter);

            response.attributeCounters = Core::PageVector<Dto::SNS::AttributeCounter>(response.attributeCounters, request.pageSize, request.pageIndex);
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get attribute counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::ListTagCountersResponse SNSService::ListTagCounters(const Dto::SNS::ListTagCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_tag_counters");
        log_trace << "List tag counters request: " << request.ToString();

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "SNS topic does not exists, topicArn: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists, topicArn: " + request.topicArn);
        }

        try {

            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListTagCountersResponse response;
            response.total = static_cast<long>(topic.tags.size());
            for (const auto &[fst, snd]: topic.tags) {
                Dto::SNS::TagCounter tagCounter;
                tagCounter.tagKey = fst;
                tagCounter.tagValue = snd;
                response.tagCounters.emplace_back(tagCounter);
            }
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get tag counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetTopicAttributesResponse SNSService::GetTopicAttributes(const Dto::SNS::GetTopicAttributesRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "get_topic_attributes");
        log_trace << "Get topic attributes request: " << request.ToString();

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists");
            }

            const Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            Dto::SNS::GetTopicAttributesResponse response;
            response.region = topic.region;
            response.topicArn = topic.topicArn;
            response.owner = topic.owner;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get topic attributes failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetTopicDetailsResponse SNSService::GetTopicDetails(const Dto::SNS::GetTopicDetailsRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "get_topic_details");
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
            Dto::SNS::GetTopicDetailsResponse response;
            response.region = topic.region;
            response.topicName = topic.topicName;
            response.topicArn = topic.topicArn;
            response.messageCount = messageCount;
            response.size = messagesSize;
            response.owner = topic.owner;
            response.created = topic.created;
            response.modified = topic.modified;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS get topic attributes failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::GetMessageCountersResponse SNSService::GetMessageCounters(const Dto::SNS::GetMessageCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "get_message");
        log_trace << "Get message request, messageId: " << request.messageId;

        if (!_snsDatabase.MessageExists(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {

            Database::Entity::SNS::Message message = _snsDatabase.GetMessageByMessageId(request.messageId);
            log_debug << "Got message , messageId: " << request.messageId;
            Dto::SNS::GetMessageCountersResponse response;
            response.message = Dto::SNS::Mapper::map(message);
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SNSService::CheckSubscriptions(const Dto::SNS::PublishRequest &request, const Database::Entity::SNS::Topic &topic, const Database::Entity::SNS::Message &message) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "check_subscriptions");
        log_trace << "Check subscriptions request: " << request;

        if (topic.subscriptions.empty()) {
            return;
        }

        for (const auto &it: topic.subscriptions) {

            if (Core::StringUtils::ToLower(it.protocol) == SQS_PROTOCOL) {

                SendSQSMessage(it, request);
                log_debug << "Message send to SQS queue, queueArn: " << it.endpoint;

            } else if (Core::StringUtils::ToLower(it.protocol) == HTTP_PROTOCOL) {

                SendHttpMessage(it, request);
                log_debug << "Message send to HTTP endpoint, endpoint: " << it.endpoint;

            } else if (Core::StringUtils::ToLower(it.protocol) == LAMBDA_ENDPOINT) {

                SendLambdaMessage(it, request, message);
                log_debug << "Message send to HTTP endpoint, endpoint: " << it.endpoint;
            }
            _snsDatabase.SetMessageStatus(message, Database::Entity::SNS::SEND);
        }
    }

    Dto::SNS::TagResourceResponse SNSService::TagResource(const Dto::SNS::TagResourceRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "tag_topic");
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
            log_debug << "SNS tags updated, count: " << topic.tags.size();

            Dto::SNS::TagResourceResponse response;
            response.region = topic.region;
            response.user = request.user;
            response.requestId = request.requestId;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS tag resource failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SNS::UntagResourceResponse SNSService::UntagResource(const Dto::SNS::UntagResourceRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "untag_topic");
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
                count += static_cast<int>(std::erase_if(topic.tags, [it](const auto &item) {
                    auto const &[key, value] = item;
                    return key == it;
                }));
            }
            topic = _snsDatabase.UpdateTopic(topic);
            log_debug << "SNS tags updated, topicArn: " << topic.topicArn << " count: " << count;

            Dto::SNS::UntagResourceResponse response;
            response.region = topic.region;
            response.user = request.user;
            response.requestId = request.requestId;
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
        Dto::SNS::SqsNotificationRequest sqsNotificationRequest;
        sqsNotificationRequest.type = "Notification";
        sqsNotificationRequest.messageId = Core::AwsUtils::CreateMessageId();
        sqsNotificationRequest.topicArn = request.topicArn;
        sqsNotificationRequest.message = request.message;
        sqsNotificationRequest.timestamp = Core::DateTimeUtils::UnixTimestamp(system_clock::now());

        // Wrap it in a SQS message request
        Dto::SQS::SendMessageRequest sendMessageRequest;
        sendMessageRequest.region = request.region;
        sendMessageRequest.queueUrl = sqsQueue.queueUrl;
        sendMessageRequest.body = sqsNotificationRequest.ToJson();
        sendMessageRequest.contentType = "application/json";
        sendMessageRequest.requestId = Core::AwsUtils::CreateRequestId();

        for (const auto &[fst, snd]: request.messageAttributes) {
            Dto::SQS::MessageAttribute messageAttribute;
            messageAttribute.stringValue = snd.stringValue;
            messageAttribute.binaryValue = snd.binaryValue;
            messageAttribute.dataType = Dto::SQS::MessageAttributeDataTypeFromString(MessageAttributeDataTypeToString(snd.dataType));
            sendMessageRequest.messageAttributes[fst] = messageAttribute;
        }

        const Dto::SQS::SendMessageResponse response = _sqsService.SendMessage(sendMessageRequest);
        log_trace << "SNS SendMessage response: " << response.ToString();
    }

    void SNSService::SendHttpMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request) {
        namespace beast = boost::beast;
        namespace http = beast::http;
        namespace net = boost::asio;
        using tcp = boost::asio::ip::tcp;

        try {
            log_debug << "Sending HTTP message to: " << subscription.endpoint;

            // Parse URI (e.g., http://host:port/path)
            const std::string &url = subscription.endpoint;
            const std::string protocol_prefix = "http://";
            if (!Core::StringUtils::StartsWith(url, protocol_prefix)) {
                log_warning << "Unsupported protocol in endpoint: " << url;
                return;
            }

            std::string host_port_path = url.substr(protocol_prefix.size());
            auto path_pos = host_port_path.find('/');
            std::string host_port = path_pos == std::string::npos ? host_port_path : host_port_path.substr(0, path_pos);
            std::string path = path_pos == std::string::npos ? "/" : host_port_path.substr(path_pos);

            std::string host;
            std::string port = "80";
            if (auto colon_pos = host_port.find(':'); colon_pos != std::string::npos) {
                host = host_port.substr(0, colon_pos);
                port = host_port.substr(colon_pos + 1);
            } else {
                host = host_port;
            }

            net::io_context ioc;
            tcp::resolver resolver(ioc);
            beast::tcp_stream stream(ioc);

            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            http::request<http::string_body> req{http::verb::post, path, 11};
            req.set(http::field::host, host);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set(http::field::content_type, "application/json");

            // Basic payload
            const std::string body = R"({"Type":"Notification","Message":")" + request.message + R"("})";
            req.body() = body;
            req.prepare_payload();

            http::write(stream, req);

            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            log_debug << "HTTP Response: " << res.result_int() << " - " << res.body();

            beast::error_code ec;
            ec = stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        } catch (const std::exception &ex) {
            log_error << "Failed to send HTTP message to: " << subscription.endpoint << ", error: " << ex.what();
        }
    }

    void SNSService::SendLambdaMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request, const Database::Entity::SNS::Message &message) const {

        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(subscription.endpoint);
        log_debug << "Found lambda, lambdaArn: " << lambda.arn;
        SendLambdaInvocationRequest(lambda, message, request.topicArn);
    }

    void SNSService::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const Database::Entity::SNS::Message &message, const std::string &eventSourceArn) const {
        log_debug << "Invoke lambda function request, function: " << lambda.function;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");

        // Create the event record
        Dto::SNS::Record record;
        record.region = lambda.region;
        record.messageId = message.messageId;
        record.receiptHandle = Core::AwsUtils::CreateSnsReceiptHandler();
        record.body = message.message;
        record.messageAttributes = Dto::SNS::Mapper::map(message.messageAttributes);
        record.md5Sum = Database::SqsUtils::CreateMd5OfMessageBody(message.message);
        record.eventSource = "aws:sns";
        record.eventSourceArn = eventSourceArn;

        Dto::SNS::EventNotification eventNotification;
        eventNotification.records.emplace_back(record);
        log_debug << "Invocation request function name: " << lambda.function << " json: " << eventNotification.ToJson();

        std::string payload = eventNotification.ToJson();
        Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, lambda.function, payload, Dto::Lambda::LambdaInvocationType::EVENT);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function << " sourceArn: " << eventSourceArn;
    }

    Dto::SNS::ListMessagesResponse SNSService::ListMessages(const Dto::SNS::ListMessagesRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_messages");
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
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "list_message_counters");
        log_trace << "List message counters request, region: " << request.region << " topicArn: " << request.topicArn;

        try {

            const Database::Entity::SNS::MessageList messageList = _snsDatabase.ListMessages(request.topicArn, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            Dto::SNS::ListMessageCountersResponse listMessageCountersResponse = Dto::SNS::Mapper::map(request, messageList);
            listMessageCountersResponse.total = _snsDatabase.CountMessages(request.topicArn);
            log_trace << "SNS list messages, response: " << listMessageCountersResponse.ToJson();

            return listMessageCountersResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SNSService::DeleteMessage(const Dto::SNS::DeleteMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SNS_SERVICE_COUNTER, "action", "delete_message");
        log_trace << "Delete a message request, messageId: " << request.messageId;

        if (!_snsDatabase.MessageExists(request.messageId)) {
            log_error << "SNS message does not exists, messageId: " << request.messageId;
            throw Core::ServiceException("SNS message does not exists, messageId: " + request.messageId);
        }

        try {

            _snsDatabase.DeleteMessage(request.messageId);
            log_trace << "SNS message deleted, messageId: " << request.messageId;

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            log_trace << "SNS topic counter adjusted, topicArn: " << request.topicArn;

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
