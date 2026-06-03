//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/sns/SNSMongoRepository.h>

namespace Awsmock::Database {

    bool SNSMongoRepository::topicExists(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_topic");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _topicCollection.count_documents(make_document(kvp("topicArn", topicArn)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool SNSMongoRepository::topicExists(const std::string &region, const std::string &topicName) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_topic");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _topicCollection.count_documents(make_document(kvp("region", region), kvp("topicName", topicName)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::createTopic(Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "create_topic");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

        try {

            const auto result = _topicCollection.insert_one(topic.ToDocument());
            log_trace << "Topic created, oid: " << result->inserted_id().get_oid().value.to_string();
            topic.oid = result->inserted_id().get_oid().value.to_string();
            return topic;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::getTopicById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            if (const auto mResult = _topicCollection.find_one(make_document(kvp("_id", oid)))) {
                Entity::SNS::Topic result;
                result.FromDocument(mResult->view());
                return result;
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
        return {};
    }

    Entity::SNS::Topic SNSMongoRepository::getTopicById(const std::string &oid) const {
        return getTopicById(bsoncxx::oid(oid));
    }

    Entity::SNS::Topic SNSMongoRepository::getTopicByArn(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            if (const auto mResult = _topicCollection.find_one(make_document(kvp("topicArn", topicArn))); !mResult->empty()) {
                Entity::SNS::Topic result;
                result.FromDocument(mResult->view());
                return result;
            }
            return {};
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::getTopicByTargetArn(const std::string &targetArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            if (const auto mResult = _topicCollection.find_one(make_document(kvp("targetArn", targetArn)));
                !mResult->empty()) {
                Entity::SNS::Topic result;
                result.FromDocument(mResult->view());
                return result;
            }
            return {};
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::getTopicByName(const std::string &region, const std::string &topicName) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            if (const auto mResult = _topicCollection.find_one(
                        make_document(kvp("region", region), kvp("topicName", topicName)));
                !mResult->empty()) {
                Entity::SNS::Topic result;
                result.FromDocument(mResult->view());
                return result;
            }
            log_warning << "Topic not found, region: " << region << " name: " << topicName;
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::TopicList SNSMongoRepository::getTopicsBySubscriptionArn(const std::string &subscriptionArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            Entity::SNS::TopicList topicList{};
            for (auto queueCursor = _topicCollection.find(make_document(kvp("subscriptions.subscriptionArn", subscriptionArn))); const auto topic: queueCursor) {
                Entity::SNS::Topic result;
                result.FromDocument(topic);
                topicList.push_back(result);
            }
            return topicList;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::TopicList SNSMongoRepository::listTopics(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_topic");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            Entity::SNS::TopicList topicList;
            for (auto queueCursor = _topicCollection.find(query.view()); const auto topic: queueCursor) {
                Entity::SNS::Topic result;
                result.FromDocument(topic);
                topicList.push_back(result);
            }
            return topicList;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::TopicList SNSMongoRepository::listTopics(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_topic");

        try {
            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

            document query = {};
            if (!prefix.empty()) {
                query.append(kvp("topicName", make_document(kvp("$regex", "^" + prefix))));
            }
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            if (pageSize > 0) {
                opts.limit(pageSize);
            }
            if (pageIndex > 0) {
                opts.skip(pageIndex * pageSize);
            }

            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                document sort;
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            Entity::SNS::TopicList topicList;
            for (auto topicCursor = _queueCollection.find(query.view(), opts); const auto &topic: topicCursor) {
                Entity::SNS::Topic result;
                result.FromDocument(topic);
                topicList.push_back(result);
            }
            return topicList;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::TopicList SNSMongoRepository::exportTopics(const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "export_topics");

        try {
            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

            const document query = {};

            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                document sort;
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            Entity::SNS::TopicList topicList;
            for (auto queueCursor = _queueCollection.find(query.view(), opts); const auto queue: queueCursor) {
                Entity::SNS::Topic result;
                result.FromDocument(queue);
                topicList.push_back(result);
            }
            log_trace << "Got topic list, size:" << topicList.size();
            return topicList;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::updateTopic(Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_topics");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        try {
            topic.modified = system_clock::now();
            const auto mResult = _topicCollection.find_one_and_update(make_document(kvp("region", topic.region), kvp("topicArn", topic.topicArn)), topic.ToDocument(), opts);
            log_trace << "Topic updated: " << topic.ToJson();

            if (mResult) {
                topic.FromDocument(mResult->view());
                return topic;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Topic SNSMongoRepository::createOrUpdateTopic(Entity::SNS::Topic &topic) const {
        if (topicExists(topic.region, topic.topicName)) {
            return updateTopic(topic);
        }
        return createTopic(topic);
    }

    void SNSMongoRepository::updateTopicCounter(const std::string &topicArn, const long messages, const long size, const long initial, const long send, const long resend) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_topics");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_topicCollectionName];

        try {


            document filterQuery;
            filterQuery.append(kvp("topicArn", topicArn));

            document setQuery;
            setQuery.append(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));
            setQuery.append(kvp("messages", static_cast<bsoncxx::types::b_int64>(messages)));
            setQuery.append(kvp("attributes.availableMessages", static_cast<bsoncxx::types::b_int64>(messages)));

            document updateQuery;
            updateQuery.append(kvp("$set", setQuery));

            _bucketCollection.update_one(filterQuery.extract(), updateQuery.extract());
            log_trace << "SNS topic counter updated";

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void SNSMongoRepository::importTopic(Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "import_topics");

        topic.topicAttribute.availableMessages = 0;
        topic = createOrUpdateTopic(topic);
        log_trace << "Topic imported, topicName: " << topic.topicName;
    }

    long SNSMongoRepository::countTopics(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_topics");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

            document query = {};

            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!prefix.empty()) {
                query.append(kvp("topicName", make_document(kvp("$regex", "^" + prefix))));
            }

            const long count = _topicCollection.count_documents(query.view());
            log_trace << "Count topics, result: " << count;
            return count;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SNSMongoRepository::purgeTopic(const Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "purge_topics");

        long purged = 0;
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _topicCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            const auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
            log_debug << "Topic purge, count: " << result->deleted_count();
            purged = result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();

        return purged;
    }

    long SNSMongoRepository::getTopicSize(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_topics");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            mongocxx::pipeline p{};
            p.match(make_document(kvp("topicArn", topicArn)));
            p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
            p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
            auto totalSizeCursor = _objectCollection.aggregate(p);
            if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                return t["totalSize"].get_int64().value;
            }
            return 0;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void SNSMongoRepository::deleteTopic(const Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_topics");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

        try {
            const auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
            log_debug << "Topic deleted, count: " << result->deleted_count();
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();
    }

    long SNSMongoRepository::deleteAllTopics() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_topics");

        long deleted = 0;
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

        try {
            const auto result = _topicCollection.delete_many({});
            log_debug << "All topics deleted, count: " << result->deleted_count();
            deleted = result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();

        return deleted;
    }

    bool SNSMongoRepository::messageExists(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_message");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _messageCollection.count_documents(make_document(kvp("messageId", messageId)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Message SNSMongoRepository::createMessage(Entity::SNS::Message &message) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "create_message");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = client->database(_databaseName)[_messageCollectionName];

        try {
            const auto result = _messageCollection.insert_one(message.ToDocument());
            log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();
            message.oid = result->inserted_id().get_oid().value.to_string();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();

        return message;
    }

    Entity::SNS::Message SNSMongoRepository::getMessageById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_message");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            const auto mResult = _messageCollection.find_one(make_document(kvp("_id", oid)));
            Entity::SNS::Message result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Message SNSMongoRepository::getMessageById(const std::string &oid) const {
        return getMessageById(bsoncxx::oid(oid));
    }

    Entity::SNS::Message SNSMongoRepository::getMessageByMessageId(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_message");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            const auto mResult = _messageCollection.find_one(make_document(kvp("messageId", messageId)));
            Entity::SNS::Message result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SNSMongoRepository::countMessages(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_messages");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

            document query = {};
            if (!topicArn.empty()) {
                query.append(kvp("topicArn", topicArn));
            }

            const long count = _messageCollection.count_documents(query.view());
            log_trace << "Count messages, arn: " << topicArn << " result: " << count;
            return count;
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SNSMongoRepository::countMessagesByStatus(const std::string &topicArn, const Entity::SNS::MessageStatus status) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_messages");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            const long count = _messageCollection.count_documents(make_document(kvp("topicArn", topicArn), kvp("status", MessageStatusToString(status))));
            log_trace << "Count resources by state, arn: " << topicArn << " result: " << count;
            return count;

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }
    
    Entity::SNS::MessageList SNSMongoRepository::listMessages(const std::string &topicArn, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_messages");

        Entity::SNS::MessageList messageList;

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

        mongocxx::options::find opts;
        if (pageSize > 0) {
            opts.limit(pageSize);
        }
        if (pageIndex > 0) {
            opts.skip(pageIndex * pageSize);
        }
        document query = {};
        if (!prefix.empty()) {
            query.append(kvp("messageId", make_document(kvp("$regex", "^" + prefix))));
        }
        if (!topicArn.empty()) {
            query.append(kvp("topicArn", topicArn));
        }
        opts.sort(make_document(kvp("_id", 1)));
        if (!sortColumns.empty()) {
            document sort;
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        }

        for (auto messageCursor = _messageCollection.find(query.extract(), opts); auto message: messageCursor) {
            Entity::SNS::Message result;
            result.FromDocument(message);
            messageList.push_back(result);
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    Entity::SNS::Message SNSMongoRepository::updateMessage(Entity::SNS::Message &message) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_messages");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            message.modified = system_clock::now();
            const auto mResult = _messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts);
            log_trace << "Message updated, count: " << bsoncxx::to_json(mResult->view());

            if (mResult) {
                message.FromDocument(mResult->view());
                return message;
            }
            return {};
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Message SNSMongoRepository::createOrUpdateMessage(Entity::SNS::Message &message) const {
        if (messageExists(message.messageId)) {
            return updateMessage(message);
        }
        return createMessage(message);
    }

    void SNSMongoRepository::setMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "set_status_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            document filterQuery;
            filterQuery.append(kvp("_id", bsoncxx::oid{message.oid}));

            document setQuery;
            setQuery.append(kvp("status", Entity::SNS::MessageStatusToString(status)));
            setQuery.append(kvp("modified", bsoncxx::types::b_date(system_clock::now())));
            if (status == Entity::SNS::MessageStatus::SEND || status == Entity::SNS::MessageStatus::RESEND) {
                setQuery.append(kvp("lastSend", bsoncxx::types::b_date(system_clock::now())));
            }

            document updateQuery;
            updateQuery.append(kvp("$set", setQuery));

            _messageCollection.update_one(filterQuery.extract(), updateQuery.extract());
            log_trace << "SNS message status updated, oid: " << message.oid << ", status: " << MessageStatusToString(status);

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set SNS message status failed, error: " << e.what();
        }
    }

    long SNSMongoRepository::deleteMessage(const Entity::SNS::Message &message) const {
        return deleteMessage(message.messageId);
    }

    long SNSMongoRepository::deleteMessage(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            const auto result = _messageCollection.delete_one(make_document(kvp("messageId", messageId)));
            log_debug << "Messages deleted, messageId: " << messageId << " count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SNSMongoRepository::deleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

        array array{};
        for (const auto &receipt: messageIds) {
            array.append(receipt);
        }

        try {

            const auto result = _messageCollection.delete_many(make_document(kvp("region", region), kvp("topicArn", topicArn), kvp("messageId", make_document(kvp("$in", array)))));
            log_debug << "Messages deleted, count: " << result->result().deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void SNSMongoRepository::deleteOldMessages(const long timeout) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        const system_clock::time_point reset = system_clock::now() - std::chrono::days{timeout};

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = messageCollection.delete_many(make_document(kvp("created", make_document(kvp("$lt", bsoncxx::types::b_date(reset))))));
            if (static_cast<long>(result->deleted_count()) > 0) {
                log_debug << "Old messages deleted, timeout: " << timeout << " count: " << static_cast<long>(result->deleted_count());
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();
    }

    long SNSMongoRepository::deleteAllMessages() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        long deleted = 0;
        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            const auto result = _messageCollection.delete_many({});
            log_debug << "All resources deleted, count: " << result->deleted_count();
            deleted = result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }

        // Update monitoring counters
        adjustMessageCounters();

        return deleted;
    }

    void SNSMongoRepository::adjustMessageCounters() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "adjust_message_counter");
        using bsoncxx::builder::basic::make_array;

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];
        auto topicCollection = (*client)[_databaseName][_topicCollectionName];

        try {
            mongocxx::pipeline p{};
            p.group(make_document(
                    kvp("_id", "$topicArn"),
                    kvp("size", make_document(kvp("$sum", "$size"))),
                    kvp("total", make_document(kvp("$sum", 1))),
                    kvp("send", make_document(kvp("$sum",
                                                  make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$status", MessageStatusToString(Entity::SNS::MessageStatus::SEND)))), 1, 0)))))),
                    kvp("resend", make_document(kvp("$sum",
                                                    make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$status", MessageStatusToString(Entity::SNS::MessageStatus::RESEND)))), 1, 0))))))));

            document projectDocument;
            projectDocument.append(kvp("_id", 0),
                                   kvp("topicArn", "$_id"),
                                   kvp("total", 1),
                                   kvp("send", 1),
                                   kvp("resend", 1),
                                   kvp("size", 1));
            p.project(projectDocument.extract());

            // log_info << SqsUtils::ShowPipelineJson(p);


            // Get all queue ARNs from the aggregation result
            std::set<std::string> topicWithMessages;
            auto bulk = topicCollection.create_bulk_write();
            for (auto cursor = messageCollection.aggregate(p); const auto t: cursor) {
                const auto topicArn = Core::Bson::BsonUtils::GetStringValue(t, "topicArn");
                topicWithMessages.insert(topicArn);
                bulk.append(mongocxx::model::update_one(
                        make_document(kvp("topicArn", topicArn)),
                        make_document(kvp("$set", make_document(
                                                          kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                          kvp("messages", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "total"))),
                                                          kvp("messagesSend", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "send"))),
                                                          kvp("messagesResend", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "resend"))))))));
            }

            // Zero out queues with no messages
            for (auto queueCursor = topicCollection.find({}); const auto &q: queueCursor) {
                if (const auto topicArn = Core::Bson::BsonUtils::GetStringValue(q, "topicArn"); !topicWithMessages.contains(topicArn)) {
                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("topicArn", topicArn)),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64()),
                                                              kvp("messages", bsoncxx::types::b_int64()),
                                                              kvp("messagesSend", bsoncxx::types::b_int64()),
                                                              kvp("messagesResend", bsoncxx::types::b_int64()))))));
                }
            }

            if (!bulk.empty()) {
                auto result = bulk.execute();
                log_debug << "Bulk write result: " << result->modified_count();
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace Awsmock::Database
