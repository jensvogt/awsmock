//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/sqs/SQSService.h>
#include <thread>

namespace AwsMock::Service {

    Dto::SQS::CreateQueueResponse SQSService::CreateQueue(const Dto::SQS::CreateQueueRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "create_queue");
        log_trace << "Create queue request, region: " << request.region << " name: " << request.queueName;

        // Get queue ARN
        const std::string queueArn = Core::CreateSQSQueueArn(request.region, request.queueName);

        // Check existence. In case the queue exists already return the existing queue.
        if (_sqsDatabase.QueueArnExists(queueArn)) {
            log_warning << "Queue exists already, region: " << request.region << " queueUrl: " << request.queueUrl;
            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(queueArn);
            return {.region = queue.region, .name = queue.name, .owner = queue.owner, .queueUrl = queue.queueUrl, .queueArn = queue.queueArn};
        }

        try {

            // Get queue URL
            const std::string queueUrl = Core::CreateSQSQueueUrl(request.queueName);

            Database::Entity::SQS::QueueAttribute attributes;
            for (const auto &[attributeName, attributeValue]: request.attributes) {
                if (attributeName == "DelaySeconds") {
                    attributes.delaySeconds = std::stoi(attributeValue);
                }
                if (attributeName == "MaxMessageSize") {
                    attributes.maxMessageSize = std::stoi(attributeValue);
                }
                if (attributeName == "MessageRetentionPeriod") {
                    attributes.messageRetentionPeriod = std::stoi(attributeValue);
                }
                if (attributeName == "VisibilityTimeout") {
                    attributes.visibilityTimeout = std::stoi(attributeValue);
                }
                if (attributeName == "Policy") {
                    attributes.policy = attributeValue;
                }
                if (attributeName == "RedrivePolicy") {
                    attributes.redrivePolicy.FromJson(attributeValue);
                }
                if (attributeName == "RedriveAllowPolicy") {
                    attributes.redriveAllowPolicy = attributeValue;
                }
                if (attributeName == "ReceiveMessageWaitTime") {
                    attributes.receiveMessageWaitTime = std::stoi(attributeValue);
                }
                if (attributeName == "QueueArn") {
                    attributes.queueArn = attributeValue;
                }
            }
            attributes.queueArn = queueArn;

            // Update database
            Database::Entity::SQS::Queue queue = {.region = request.region, .name = request.queueName, .owner = request.owner, .queueUrl = queueUrl, .queueArn = queueArn, .attributes = attributes, .tags = request.tags};
            queue = _sqsDatabase.CreateQueue(queue);
            log_trace << "SQS queue created: " << queue.ToString();
            Dto::SQS::CreateQueueResponse response = {.region = queue.region, .name = queue.name, .owner = queue.owner, .queueUrl = queue.queueUrl, .queueArn = queue.queueArn};
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueuesResponse SQSService::ListQueues(const Dto::SQS::ListQueuesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_queues");
        log_trace << "List all queues request, region: " << request.region;

        try {

            if (request.maxResults > 0) {

                // Get total number
                const long total = _sqsDatabase.CountQueues(request.region);
                const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.queueNamePrefix, request.maxResults, 0, {}, request.region);
                const std::string nextToken = queueList.size() > 0 ? queueList.back().oid : "";
                Dto::SQS::ListQueuesResponse listQueueResponse = {.queueList = queueList, .nextToken = nextToken, .total = total};
                log_trace << "SQS create queue list response: " << listQueueResponse.ToXml();
                return listQueueResponse;
            }
            const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.region);
            Dto::SQS::ListQueuesResponse listQueueResponse = {.queueList = queueList};
            log_trace << "SQS create queue list response: " << listQueueResponse.ToXml();
            return listQueueResponse;
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::ListQueueArnsResponse SQSService::ListQueueArns() const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_queue_arns");
        log_trace << "List all queues ARNs request";

        try {

            Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
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
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_queue_counters");
        log_trace << "List all queues counters request";

        try {

            Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(request.prefix, request.pageSize, request.pageIndex, request.sortColumns, request.region);

            Dto::SQS::ListQueueCountersResponse listQueueResponse;
            listQueueResponse.total = _sqsDatabase.CountQueues(request.prefix, request.region);
            for (const auto &queue: queueList) {
                const long size = _sqsDatabase.GetQueueSize(queue.queueArn);
                Dto::SQS::QueueCounter counter;
                counter.queueName = queue.name;
                counter.queueArn = queue.queueArn;
                counter.queueUrl = queue.queueUrl;
                counter.available = queue.attributes.approximateNumberOfMessages;
                counter.invisible = queue.attributes.approximateNumberOfMessagesNotVisible;
                counter.delayed = queue.attributes.approximateNumberOfMessagesDelayed;
                counter.visibilityTimeout = queue.attributes.visibilityTimeout;
                counter.maxMessageSize = queue.attributes.maxMessageSize;
                counter.retentionPeriod = queue.attributes.messageRetentionPeriod;
                counter.size = size;
                counter.created = queue.created;
                counter.modified = queue.modified;
                listQueueResponse.queueCounters.emplace_back(counter);
            }
            log_trace << "SQS create queue counters list response: " << listQueueResponse.ToJson();
            return listQueueResponse;

        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SQS::GetQueueDetailsResponse SQSService::GetQueueDetails(const Dto::SQS::GetQueueDetailsRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "get_queue_details");
        log_trace << "Get queue details request, queueArn: " << request.queueArn;

        try {

            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.queueArn);
            const long size = _sqsDatabase.CountMessageSize(request.queueArn);

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
            sqsResponse.size = size;
            sqsResponse.available = queue.attributes.approximateNumberOfMessages;
            sqsResponse.invisible = queue.attributes.approximateNumberOfMessagesNotVisible;
            sqsResponse.delayed = queue.attributes.approximateNumberOfMessagesDelayed;
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
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_queue_tags");
        log_trace << "List all queues tags request";

        try {

            const Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            Dto::SQS::ListQueueTagsResponse listQueueTagsResponse;
            listQueueTagsResponse.total = queue.tags.size();
            listQueueTagsResponse.tags = queue.tags;
            log_trace << "SQS create queue tags list response: " << listQueueTagsResponse.ToJson();
            return listQueueTagsResponse;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::PurgeQueue(const Dto::SQS::PurgeQueueRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "purge_queue");
        log_trace << "Purge queue request, region: " << request.region << " queueUrl: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {

            // Update messages
            const std::string queueArn = Core::AwsUtils::ConvertSQSQueueUrlToArn(request.region, request.queueUrl);
            _sqsDatabase.PurgeQueue(queueArn);
            log_trace << "SQS queue purged, queueArn: " << queueArn;

            // Update queue counter
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);
            queue.attributes.approximateNumberOfMessages = 0;
            queue.attributes.approximateNumberOfMessagesDelayed = 0;
            queue.attributes.approximateNumberOfMessagesNotVisible = 0;
            queue = _sqsDatabase.UpdateQueue(queue);
            log_trace << "SQS queue counter updated, queueArn: " << queue.queueArn;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetQueueUrlResponse SQSService::GetQueueUrl(const Dto::SQS::GetQueueUrlRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "get_queue_url");
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
            log_info << "SQS get queue URL, region: " << request.region << " queueName: " << queue.queueUrl;
            return {.queueUrl = queue.queueUrl};

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::GetQueueAttributesResponse SQSService::GetQueueAttributes(const Dto::SQS::GetQueueAttributesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "get_queue_attributes");
        log_trace << "Get queue userAttributes request, request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

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
        log_debug << response.ToString();
        return response;
    }

    Dto::SQS::SetQueueAttributesResponse SQSService::SetQueueAttributes(Dto::SQS::SetQueueAttributesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "set_queue_attributes");
        log_trace << "Put queue sqs request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {

            // Get the queue
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);
            log_trace << "Got queue: " << queue.ToString();

            // Reset all userAttributes
            if (!request.attributes["Policy"].empty()) {
                queue.attributes.policy = request.attributes["Policy"];
            }
            if (!request.attributes["RedrivePolicy"].empty()) {
                queue.attributes.redrivePolicy.FromJson(request.attributes["RedrivePolicy"]);
            }
            if (!request.attributes["RedriveAllowPolicy"].empty()) {
                queue.attributes.redriveAllowPolicy = request.attributes["RedriveAllowPolicy"];
            }
            if (!request.attributes["MessageRetentionPeriod"].empty()) {
                queue.attributes.messageRetentionPeriod = std::stoi(request.attributes["MessageRetentionPeriod"]);
            }
            if (!request.attributes["VisibilityTimeout"].empty()) {
                queue.attributes.visibilityTimeout = std::stoi(request.attributes["VisibilityTimeout"]);
            }
            if (!request.attributes["QueueArn"].empty()) {
                queue.attributes.queueArn = request.attributes["QueueArn"];
            } else {
                queue.attributes.queueArn = queue.queueArn;
            }

            // Update database
            queue = _sqsDatabase.UpdateQueue(queue);
            log_trace << "Queue updated: " << queue.ToString();

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
        return {.resource = "sqs", .requestId = request.requestId};
    }

    void SQSService::SetVisibilityTimeout(Dto::SQS::ChangeMessageVisibilityRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "set_visibility_timeout");
        log_trace << "Change message visibilityTimeout request, queue: " << request.queueUrl;

        // Check existence
        if (!_sqsDatabase.MessageExists(request.receiptHandle)) {
            log_error << "Message does not exist, receiptHandle: " << request.receiptHandle;
            throw Core::ServiceException("Message does not exist, receiptHandle: " + request.receiptHandle);
        }

        try {

            // Get the message
            Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByReceiptHandle(request.receiptHandle);
            log_trace << "Got message: " << message.ToString();

            // Reset all userAttributes
            const Database::Entity::SQS::MessageAttribute messageAttribute = {.attributeName = "VisibilityTimeout", .attributeValue = std::to_string(request.visibilityTimeout), .attributeType = Database::Entity::SQS::MessageAttributeType::NUMBER};
            message.messageAttributes.push_back(messageAttribute);
            message.reset = system_clock::now() + std::chrono::seconds(request.visibilityTimeout);

            // Update database
            message = _sqsDatabase.UpdateMessage(message);
            log_trace << "Message updated: " << message.ToString();

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void SQSService::TagQueue(const Dto::SQS::TagQueueRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "set_visibility_timeout");
        log_trace << "Tag queue request, queue: " << request.queueUrl;

        try {

            // Check existence
            if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
                throw Core::ServiceException("SQS queue topic does not exists");
            }

            // Get the topic
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            // Set tags and update database
            for (const auto &[fst, snd]: request.tags) {
                queue.tags[fst] = snd;
            }

            queue = _sqsDatabase.UpdateQueue(queue);
            log_info << "SQS queue tags updated, count: " << request.tags.size() << " queue: " << queue.name;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::DeleteQueueResponse SQSService::DeleteQueue(const Dto::SQS::DeleteQueueRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "delete_queue");
        log_trace << "Delete queue request, request: " << request.ToString();

        // Check existence
        if (!_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {

            // Delete all resources in queue
            long deleted = _sqsDatabase.DeleteMessages(request.queueUrl);
            log_debug << "Messages deleted, queue: " << request.queueUrl << " count:" << deleted;

            // Update database
            deleted = _sqsDatabase.DeleteQueue({.region = request.region, .queueUrl = request.queueUrl});
            log_debug << "Queue deleted, queue: " << request.queueUrl << " count:" << deleted;

            return {.region = request.region, .queueUrl = request.queueUrl, .requestId = request.requestId};

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::SendMessageResponse SQSService::SendMessage(const Dto::SQS::SendMessageRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "send_message");
        log_trace << "Send message request, queueUrl: " << request.queueUrl;

        if (!request.queueUrl.empty() && !_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {

            // Get queue by URL
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            // Entity
            Database::Entity::SQS::Message message = Dto::SQS::Mapper::map(request);

            // System attributes
            message.attributes["SentTimestamp"] = std::to_string(Core::DateTimeUtils::UnixTimestampNow());
            message.attributes["ApproximateFirstReceivedTimestamp"] = std::to_string(Core::DateTimeUtils::UnixTimestampNow());
            message.attributes["ApproximateReceivedCount"] = std::to_string(0);
            message.attributes["VisibilityTimeout"] = std::to_string(queue.attributes.visibilityTimeout);
            message.attributes["SenderId"] = request.senderId;

            // Set userAttributes
            for (const auto &[fst, snd]: request.messageAttributes) {
                message.messageAttributes.push_back({.attributeName = fst,
                                                     .attributeValue = snd.stringValue,
                                                     .attributeType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.type))});
            }

            // Set delay
            if (queue.attributes.delaySeconds > 0) {
                message.reset = system_clock::now() + std::chrono::seconds(queue.attributes.delaySeconds);
                queue.attributes.approximateNumberOfMessagesDelayed++;
            } else {
                message.reset = system_clock::now() + std::chrono::seconds(queue.attributes.visibilityTimeout);
            }

            // Set parameters
            message.queueArn = queue.queueArn;
            message.queueName = queue.name;
            message.created = system_clock::now();
            message.modified = system_clock::now();
            message.messageId = Core::AwsUtils::CreateMessageId();
            message.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
            message.md5Body = Core::Crypto::GetMd5FromString(request.body);
            message.md5UserAttr = Dto::SQS::MessageAttribute::GetMd5MessageAttributes(request.messageAttributes);
            message.md5SystemAttr = Dto::SQS::MessageAttribute::GetMd5Attributes(request.attributes);

            // Update database
            queue.attributes.approximateNumberOfMessages++;
            queue = _sqsDatabase.UpdateQueue(queue);
            message = _sqsDatabase.CreateMessage(message);
            log_debug << "Message send, queueName: " << queue.name << " messageId: " << request.messageId;

            // Find Lambdas with this as event source
            const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
            const std::string queueArn = Core::AwsUtils::CreateSQSQueueArn(request.region, accountId, queue.name);
            if (std::vector<Database::Entity::Lambda::Lambda> lambdas = Database::LambdaDatabase::instance().ListLambdasWithEventSource(queueArn); !lambdas.empty()) {
                log_info << "Found lambda notification events, count: " << lambdas.size();
                for (const auto &lambda: lambdas) {
                    SendLambdaInvocationRequest(lambda, message, queueArn);
                }
            }

            return Dto::SQS::Mapper::map(request, message);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::SendMessageBatchResponse SQSService::SendMessageBatch(const Dto::SQS::SendMessageBatchRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "send_message_batch");
        log_trace << "Send message batch request, queueUrl: " << request.queueUrl;

        if (!request.queueUrl.empty() && !_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {

            // Get queue by URL
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            Dto::SQS::SendMessageBatchResponse sqsResponse;
            for (const auto &entry: request.entries) {
                Dto::SQS::SendMessageRequest entryRequest;
                entryRequest.region = request.region;
                entryRequest.queueUrl = request.queueUrl;
                entryRequest.attributes = entry.attributes;
                entryRequest.messageAttributes = entry.messageAttributes;
                entryRequest.body = entry.body;
                entryRequest.md5sum = entry.md5Sum;
                entryRequest.messageId = entry.messageId;
                try {
                    Dto::SQS::SendMessageResponse response = SendMessage(entryRequest);
                    Dto::SQS::MessageSuccessful s = {.id = entry.id, .messageId = response.messageId, .md5Body = response.md5Body, .md5UserAttr = response.md5UserAttr, .md5SystemAttr = response.md5SystemAttr};
                    sqsResponse.successful.emplace_back(s);
                } catch (Core::DatabaseException &exc) {
                    Dto::SQS::MessageFailed f = {.id = Core::StringUtils::CreateRandomUuid(), .message = exc.message(), .senderFault = false};
                    sqsResponse.failed.emplace_back(f);
                }
            }

            return sqsResponse;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ReceiveMessageResponse SQSService::ReceiveMessages(const Dto::SQS::ReceiveMessageRequest &request) const {
        log_trace << "Receive message request: " << request.ToString();

        if (!request.queueUrl.empty() && !_sqsDatabase.QueueUrlExists(request.region, request.queueUrl)) {
            log_error << "Queue does not exist, region: " << request.region << " queueUrl: " << request.queueUrl;
            throw Core::ServiceException("Queue does not exist, region: " + request.region + " queueUrl: " + request.queueUrl);
        }

        try {
            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByUrl(request.region, request.queueUrl);

            // Check re-drive policy
            std::string dlQueueArn{};
            int maxRetries = -1;
            const int visibilityTimeout = queue.attributes.visibilityTimeout;
            if (!queue.attributes.redrivePolicy.deadLetterTargetArn.empty()) {
                dlQueueArn = queue.attributes.redrivePolicy.deadLetterTargetArn;
                maxRetries = queue.attributes.redrivePolicy.maxReceiveCount;
            }

            Database::Entity::SQS::MessageList messageList;
            if (request.waitTimeSeconds == 0) {

                // Short polling period
                _sqsDatabase.ReceiveMessages(queue.queueArn, visibilityTimeout, request.maxMessages, dlQueueArn, maxRetries, messageList);
                log_trace << "Messages in list, url: " << queue.queueUrl << " count: " << messageList.size();

            } else {
                long elapsed = 0;

                // Long polling period
                const auto begin = system_clock::now();
                while (elapsed < request.waitTimeSeconds) {

                    Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "receive_message");

                    _sqsDatabase.ReceiveMessages(queue.queueArn, visibilityTimeout, request.maxMessages, dlQueueArn, maxRetries, messageList);
                    log_trace << "Messages in list, url: " << queue.queueUrl << " count: " << messageList.size();

                    if (!messageList.empty()) {
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    elapsed = std::chrono::duration_cast<std::chrono::seconds>(system_clock::now() - begin).count();
                }
            }

            // Prepare response
            Dto::SQS::ReceiveMessageResponse response;
            response.requestId = request.requestId;
            if (!messageList.empty()) {

                // Update queue
                queue.attributes.approximateNumberOfMessagesNotVisible += static_cast<long>(messageList.size());
                queue = _sqsDatabase.UpdateQueue(queue);
                response.messageList = messageList;
            }
            log_info << "Messages received, count: " << messageList.size() << " queue: " << queue.name;

            return response;

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::ListMessagesResponse SQSService::ListMessages(const Dto::SQS::ListMessagesRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_messages");
        log_trace << "List all messages request";

        try {

            const long total = _sqsDatabase.CountMessages(request.queueArn);

            Database::Entity::SQS::MessageList messages = _sqsDatabase.ListMessages(request.queueArn, {}, request.pageSize, request.pageIndex, request.sortColumns);

            Dto::SQS::ListMessagesResponse listMessagesResponse;
            listMessagesResponse.total = total;
            for (const auto &message: messages) {
                Dto::SQS::MessageEntry messageEntry;
                messageEntry.region = request.region;
                messageEntry.messageId = message.messageId;
                messageEntry.id = message.oid;
                messageEntry.body = message.body;
                messageEntry.receiptHandle = message.receiptHandle;
                messageEntry.md5Sum = message.md5Body;
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
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "list_messages");
        log_trace << "List message counters request";

        try {

            const long total = _sqsDatabase.CountMessages(request.queueArn, request.prefix);

            Database::Entity::SQS::MessageList messages = _sqsDatabase.ListMessages(request.queueArn, request.prefix, request.pageSize, request.pageIndex, request.sortColumns);

            Dto::SQS::ListMessageCountersResponse listMessagesResponse;
            listMessagesResponse.total = total;
            for (const auto &message: messages) {
                Dto::SQS::MessageEntry messageEntry;
                messageEntry.messageId = message.messageId;
                messageEntry.id = message.oid;
                messageEntry.body = message.body;
                messageEntry.receiptHandle = message.receiptHandle;
                messageEntry.md5Sum = message.md5Body;
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

    void SQSService::DeleteMessage(const Dto::SQS::DeleteMessageRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "delete_message");
        log_trace << "Delete message request, url: " << request.receiptHandle;

        try {
            if (!_sqsDatabase.MessageExists(request.receiptHandle)) {
                log_error << "Message does not exist, receiptHandle: " << request.receiptHandle;
                throw Core::ServiceException("Message does not exist, receiptHandle: " + request.receiptHandle);
            }
            const Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByReceiptHandle(request.receiptHandle);

            // Delete from database
            const long deleted = _sqsDatabase.DeleteMessage({.receiptHandle = request.receiptHandle});
            log_debug << "Message deleted, receiptHandle: " << request.receiptHandle << "deleted: " << deleted;

            // Update queue counters
            AdjustMessageCounters(message.queueArn);

        } catch (Core::DatabaseException &ex) {
            log_error << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::SQS::DeleteMessageBatchResponse SQSService::DeleteMessageBatch(const Dto::SQS::DeleteMessageBatchRequest &request) const {
        Monitoring::MetricServiceTimer measure(SQS_SERVICE_TIMER, "method", "delete_message_batch");
        log_trace << "Delete message batch request, size: " << request.deleteMessageBatchEntries.size();

        const std::string queueArn = Core::AwsUtils::ConvertSQSQueueUrlToArn(request.region, request.queueUrl);
        try {
            long deleted = 0;
            Dto::SQS::DeleteMessageBatchResponse deleteMessageBatchResponse;
            for (const auto &[id, receiptHandle]: request.deleteMessageBatchEntries) {

                if (!_sqsDatabase.MessageExists(receiptHandle)) {
                    log_warning << "Message does not exist, receiptHandle: " << receiptHandle.substr(0, 40);
                    Dto::SQS::BatchResultErrorEntry failure = {.id = id};
                    deleteMessageBatchResponse.failed.emplace_back(failure);
                    return deleteMessageBatchResponse;
                }

                // Delete from database
                Database::Entity::SQS::Message message = _sqsDatabase.GetMessageByReceiptHandle(receiptHandle);
                deleted += _sqsDatabase.DeleteMessage(message);

                // Successful
                Dto::SQS::DeleteMessageBatchResultEntry success = {.id = id};
                deleteMessageBatchResponse.successFull.emplace_back(success);
            }

            // Update queue counters
            AdjustMessageCounters(queueArn);

            log_debug << "Message batch deleted, count: " << deleted;
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

    void SQSService::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, Database::Entity::SQS::Message &message, const std::string &eventSourceArn) const {
        log_debug << "Invoke lambda function request, size: " << lambda.function;

        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");
        const std::string user = Core::Configuration::instance().GetValueString("awsmock.user");

        // Create the event record
        Dto::SQS::Record record = {.region = lambda.region, .messageId = message.messageId, .receiptHandle = message.receiptHandle, .body = message.body, .attributes = message.attributes, .md5Sum = message.md5Body, .eventSource = "aws:sqs", .eventSourceArn = eventSourceArn};

        Dto::SQS::EventNotification eventNotification;
        eventNotification.records.emplace_back(record);
        log_debug << "Invocation request function name: " << lambda.function << " json: " << eventNotification.ToJson();

        std::string output = _lambdaService.InvokeLambdaFunction(lambda.function, eventNotification.ToJson(), region, user);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function << " sourceArn: " << eventSourceArn;
    }

    void SQSService::AdjustMessageCounters(const std::string &queueArn) const {
        Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(queueArn);

        /*switch (message.status) {
            case Database::Entity::SQS::MessageStatus::INITIAL:
                queue.attributes.approximateNumberOfMessages--;
                break;
            case Database::Entity::SQS::MessageStatus::INVISIBLE:
                queue.attributes.approximateNumberOfMessages--;
                queue.attributes.approximateNumberOfMessagesNotVisible--;
                break;
            case Database::Entity::SQS::MessageStatus::DELAYED:
                queue.attributes.approximateNumberOfMessages--;
                queue.attributes.approximateNumberOfMessagesDelayed--;
                break;
            case Database::Entity::SQS::MessageStatus::UNKNOWN:
                break;
        }*/
        _sqsDatabase.UpdateQueue(queue);
        log_debug << "Queue counters updated, queue: " << queue.queueArn;
    }

}// namespace AwsMock::Service
