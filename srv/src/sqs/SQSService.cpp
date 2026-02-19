//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/sqs/SQSService.h>
#include <boost/container/map.hpp>

namespace AwsMock::Service {

    boost::mutex SQSService::_subscriptionMutex;

    Dto::SQS::CreateQueueResponse SQSService::CreateQueue(const Dto::SQS::CreateQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "create_queue");
        log_trace << "Create queue request, region: " << request.region << " name: " << request.queueName;

        // Get queue ARN
        const std::string queueArn = Core::CreateSQSQueueArn(request.region, request.queueName);

        // Check existence. In case the queue exists already return the existing queue.
        if (_sqsDatabase.QueueArnExists(queueArn)) {
            log_warning << "Queue exists already, region: " << request.region << " queueUrl: " << request.queueUrl;
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(queueArn);
            Dto::SQS::CreateQueueResponse response;
            response.region = queue.region;
            response.queueName = queue.name;
            response.owner = queue.owner;
            response.queueUrl = queue.queueUrl;
            response.queueArn = queue.queueArn;
            return response;
        }

        try {
            // Get queue URL
            const std::string queueUrl = Core::CreateSQSQueueUrl(request.queueName);

            Database::Entity::SQS::QueueAttribute attributes;
            attributes.approximateNumberOfMessages = 0;
            if (request.attributes.contains("DelaySeconds")) {
                attributes.delaySeconds = std::stoi(request.attributes.at("DelaySeconds"));
            }
            if (request.attributes.contains("MaximumMessageSize")) {
                attributes.maxMessageSize = std::stoi(request.attributes.at("MaximumMessageSize"));
            }
            if (request.attributes.contains("MessageRetentionPeriod")) {
                attributes.messageRetentionPeriod = std::stoi(request.attributes.at("MessageRetentionPeriod"));
            }
            if (request.attributes.contains("VisibilityTimeout")) {
                attributes.visibilityTimeout = std::stoi(request.attributes.at("VisibilityTimeout"));
            }
            if (request.attributes.contains("ReceiveMessageWaitTimeSeconds")) {
                attributes.receiveMessageWaitTime = std::stoi(request.attributes.at("ReceiveMessageWaitTimeSeconds"));
            }
            if (request.attributes.contains("Policy")) {
                attributes.policy = request.attributes.at("Policy");
            }
            // TODO: deserialize redrive policy
            if (request.attributes.contains("RedrivePolicy")) {
                //attributes.redrivePolicy = boost::json::value_to<Database::Dto::SQS::RedriveMessagesRequest>(request.attributes.at("RedrivePolicy"));
            }
            if (request.attributes.contains("RedriveAllowPolicy")) {
                attributes.redriveAllowPolicy = request.attributes.at("RedriveAllowPolicy");
            }
            if (request.attributes.contains("RedriveAllowPolicy")) {
                attributes.redriveAllowPolicy = request.attributes.at("RedriveAllowPolicy");
            }

            if (request.attributes.contains("QueueArn")) {
                attributes.queueArn = request.attributes.at("QueueArn");
            }

            // TODO:: Use mapper
            // Update database
            Database::Entity::SQS::Queue queue;
            queue.region = request.region;
            queue.name = request.queueName;
            queue.owner = request.owner;
            queue.queueUrl = queueUrl;
            queue.queueArn = queueArn;
            queue.attributes = attributes;
            queue.tags = request.tags;
            queue = _sqsDatabase.CreateQueue(queue);
            log_trace << "SQS queue created: " << Core::Bson::BsonUtils::ToJsonString(queue.ToDocument());

            // TODO:: Use mapper
            Dto::SQS::CreateQueueResponse response;
            response.region = queue.region;
            response.queueName = queue.name;
            response.queueUrl = queue.queueUrl;
            response.queueArn = queue.queueArn;
            response.owner = queue.owner;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueuesResponse SQSService::ListQueues(const Dto::SQS::ListQueuesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_queues");
        log_trace << "List all queues request, region: " << request.region;

        try {
            Dto::SQS::ListQueuesResponse listQueueResponse;
            listQueueResponse.total = _sqsDatabase.CountQueues(request.region);
            if (request.maxResults > 0) {
                // Get the total number
                const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.queueNamePrefix, request.maxResults, 0, {}, request.region);
                const std::string nextToken = static_cast<long>(queueList.size()) > 0 ? queueList.back().oid : "";
                listQueueResponse.queueUrls = Dto::SQS::Mapper::mapUrls(queueList);
                listQueueResponse.nextToken = nextToken;
                log_trace << "SQS create queue list response: " << listQueueResponse.ToJson();
                return listQueueResponse;
            }
            const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.region);
            listQueueResponse.queueUrls = Dto::SQS::Mapper::mapUrls(queueList);

            log_trace << "SQS create queue list response: " << listQueueResponse.ToJson();
            return listQueueResponse;
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueueArnsResponse SQSService::ListQueueArns() const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_queue_arns");
        log_trace << "List all queues ARNs request";

        try {
            const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
            Dto::SQS::ListQueueArnsResponse listQueueResponse;
            for (const auto &queue: queueList) {
                listQueueResponse.queueArns.emplace_back(queue.queueArn);
            }
            log_trace << "SQS create queue ARN list response: " << listQueueResponse.ToJson();
            return listQueueResponse;
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueueCountersResponse SQSService::ListQueueCounters(const Dto::SQS::ListQueueCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_queue_counters");
        log_trace << "List all queues counters request";

        try {

            // Adjust SQS queue counters
            //            _sqsDatabase.AdjustMessageCounters();

            // Get the queue list and the total number
            const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns), request.region);
            const long total = _sqsDatabase.CountQueues(request.prefix, request.region);

            log_trace << "SQS create queue counters list, count: " << total;
            return Dto::SQS::Mapper::map(queueList, total);

        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::GetQueueDetailsResponse SQSService::GetQueueDetails(const Dto::SQS::GetQueueDetailsRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "get_queue_details");
        log_trace << "Get queue details request, queueArn: " << request.queueArn;

        // Check existence. In case the queue exists already return the existing queue.
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_warning << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        try {
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            Dto::SQS::GetQueueDetailsResponse sqsResponse;
            sqsResponse.messageCount = _sqsDatabase.CountMessages(request.queueArn);
            sqsResponse.queueName = queue.name;
            sqsResponse.queueArn = queue.queueArn;
            sqsResponse.queueUrl = queue.queueUrl;
            sqsResponse.region = queue.region;
            sqsResponse.retentionPeriod = queue.attributes.messageRetentionPeriod;
            sqsResponse.maxMessageSize = queue.attributes.maxMessageSize;
            sqsResponse.visibilityTimeout = queue.attributes.visibilityTimeout;
            sqsResponse.delay = queue.attributes.delaySeconds;
            sqsResponse.owner = queue.owner;
            sqsResponse.size = queue.size;
            sqsResponse.available = queue.attributes.approximateNumberOfMessages;
            sqsResponse.invisible = queue.attributes.approximateNumberOfMessagesNotVisible;
            sqsResponse.delayed = queue.attributes.approximateNumberOfMessagesDelayed;
            sqsResponse.dlqArn = queue.attributes.redrivePolicy.deadLetterTargetArn;
            sqsResponse.dlqMaxReceive = queue.attributes.redrivePolicy.maxReceiveCount;
            sqsResponse.created = queue.created;
            sqsResponse.modified = queue.modified;

            log_trace << "SQS get queue details response: " << sqsResponse.ToJson();
            return sqsResponse;
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueueTagsResponse SQSService::ListQueueTags(const Dto::SQS::ListQueueTagsRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_queue_tags");
        log_trace << "List all queues tags request";

        try {
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            Dto::SQS::ListQueueTagsResponse listQueueTagsResponse;
            listQueueTagsResponse.total = static_cast<long>(queue.tags.size());
            listQueueTagsResponse.tags = queue.tags;
            log_trace << "SQS create queue tags list response: " << listQueueTagsResponse.ToJson();
            return listQueueTagsResponse;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListQueueAttributeCountersResponse SQSService::ListQueueAttributeCounters(
        const Dto::SQS::ListQueueAttributeCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_queue_attribute_counters");
        log_trace << "List queue attribute counters request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "SQS queue does not exists, queueArn: " << request.queueArn;
            throw Core::ServiceException("SQS queue does not exists, queueArn: " + request.queueArn);
        }

        try {
            int total = 13;

            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            Dto::SQS::ListQueueAttributeCountersResponse response;
            response.total = total;
            Dto::SQS::AttributeCounter attributeCounter;
            attributeCounter.attributeKey = "approximateNumberOfMessages";
            attributeCounter.attributeValue = std::to_string(queue.attributes.approximateNumberOfMessages);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "approximateNumberOfMessagesDelayed";
            attributeCounter.attributeValue = std::to_string(queue.attributes.approximateNumberOfMessagesDelayed);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "approximateNumberOfMessagesNotVisible",
                    attributeCounter.attributeValue = std::to_string(queue.attributes.approximateNumberOfMessagesNotVisible);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "deadLetterTargetArn";
            attributeCounter.attributeValue = queue.attributes.redrivePolicy.deadLetterTargetArn;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "delaySeconds";
            attributeCounter.attributeValue = std::to_string(queue.attributes.delaySeconds);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "maxMessageSize";
            attributeCounter.attributeValue = std::to_string(queue.attributes.maxMessageSize);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "messageRetentionPeriod";
            attributeCounter.attributeValue = std::to_string(queue.attributes.messageRetentionPeriod);
            response.attributeCounters.emplace_back(attributeCounter);
            attributeCounter.attributeKey = "policy";
            attributeCounter.attributeValue = queue.attributes.policy;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "maxReceiveCount";
            attributeCounter.attributeValue = std::to_string(queue.attributes.redrivePolicy.maxReceiveCount);
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "queueArn";
            attributeCounter.attributeValue = queue.attributes.queueArn;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "redriveAllowPolicy";
            attributeCounter.attributeValue = queue.attributes.redriveAllowPolicy;
            response.attributeCounters.emplace_back(attributeCounter);

            attributeCounter.attributeKey = "visibilityTimeout";
            attributeCounter.attributeValue = std::to_string(queue.attributes.visibilityTimeout);
            response.attributeCounters.emplace_back(attributeCounter);

            if (request.pageSize > 0) {
                auto endArray = response.attributeCounters.begin() + request.pageSize * (request.pageIndex + 1);
                if (request.pageSize * (request.pageIndex + 1) > total) {
                    endArray = response.attributeCounters.end();
                }
                response.attributeCounters = std::vector(
                    response.attributeCounters.begin() + request.pageSize * request.pageIndex,
                    endArray);
            }
            return response;
        } catch (Core::DatabaseException &ex) {
            log_error << "SQS get attribute counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SQS::ListLambdaTriggerCountersResponse SQSService::ListLambdaTriggerCounters(const Dto::SQS::ListLambdaTriggerCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_lambda_trigger_counters");
        log_trace << "List lambda trigger counters request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "SQS queue does not exists, queueArn: " << request.queueArn;
            throw Core::ServiceException("SQS queue does not exists, queueArn: " + request.queueArn);
        }

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdas = Database::LambdaDatabase::instance().ListLambdasWithEventSource(request.queueArn);

            Dto::SQS::ListLambdaTriggerCountersResponse response;
            response.total = static_cast<long>(lambdas.size());
            std::string queueArn = request.queueArn;
            for (const auto &lambda: lambdas) {
                if (lambda.HasEventSource(request.queueArn)) {
                    const Database::Entity::Lambda::EventSourceMapping eventSourceMapping = lambda.GetEventSource(request.queueArn);
                    Dto::SQS::LambdaTriggerCounter triggerCounter;
                    triggerCounter.uuid = eventSourceMapping.uuid;
                    triggerCounter.arn = lambda.arn;
                    triggerCounter.enabled = eventSourceMapping.enabled;
                    response.lambdaTriggerCounters.emplace_back(triggerCounter);
                }
            }
            return response;
        } catch (Core::DatabaseException &ex) {
            log_error << "SQS get lambda trigger counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::SQS::ListQueueTagCountersResponse SQSService::ListTagCounters(const Dto::SQS::ListQueueTagCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SNS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_tag_counters");
        log_trace << "List tag counters request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "SQS queue does not exists, queueArn: " << request.queueArn;
            throw Core::ServiceException("SQS queue does not exists, queueArn: " + request.queueArn);
        }

        try {
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            Dto::SQS::ListQueueTagCountersResponse response;
            response.total = static_cast<long>(queue.tags.size());
            for (const auto &[fst, snd]: queue.tags) {
                Dto::SQS::TagCounter tagCounter;
                tagCounter.tagKey = fst;
                tagCounter.tagValue = snd;
                response.tagCounters.emplace_back(tagCounter);
            }
            return response;
        } catch (Core::DatabaseException &ex) {
            log_error << "SQS get tag counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    long SQSService::PurgeQueue(const Dto::SQS::PurgeQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "purge_queue");
        log_trace << "Purge queue request, region: " << request.region << " queueUrl: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {
            // Update messages
            const std::string queueArn = Core::AwsUtils::ConvertSQSQueueUrlToArn(request.region, request.queueUrl);
            const long deleted = _sqsDatabase.PurgeQueue(queueArn);
            log_trace << "SQS queue purged, queueArn: " << queueArn;

            // Update queue counter
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);
            queue.attributes.approximateNumberOfMessages = 0;
            queue.attributes.approximateNumberOfMessagesDelayed = 0;
            queue.attributes.approximateNumberOfMessagesNotVisible = 0;
            queue = _sqsDatabase.UpdateQueue(queue);
            log_trace << "SQS queue counter updated, queueArn: " << queue.queueArn;
            return deleted;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    long SQSService::PurgeAllQueues() const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "purge_all_queue");

        try {
            long deleted = 0;
            for (const auto &queue: _sqsDatabase.ListQueues()) {
                Dto::SQS::PurgeQueueRequest request;
                request.region = queue.region;
                request.queueUrl = queue.queueUrl;
                deleted += PurgeQueue(request);
            }
            return deleted;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::RedriveMessage(const Dto::SQS::RedriveMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "redrive_message");
        log_trace << "Redrive message request, queueArn: " << request.queueArn;

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        try {
            // Update message
            const Database::Entity::SQS::Queue originalQueue = _sqsDatabase.GetQueueByDlq(request.queueArn);
            const Database::Entity::SQS::Queue dqlQueue = _sqsDatabase.GetQueueByArn(request.queueArn);
            _sqsDatabase.RedriveMessage(originalQueue, dqlQueue, request.messageId);
            log_debug << "SQS redrive message, queueArn: " << request.queueArn;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    long SQSService::RedriveMessages(const Dto::SQS::RedriveMessagesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "redrive_messages");
        log_trace << "Redrive messages request, queueArn: " << request.queueArn;

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        try {
            // Update messages
            const Database::Entity::SQS::Queue originalQueue = _sqsDatabase.GetQueueByDlq(request.queueArn);
            const Database::Entity::SQS::Queue dqlQueue = _sqsDatabase.GetQueueByArn(request.queueArn);
            const long count = _sqsDatabase.RedriveMessages(originalQueue, dqlQueue);
            log_debug << "SQS redrive messages, queueArn: " << request.queueArn << " count: " << count;

            // Update monitoring counter
            _sqsDatabase.AdjustMessageCounters();

            return count;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetQueueUrlResponse SQSService::GetQueueUrl(const Dto::SQS::GetQueueUrlRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "get_queue_url");
        log_debug << "Get queue URL request, region: " << request.region << " queueName: " << request.queueName;

        const std::string queueUrl = Core::SanitizeSQSUrl(request.queueName);

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueName: " << request.queueName << " queueUrl: " << queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueName: " + request.queueName + " queueUrl: " + queueUrl);
        }

        try {

            // Get queue
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, queueUrl);
            log_debug << "SQS get queue URL, region: " << request.region << " queueName: " << queue.queueUrl;
            Dto::SQS::GetQueueUrlResponse response;
            response.queueUrl = queue.queueUrl;
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetQueueAttributesResponse SQSService::GetQueueAttributes(const Dto::SQS::GetQueueAttributesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "get_queue_attributes");
        log_trace << "Get queue userAttributes request, request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);
            log_debug << "Got queue: " << queue.queueUrl;

            Dto::SQS::GetQueueAttributesResponse response;
            if (CheckAttribute(request.attributeNames, "all")) {
                response.attributes.emplace_back("ApproximateNumberOfMessages", std::to_string(queue.attributes.approximateNumberOfMessages));
                response.attributes.emplace_back("ApproximateNumberOfMessagesDelayed", std::to_string(queue.attributes.approximateNumberOfMessagesDelayed));
                response.attributes.emplace_back("ApproximateNumberOfMessagesNotVisible", std::to_string(queue.attributes.approximateNumberOfMessagesNotVisible));
                response.attributes.emplace_back("CreatedTimestamp", Core::DateTimeUtils::HttpFormat(queue.created));
                response.attributes.emplace_back("DelaySeconds", std::to_string(queue.attributes.delaySeconds));
                response.attributes.emplace_back("LastModifiedTimestamp", Core::DateTimeUtils::HttpFormat(queue.modified));
                response.attributes.emplace_back("MaximumMessageSize", std::to_string(queue.attributes.maxMessageSize));
                response.attributes.emplace_back("MessageRetentionPeriod", std::to_string(queue.attributes.messageRetentionPeriod));
                response.attributes.emplace_back("Policy", queue.attributes.policy);
                response.attributes.emplace_back("QueueArn", queue.queueArn);
                response.attributes.emplace_back("ReceiveMessageWaitTimeSeconds", std::to_string(queue.attributes.receiveMessageWaitTime));
                response.attributes.emplace_back("VisibilityTimeout", std::to_string(queue.attributes.visibilityTimeout));
            } else {
                if (CheckAttribute(request.attributeNames, "Policy")) {
                    response.attributes.emplace_back("Policy", queue.attributes.policy);
                }
                if (CheckAttribute(request.attributeNames, "VisibilityTimeout")) {
                    response.attributes.emplace_back("VisibilityTimeout", std::to_string(queue.attributes.visibilityTimeout));
                }
                if (CheckAttribute(request.attributeNames, "MaximumMessageSize")) {
                    response.attributes.emplace_back("MaximumMessageSize", std::to_string(queue.attributes.maxMessageSize));
                }
                if (CheckAttribute(request.attributeNames, "MessageRetentionPeriod")) {
                    response.attributes.emplace_back("MessageRetentionPeriod", std::to_string(queue.attributes.messageRetentionPeriod));
                }
                if (CheckAttribute(request.attributeNames, "ApproximateNumberOfMessages")) {
                    response.attributes.emplace_back("ApproximateNumberOfMessages", std::to_string(queue.attributes.approximateNumberOfMessages));
                }
                if (CheckAttribute(request.attributeNames, "ApproximateNumberOfMessagesNotVisible")) {
                    response.attributes.emplace_back("ApproximateNumberOfMessagesNotVisible", std::to_string(queue.attributes.approximateNumberOfMessagesNotVisible));
                }
                if (CheckAttribute(request.attributeNames, "ApproximateNumberOfMessagesDelayed")) {
                    response.attributes.emplace_back("ApproximateNumberOfMessagesDelayed", std::to_string(queue.attributes.approximateNumberOfMessagesDelayed));
                }
                if (CheckAttribute(request.attributeNames, "CreatedTimestamp")) {
                    response.attributes.emplace_back("CreatedTimestamp", Core::DateTimeUtils::HttpFormat(queue.created));
                }
                if (CheckAttribute(request.attributeNames, "LastModifiedTimestamp")) {
                    response.attributes.emplace_back("LastModifiedTimestamp", Core::DateTimeUtils::HttpFormat(queue.modified));
                }
                if (CheckAttribute(request.attributeNames, "DelaySeconds")) {
                    response.attributes.emplace_back("DelaySeconds", std::to_string(queue.attributes.delaySeconds));
                }
                if (CheckAttribute(request.attributeNames, "ReceiveMessageWaitTimeSeconds")) {
                    response.attributes.emplace_back("ReceiveMessageWaitTimeSeconds", std::to_string(queue.attributes.receiveMessageWaitTime));
                }
                if (CheckAttribute(request.attributeNames, "RedrivePolicy")) {
                    response.attributes.emplace_back("RedrivePolicy", queue.attributes.redrivePolicy.ToJson());
                }
                if (CheckAttribute(request.attributeNames, "QueueArn")) {
                    response.attributes.emplace_back("QueueArn", queue.queueArn);
                }
            }
            return response;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::SetQueueAttributes(const Dto::SQS::SetQueueAttributesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "set_queue_attributes");
        log_trace << "Put queue sqs request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException(
                "Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {
            // Get the queue
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);
            log_trace << "Got queue: " << Core::Bson::BsonUtils::ToJsonString(queue.ToDocument());

            // Reset all userAttributes
            std::map<std::string, std::string> attributes = request.attributes;
            if (!attributes["Policy"].empty()) {
                queue.attributes.policy = attributes["Policy"];
            }
            if (!attributes["RedrivePolicy"].empty()) {
                queue.attributes.redrivePolicy.FromJson(attributes["RedrivePolicy"]);
            }
            if (!attributes["RedriveAllowPolicy"].empty()) {
                queue.attributes.redriveAllowPolicy = attributes["RedriveAllowPolicy"];
            }
            if (!attributes["MessageRetentionPeriod"].empty()) {
                queue.attributes.messageRetentionPeriod = std::stoi(attributes["MessageRetentionPeriod"]);
            }
            if (!attributes["VisibilityTimeout"].empty()) {
                queue.attributes.visibilityTimeout = std::stoi(attributes["VisibilityTimeout"]);
            }
            if (!attributes["QueueArn"].empty()) {
                queue.attributes.queueArn = attributes["QueueArn"];
            } else {
                queue.attributes.queueArn = queue.queueArn;
            }

            // Update database
            queue = _sqsDatabase.UpdateQueue(queue);
            log_trace << "Queue updated: " << Core::Bson::BsonUtils::ToJsonString(queue.ToDocument());
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetEventSourceResponse SQSService::GetEventSource(const Dto::SQS::GetEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "get_event_source");
        log_trace << "Get event source request, sqsRequest: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.eventSourceArn)) {
            log_warning << "Queue does not exists, arn: " << request.eventSourceArn;
            throw Core::NotFoundException("Queue does not exists, arn: " + request.eventSourceArn);
        }

        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.eventSourceArn);
            log_debug << "Queue returned, queue: " << queue.name;

            Dto::SQS::GetEventSourceResponse response;
            response.lambdaConfiguration.arn = queue.queueArn;
            response.lambdaConfiguration.enabled = true;
            response.lambdaConfiguration.uuid = queue.oid;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get event source failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void SQSService::SetMessageVisibilityTimeout(const Dto::SQS::ChangeMessageVisibilityRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "set_message_visibility_timeout");
        log_trace << "Change message visibilityTimeout request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.MessageExists(request.receiptHandle)) {
            log_error << "Message does not exist, receiptHandle: " << request.receiptHandle;
            throw Core::ServiceException("Message does not exist, receiptHandle: " + request.receiptHandle);
        }

        try {
            // Get the message
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByReceiptHandle(request.receiptHandle);
            log_trace << "Got message: " << Core::Bson::BsonUtils::ToJsonString(message.ToDocument());

            long visibilityTimeout = request.visibilityTimeout;
            if (visibilityTimeout < 10) {
                visibilityTimeout = 30;
            }
            if (request.visibilityTimeout > 43200) {
                visibilityTimeout = 43200;
            }

            // Set as attribute
            message.attributes["VisibilityTimeout"] = std::to_string(visibilityTimeout);
            message.reset = system_clock::now() + std::chrono::seconds(visibilityTimeout);

            // Update database
            message = _sqsDatabase.UpdateMessage(message);
            log_trace << "Message updated, queue: " << Core::AwsUtils::ConvertSQSQueueUrlToName(request.queueUrl) << ", receiptHandle: " << message.receiptHandle;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::TagQueue(const Dto::SQS::TagQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "tag_queue");
        log_trace << "Tag queue request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, queueUrl: " << request.queueUrl;
            throw Core::ServiceException("SQS queue does not exists, queueUrl: " + request.queueUrl);
        }

        try {
            // Get the topic
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            // Set tags and update database
            for (const auto &[fst, snd]: request.tags) {
                queue.tags[fst] = snd;
            }

            queue = _sqsDatabase.UpdateQueue(queue);
            log_debug << "SQS queue tags updated, count: " << request.tags.size() << " queue: " << queue.name;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::UntagQueue(const Dto::SQS::UntagQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "untag_queue");
        log_trace << "Untag queue request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, queueUrl: " << request.queueUrl;
            throw Core::ServiceException("SQS queue topic does not exists");
        }

        if (request.tags.empty()) {
            log_warning << "Empty tags array, queueUrl: " << request.queueUrl;
            return;
        }

        try {
            // Get the topic
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            // Set tags and update database
            int count = 0;
            if (!request.tags.empty()) {
                for (const auto &tag: request.tags) {
                    count += static_cast<int>(std::erase_if(queue.tags, [tag](const auto &item) {
                        auto const &[k, v] = item;
                        return k == tag;
                    }));
                }
            }
            queue = _sqsDatabase.UpdateQueue(queue);
            log_debug << "SQS queue tags deleted, count: " << count << " queue: " << queue.name;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListDefaultMessageAttributeCountersResponse SQSService::ListDefaultMessageAttributeCounters(const Dto::SQS::ListDefaultMessageAttributeCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_default_message_attribute_counters");
        log_trace << "List message counters request";

        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        try {
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            Dto::SQS::ListDefaultMessageAttributeCountersResponse response;
            response.total = static_cast<long>(queue.defaultMessageAttributes.size());
            response.attributeCounters = Dto::SQS::Mapper::map(queue.defaultMessageAttributes);
            response.attributeCounters = Core::PageMap<std::string, Dto::SQS::MessageAttribute>(response.attributeCounters, request.pageSize, request.pageIndex);
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListDefaultMessageAttributeCountersResponse SQSService::AddDefaultMessageAttribute(const Dto::SQS::AddDefaultMessageAttributeRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "add_default_message_attribute");
        log_trace << "Add default message attribute request, queueArn: " << request.queueArn;

        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        Dto::SQS::ListDefaultMessageAttributeCountersResponse response;
        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            // Add attributes
            Database::Entity::SQS::MessageAttribute messageAttribute;
            messageAttribute.stringValue = request.messageAttribute.stringValue;
            messageAttribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(Dto::SQS::MessageAttributeDataTypeToString(request.messageAttribute.dataType));
            queue.defaultMessageAttributes[request.name] = messageAttribute;
            queue = _sqsDatabase.UpdateQueue(queue);

            response.total = static_cast<long>(queue.defaultMessageAttributes.size());
            response.attributeCounters = Dto::SQS::Mapper::map(queue.defaultMessageAttributes);

            log_debug << "Default message attribute added, queueArn: " << queue.queueArn << ", name: " << request.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
        return response;
    }

    Dto::SQS::ListDefaultMessageAttributeCountersResponse SQSService::UpdateDefaultMessageAttribute(const Dto::SQS::UpdateDefaultMessageAttributeRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "update_default_message_attribute");
        log_trace << "Update default message attribute request, queueArn: " << request.queueArn;

        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        Dto::SQS::ListDefaultMessageAttributeCountersResponse response;
        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            // Delete a default message attributes
            queue.defaultMessageAttributes[request.name].stringValue = request.value;
            queue.defaultMessageAttributes[request.name].dataType = Database::Entity::SQS::MessageAttributeTypeFromString(request.dataType);
            queue = _sqsDatabase.UpdateQueue(queue);

            response.total = static_cast<long>(queue.defaultMessageAttributes.size());
            response.attributeCounters = Dto::SQS::Mapper::map(queue.defaultMessageAttributes);

            log_debug << "Default message attribute updated, queueArn: " << queue.queueArn << ", name: " << request.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
        return response;
    }

    Dto::SQS::ListDefaultMessageAttributeCountersResponse SQSService::DeleteDefaultMessageAttribute(const Dto::SQS::DeleteDefaultMessageAttributeRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "delete_default_message_attribute");
        log_trace << "Delete default message attribute request, queueArn: " << request.queueArn;

        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        Dto::SQS::ListDefaultMessageAttributeCountersResponse response;
        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            // Delete a default message attributes
            queue.defaultMessageAttributes.erase(request.name);
            queue = _sqsDatabase.UpdateQueue(queue);

            response.total = static_cast<long>(queue.defaultMessageAttributes.size());
            response.attributeCounters = Dto::SQS::Mapper::map(queue.defaultMessageAttributes);

            log_debug << "Default message attribute deleted, queueArn: " << queue.queueArn << ", name: " << request.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
        return response;
    }

    void SQSService::UpdateDql(const Dto::SQS::UpdateDqlRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "update_dlq");
        log_trace << "Update DQL subscription request, queueArn: " << request.queueArn;

        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + request.queueArn);
        }

        if (!_sqsDatabase.QueueArnExists(request.targetArn)) {
            log_error << "Target queue does not exist, targetArn: " << request.targetArn;
            throw Core::ServiceException("Target queue does not exist, targetArn: " + request.targetArn);
        }

        if (request.retries < 1) {
            log_error << "Retries must be >=1, queueArn: " << request.queueArn;
            throw Core::ServiceException("Retries must be >=1, , queueArn: " + request.queueArn);
        }

        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);
            queue.attributes.redrivePolicy.deadLetterTargetArn = request.targetArn;
            queue.attributes.redrivePolicy.maxReceiveCount = request.retries;

            // Update queue
            queue = _sqsDatabase.UpdateQueue(queue);
            log_debug << "Queue DQL subscription updated, queueArn: " << request.queueArn;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::ReloadCounters(const Dto::SQS::ReloadCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "reload_counters");
        log_trace << "Reload queue counters request, request: " << request;

        // Check existence
        if (!request.queueArn.empty() && !_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, region: " << request.region << " queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueArn: " + request.queueArn);
        }

        try {
            // Delete all resources in queue
            _sqsDatabase.AdjustMessageCounters();
            log_debug << "Count messages, queueArn: " << request.queueArn;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::ReloadAllCounters() const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "reload_all_counters");
        log_trace << "Reload all counters";

        try {
            _sqsDatabase.AdjustMessageCounters();

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::UpdateQueue(const Dto::SQS::UpdateQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "update_queue");
        log_trace << "Update queue request, request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueArnExists(request.queueArn)) {
            log_error << "Queue does not exist, region: " << request.region << " queueArn: " << request.queueArn;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueArn: " + request.queueArn);
        }

        try {

            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);
            queue.attributes.delaySeconds = request.delay;
            queue.attributes.visibilityTimeout = request.visibilityTimeout;
            queue.attributes.messageRetentionPeriod = request.retentionPeriod;
            queue = _sqsDatabase.UpdateQueue(queue);
            log_debug << "Queue updated, queueArn: " << request.queueArn;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::DeleteQueueResponse SQSService::DeleteQueue(const Dto::SQS::DeleteQueueRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "delete_queue");
        log_trace << "Delete queue request, request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException(
                "Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {
            // Delete all resources in queue
            long deleted = _sqsDatabase.DeleteMessages(request.queueUrl);
            log_debug << "Messages deleted, queue: " << request.queueUrl << " count:" << deleted;

            // Update database
            Database::Entity::SQS::Queue queue;
            queue.region = request.region;
            queue.queueUrl = request.queueUrl;
            deleted = _sqsDatabase.DeleteQueue(queue);
            log_debug << "Queue deleted, queue: " << request.queueUrl << " count:" << deleted;

            Dto::SQS::DeleteQueueResponse response;
            response.queueUrl = request.queueUrl;
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::SendMessageResponse SQSService::SendMessage(const Dto::SQS::SendMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "send_message");
        log_trace << "Sending message request, queueUrl: " << request.queueUrl;

        // Queue URL contains the host name and is therefore not reliable
        const std::string queueArn = Core::AwsUtils::ConvertSQSQueueUrlToArn(request.region, request.queueUrl);

        if (!request.queueUrl.empty() && !_sqsDatabase.QueueArnExists(queueArn)) {
            log_error << "Queue does not exist, queueArn: " << queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + queueArn);
        }

        try {

            // Get queue by ARN
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(queueArn);

            // Entity
            Database::Entity::SQS::Message message = Dto::SQS::Mapper::map(request);

            // System attributes
            message.attributes["SentTimestamp"] = std::to_string(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()));
            message.attributes["ApproximateFirstReceivedTimestamp"] = std::to_string(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()));
            message.attributes["ApproximateReceivedCount"] = std::to_string(1);
            message.attributes["VisibilityTimeout"] = std::to_string(queue.attributes.visibilityTimeout);
            message.attributes["SenderId"] = request.user;

            // Default message attributes
            if (!queue.defaultMessageAttributes.empty()) {
                message.messageAttributes.insert(queue.defaultMessageAttributes.begin(), queue.defaultMessageAttributes.end());
            }

            // Set delay
            if (queue.attributes.delaySeconds > 0) {
                message.reset = system_clock::now() + std::chrono::seconds(queue.attributes.delaySeconds);
                queue.attributes.approximateNumberOfMessagesDelayed++;
                queue = _sqsDatabase.UpdateQueue(queue);
            } else {
                message.reset = system_clock::now() + std::chrono::seconds(queue.attributes.visibilityTimeout);
            }

            // Set parameters
            message.queueArn = queue.queueArn;
            message.queueName = queue.name;
            message.contentType = request.contentType;
            message.size = static_cast<long>(request.body.size());
            message.created = system_clock::now();
            message.modified = system_clock::now();
            message.messageId = Core::AwsUtils::CreateMessageId();
            message.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
            message.md5Body = Core::Crypto::GetMd5FromString(request.body);
            message.md5MessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(message.messageAttributes);
            //message.md5MessageSystemAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(message.attributes);

            // Update database
            message = _sqsDatabase.CreateMessage(message);
            log_debug << "Message send, queueName: " << queue.name;

            // Find Lambdas with this as an event source
            CheckLambdaNotifications(queue.queueArn, message);
            log_debug << "Send message, queueArn: " << queue.queueArn;
            return Dto::SQS::Mapper::map(request, message);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::SendMessageBatchResponse SQSService::SendMessageBatch(const Dto::SQS::SendMessageBatchRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "send_message_batch");
        log_trace << "Send message batch request, queueUrl: " << request.queueUrl;

        const std::string queueUrl = Core::SanitizeSQSUrl(request.queueUrl);
        if (!_sqsDatabase.QueueUrlExists(request.region, queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + queueUrl);
        }

        try {
            // Get queue by URL
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, queueUrl);

            Dto::SQS::SendMessageBatchResponse sqsResponse;
            for (const auto &entry: request.entries) {

                Dto::SQS::SendMessageRequest entryRequest;
                entryRequest.region = request.region;
                entryRequest.queueUrl = queueUrl;
                entryRequest.attributes = entry.attributes;
                entryRequest.messageAttributes = entry.messageAttributes;
                entryRequest.body = entry.body;

                // Send the message
                Dto::SQS::SendMessageResponse response = SendMessage(entryRequest);

                Dto::SQS::MessageSuccessful s;
                s.id = entry.id;
                s.messageId = response.messageId;
                s.md5Body = response.md5Body;
                s.md5MessageAttributes = response.md5MessageAttributes;
                s.md5SystemAttributes = response.md5MessageSystemAttributes;
                sqsResponse.successful.emplace_back(s);
            }
            return sqsResponse;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ReceiveMessageResponse SQSService::ReceiveMessages(const Dto::SQS::ReceiveMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "receive_messages");
        log_trace << "Receive message request: " << request.ToString();

        // Queue URL contains the host name and is therefore not reliable
        const std::string queueArn = Core::AwsUtils::ConvertSQSQueueUrlToArn(request.region, request.queueUrl);

        // Check input parameter
        if (!request.queueUrl.empty() && !_sqsDatabase.QueueArnExists(queueArn)) {
            log_error << "Queue does not exist, queueArn: " << queueArn;
            throw Core::ServiceException("Queue does not exist, queueArn: " + queueArn);
        }

        const long pollPeriod = Core::Configuration::instance().GetValue<long>("awsmock.modules.sqs.receive-poll");
        try {

            // Get the queue
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(queueArn);

            // Check re-drive policy
            std::string dlQueueArn{};
            long maxRetries = -1;
            const long visibilityTimeout = queue.attributes.visibilityTimeout;
            if (!queue.attributes.redrivePolicy.deadLetterTargetArn.empty()) {
                dlQueueArn = queue.attributes.redrivePolicy.deadLetterTargetArn;
                maxRetries = queue.attributes.redrivePolicy.maxReceiveCount;
            }

            // Restrict wait time, otherwise the socker timeout is reached
            const long waitTimeSeconds = request.waitTimeSeconds > 10 ? 10 : request.waitTimeSeconds;

            Database::Entity::SQS::MessageList messageList;
            if (waitTimeSeconds == 0) {

                // Short polling period
                _sqsDatabase.ReceiveMessages(queue.queueArn, visibilityTimeout, request.maxMessages, dlQueueArn, maxRetries, messageList);
                log_trace << "Messages in list, url: " << queue.queueUrl << " count: " << messageList.size();

            } else {

                // Long polling period
                long elapsed = 0;
                const auto begin = system_clock::now();
                while (elapsed < waitTimeSeconds) {

                    _sqsDatabase.ReceiveMessages(queue.queueArn, visibilityTimeout, request.maxMessages, dlQueueArn, maxRetries, messageList);
                    log_trace << "Messages in list, url: " << queue.queueUrl << " count: " << messageList.size();

                    if (!messageList.empty()) {
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(pollPeriod));
                    elapsed = std::chrono::duration_cast<std::chrono::seconds>(system_clock::now() - begin).count();
                }
            }

            // Prepare response
            Dto::SQS::ReceiveMessageResponse response;
            response.requestId = request.requestId;
            if (!messageList.empty()) {

                // Adjust queue attributes
                _sqsDatabase.UpdateQueueInvisibleNumber(queueArn, messageList.size());

                response.messageList = Dto::SQS::Mapper::map(messageList);
            }
            log_debug << "Messages received, count: " << messageList.size() << " queue: " << queue.name;
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListMessagesResponse SQSService::ListMessages(const Dto::SQS::ListMessagesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_messages");
        log_trace << "List all messages request";

        try {
            const long total = _sqsDatabase.CountMessages(request.queueArn);

            const Database::Entity::SQS::MessageList messages = _sqsDatabase.ListMessages(request.queueArn, {}, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));

            Dto::SQS::ListMessagesResponse listMessagesResponse;
            listMessagesResponse.total = total;
            for (const auto &message: messages) {
                Dto::SQS::MessageEntry messageEntry;
                messageEntry.region = request.region;
                messageEntry.messageId = message.messageId;
                messageEntry.id = message.oid;
                messageEntry.body = message.body;
                messageEntry.receiptHandle = message.receiptHandle;
                messageEntry.md5Body = message.md5Body;
                messageEntry.created = message.created;
                messageEntry.modified = message.modified;
                listMessagesResponse.messages.emplace_back(messageEntry);
            }
            log_trace << "SQS list messages response: " << listMessagesResponse.ToJson();
            return listMessagesResponse;
        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListMessageCountersResponse SQSService::ListMessageCounters(const Dto::SQS::ListMessageCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_message_counters");
        log_trace << "List message counters request, queueArn: " << request.queueArn;

        try {
            const long total = _sqsDatabase.CountMessages(request.queueArn, request.prefix);

            const Database::Entity::SQS::MessageList messages = _sqsDatabase.ListMessages(request.queueArn, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            log_trace << "List message counters request, queueArn: " << request.queueArn << ", count: " << messages.size();
            return Dto::SQS::Mapper::map(messages, total);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetMessageCountersResponse SQSService::GetMessageCounters(const Dto::SQS::GetMessageCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "get_message");
        log_trace << "Get message request, messageId: " << request.messageId;

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);
            log_debug << "Got message , messageId: " << request.messageId;
            Dto::SQS::GetMessageCountersResponse response;
            response.message = Dto::SQS::Mapper::map(message);
            response.region = request.region;
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::UpdateMessage(const Dto::SQS::UpdateMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "update_message");
        log_trace << "Update message request, messageId: " << request.messageId;

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);
            message.messageAttributes = Dto::SQS::Mapper::map(request.messageAttributes);

            // Delete from database
            message = _sqsDatabase.UpdateMessage(message);
            log_debug << "Message updated, messageId: " << request.messageId;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::ResendMessage(const Dto::SQS::ResendMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, "action", "resend_message");
        log_trace << "Resend message request, queueArn: " << request.queueArn;

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);

            // Adjust queue attributes
            queue.attributes.approximateNumberOfMessagesNotVisible--;

            message.status = Database::Entity::SQS::MessageStatus::INITIAL;
            message.retries = 0;
            message.reset = system_clock::now() + std::chrono::seconds(std::stoi(message.attributes.at("VisibilityTimeout")));

            // Update database
            message = _sqsDatabase.UpdateMessage(message);
            queue = _sqsDatabase.UpdateQueue(queue);
            log_debug << "Message resend, messageId: " << request.messageId;

            // Check lambda notification
            CheckLambdaNotifications(request.queueArn, message);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListMessageAttributeCountersResponse SQSService::ListMessageAttributeCounters(const Dto::SQS::ListMessageAttributeCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "list_message_attribute_counters");
        log_trace << "List message counters request";

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }
        try {
            const Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);

            Dto::SQS::ListMessageAttributeCountersResponse response;
            response.total = static_cast<long>(message.messageAttributes.size());
            response.messageAttributeCounters = Dto::SQS::Mapper::map(message.messageAttributes);
            response.messageAttributeCounters = Core::PageMap<std::string, Dto::SQS::MessageAttribute>(response.messageAttributeCounters, request.pageSize, request.pageIndex);
            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    std::string SQSService::ExportMessages(const Dto::SQS::ExportMessagesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "export_messages");
        log_trace << "Export all messages request";

        try {
            const Database::Entity::SQS::MessageList messages = _sqsDatabase.ListMessages(request.queueArn, {}, -1, -1, {});

            array listMessagesResponse;
            for (const auto &message: messages) {
                listMessagesResponse.append(message.ToDocument());
            }
            return Core::Bson::BsonUtils::ToJsonString(listMessagesResponse);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::ImportMessages(const Dto::SQS::ImportMessagesRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "import_messages");
        log_trace << "Export all messages request";

        try {
            const value messageArray = bsoncxx::from_json(request.messages);
            _sqsDatabase.ImportMessages(request.queueArn, messageArray);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::AddMessageAttribute(const Dto::SQS::AddAttributeRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "add_message_attribute");
        log_trace << "Delete message attribute request, messageId: " << request.messageId << ", name: " << request.name;

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);

            // Add attributes
            Database::Entity::SQS::MessageAttribute messageAttribute;
            messageAttribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(request.dataType);
            messageAttribute.stringValue = request.value;
            message.messageAttributes[request.name] = messageAttribute;
            message = _sqsDatabase.UpdateMessage(message);
            log_debug << "Message attribute deleted, messageId: " << message.messageId << ", name: " << request.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::DeleteMessageAttribute(const Dto::SQS::DeleteAttributeRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "delete_message_attribute");
        log_trace << "Delete message attribute request, messageId: " << request.messageId << ", name: " << request.name;

        if (!_sqsDatabase.MessageExistsByMessageId(request.messageId)) {
            log_error << "Message does not exist, messageId: " << request.messageId;
            throw Core::ServiceException("Message does not exist, messageId: " + request.messageId);
        }

        try {
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByMessageId(request.messageId);

            // Update attributes
            message.messageAttributes.erase(request.name);
            message = _sqsDatabase.UpdateMessage(message);
            log_debug << "Message attribute deleted, messageId: " << message.messageId << ", name: " << request.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::DeleteMessage(const Dto::SQS::DeleteMessageRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "delete_message");
        log_trace << "Delete message request, url: " << request.receiptHandle;

        try {

            // Delete from database
            const long deleted = _sqsDatabase.DeleteMessage(request.receiptHandle);
            log_debug << "Message deleted, receiptHandle: " << request.receiptHandle << " deleted: " << deleted;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::DeleteMessageBatchResponse SQSService::DeleteMessageBatch(const Dto::SQS::DeleteMessageBatchRequest &request) const {
        Monitoring::MonitoringTimer measure(SQS_SERVICE_TIMER, SQS_SERVICE_COUNTER, "action", "delete_message_batch");
        log_trace << "Delete message batch request, size: " << request.entries.size();

        try {

            Dto::SQS::DeleteMessageBatchResponse deleteMessageBatchResponse;
            for (const auto &d: request.entries) {

                // Delete from database
                const long deleted = _sqsDatabase.DeleteMessage(d.receiptHandle);
                if (deleted > 0) {
                    deleteMessageBatchResponse.successfull.emplace_back(d.id);
                } else {
                    deleteMessageBatchResponse.failed.emplace_back(d.id);
                }
                log_trace << "Message deleted, receiptHandle: " << d.receiptHandle.substr(0, 40) << " deleted: " << deleted;
            }
            log_debug << "Message batch deleted, success: " << deleteMessageBatchResponse.successfull.size() << ", failure: " << deleteMessageBatchResponse.failed.size();
            return deleteMessageBatchResponse;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    bool SQSService::CheckAttribute(const std::vector<std::string> &attributes, const std::string &value) {
        return std::ranges::find_if(attributes, [&value](const std::string &attribute) {
            return Core::StringUtils::EqualsIgnoreCase(attribute, value);
        }) != attributes.end();
    }

    void SQSService::CheckLambdaNotifications(const std::string &queueArn, const Database::Entity::SQS::Message &message) const {
        if (const std::vector<Database::Entity::Lambda::Lambda> lambdas = Database::LambdaDatabase::instance().ListLambdasWithEventSource(queueArn); !lambdas.empty()) {
            log_debug << "Found lambda notification events, count: " << lambdas.size();
            for (const auto &lambda: lambdas) {
                SendLambdaInvocationRequest(lambda, message, queueArn);
            }
        }
    }

    void SQSService::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const Database::Entity::SQS::Message &message, const std::string &eventSourceArn) const {
        log_debug << "Invoke lambda function request, function: " << lambda.function;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");

        // Create the event record
        Dto::SQS::EventRecord record;
        record.region = lambda.region;
        record.messageId = message.messageId;
        record.receiptHandle = message.receiptHandle;
        record.body = message.body;
        record.attributes = message.attributes;
        record.messageAttributes = Dto::SQS::Mapper::mapEventMessageAttributes(message.messageAttributes);
        record.md5Sum = message.md5Body;
        record.eventSource = "aws:sqs";
        record.eventSourceArn = eventSourceArn;

        Dto::SQS::EventNotification eventNotification;
        eventNotification.records.emplace_back(record);
        log_debug << "Invocation request function name: " << lambda.function << " json: " << eventNotification.ToJson();

        std::string payload = eventNotification.ToJson();
        Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, lambda.function, payload, Dto::Lambda::LambdaInvocationType::EVENT);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function << ", sourceArn: " << eventSourceArn << ", result: " << result;
    }
} // namespace AwsMock::Service
