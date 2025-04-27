//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SNSDatabase.h>

namespace AwsMock::Database {
    SNSDatabase::SNSDatabase() : _databaseName(GetDatabaseName()), _topicCollectionName("sns_topic"),
                                 _messageCollectionName("sns_message"), _memoryDb(SNSMemoryDb::instance()) {
    }

    bool SNSDatabase::TopicExists(const std::string &topicArn) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                const int64_t count = _topicCollection.count_documents(make_document(kvp("topicArn", topicArn)));
                log_trace << "Topic exists: " << std::boolalpha << count;
                return count > 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.TopicExists(topicArn);
    }

    bool SNSDatabase::TopicExists(const std::string &region, const std::string &topicName) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                const int64_t count = _topicCollection.count_documents(make_document(kvp("region", region), kvp("topicName", topicName)));
                log_trace << "Topic exists: " << std::boolalpha << count;
                return count > 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.TopicExists(region, topicName);
    }

    Entity::SNS::Topic SNSDatabase::CreateTopic(Entity::SNS::Topic &topic) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _topicCollection.insert_one(topic.ToDocument());
                session.commit_transaction();
                log_trace << "Topic created, oid: " << result->inserted_id().get_oid().value.to_string();

                topic.oid = result->inserted_id().get_oid().value.to_string();
                return topic;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CreateTopic(topic);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(bsoncxx::oid oid) const {

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

    Entity::SNS::Topic SNSDatabase::GetTopicById(const std::string &oid) const {
        if (HasDatabase()) {
            return GetTopicById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetTopicById(oid);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByArn(const std::string &topicArn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                if (const auto mResult = _topicCollection.find_one(make_document(kvp("topicArn", topicArn))); !mResult->empty()) {
                    Entity::SNS::Topic result;
                    result.FromDocument(mResult->view());
                    return result;
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetTopicByArn(topicArn);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByTargetArn(const std::string &targetArn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                if (const auto mResult = _topicCollection.find_one(
                            make_document(kvp("targetArn", targetArn)));
                    !mResult->empty()) {
                    Entity::SNS::Topic result;
                    result.FromDocument(mResult->view());
                    return result;
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetTopicByTargetArn(targetArn);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByName(const std::string &region, const std::string &topicName) const {
        if (HasDatabase()) {
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
        return _memoryDb.GetTopicByName(region, topicName);
    }

    Entity::SNS::TopicList SNSDatabase::GetTopicsBySubscriptionArn(const std::string &subscriptionArn) const {
        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                for (auto queueCursor = _topicCollection.find(
                             make_document(kvp("subscriptions.subscriptionArn", subscriptionArn)));
                     auto topic:
                     queueCursor) {
                    Entity::SNS::Topic result;
                    result.FromDocument(topic);
                    topicList.push_back(result);
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            topicList = _memoryDb.GetTopicsBySubscriptionArn(subscriptionArn);
        }
        return topicList;
    }

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &region) const {
        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                if (region.empty()) {
                    for (auto queueCursor = _topicCollection.find({}); auto topic: queueCursor) {
                        Entity::SNS::Topic result;
                        result.FromDocument(topic);
                        topicList.push_back(result);
                    }
                } else {
                    for (auto queueCursor = _topicCollection.find(make_document(kvp("region", region))); auto topic:
                                                                                                         queueCursor) {
                        Entity::SNS::Topic result;
                        result.FromDocument(topic);
                        topicList.push_back(result);
                    }
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            topicList = _memoryDb.ListTopics(region);
        }
        log_trace << "Got topic list, size:" << topicList.size();
        return topicList;
    }

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &prefix, const int pageSize, const int pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {
        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
            try {
                mongocxx::options::find opts;

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

                bsoncxx::builder::basic::document query = {};
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
                    bsoncxx::builder::basic::document sort;
                    for (const auto sortColumn: sortColumns) {
                        sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                for (auto queueCursor = _queueCollection.find(query.view(), opts); auto queue: queueCursor) {
                    Entity::SNS::Topic result;
                    result.FromDocument(queue);
                    topicList.push_back(result);
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            topicList = _memoryDb.ListTopics(region);
        }
        log_trace << "Got topic list, size:" << topicList.size();
        return topicList;
    }

    Entity::SNS::TopicList SNSDatabase::ExportTopics(const std::vector<SortColumn> &sortColumns) const {
        if (HasDatabase()) {
            try {
                mongocxx::options::find opts;

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

                const bsoncxx::builder::basic::document query = {};

                opts.sort(make_document(kvp("_id", 1)));
                if (!sortColumns.empty()) {
                    bsoncxx::builder::basic::document sort;
                    for (const auto sortColumn: sortColumns) {
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
        return _memoryDb.ExportTopics(sortColumns);
    }

    Entity::SNS::Topic SNSDatabase::UpdateTopic(Entity::SNS::Topic &topic) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            try {
                session.start_transaction();
                topic.modified = system_clock::now();
                const auto mResult = _topicCollection.find_one_and_update(make_document(kvp("region", topic.region), kvp("topicArn", topic.topicArn)), topic.ToDocument(), opts);
                log_trace << "Topic updated: " << topic.ToString();
                session.commit_transaction();

                if (mResult) {
                    topic.FromDocument(mResult->view());
                    return topic;
                }
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            return _memoryDb.UpdateTopic(topic);
        }
        return {};
    }

    Entity::SNS::Topic SNSDatabase::CreateOrUpdateTopic(Entity::SNS::Topic &topic) const {
        if (TopicExists(topic.region, topic.topicName)) {
            return UpdateTopic(topic);
        }
        return CreateTopic(topic);
    }

    void SNSDatabase::ImportTopic(Entity::SNS::Topic &topic) const {

        topic.topicAttribute.availableMessages = 0;
        CreateOrUpdateTopic(topic);
        log_trace << "Topic imported, topicName: " << topic.topicName;
    }

    long SNSDatabase::CountTopics(const std::string &region, const std::string &prefix) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                bsoncxx::builder::basic::document query = {};

                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("topicName", make_document(kvp("$regex", "^" + prefix))));
                }

                const long count = _topicCollection.count_documents(query.extract());
                log_trace << "Count topics, result: " << count;
                return count;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountTopics();
    }

    long SNSDatabase::PurgeTopic(const Entity::SNS::Topic &topic) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
                log_debug << "Topic purge, count: " << result->deleted_count();
                session.commit_transaction();

                // Adjust all topic message counters
                AdjustMessageCounters(topic.topicArn);

                return result->deleted_count();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.PurgeTopic(topic);
    }

    long SNSDatabase::GetTopicSize(const std::string &topicArn) const {
        if (HasDatabase()) {
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
        return _memoryDb.GetTopicSize(topicArn);
    }

    void SNSDatabase::DeleteTopic(const Entity::SNS::Topic &topic) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
                log_debug << "Topic deleted, count: " << result->deleted_count();
                session.commit_transaction();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteTopic(topic);
        }
    }

    long SNSDatabase::DeleteAllTopics() const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _topicCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All topics deleted, count: " << result->deleted_count();
                return result->deleted_count();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            return _memoryDb.DeleteAllTopics();
        }
    }

    bool SNSDatabase::MessageExists(const std::string &messageId) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                const int64_t count = _messageCollection.count_documents(make_document(kvp("messageId", messageId)));
                log_trace << "Message exists: " << std::boolalpha << count;
                return count > 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.MessageExists(messageId);
    }

    Entity::SNS::Message SNSDatabase::CreateMessage(Entity::SNS::Message &message) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _messageCollection.insert_one(message.ToDocument());
                log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();

                // Adjust all topic message counters
                AdjustMessageCounters(message.topicArn);

                message.oid = result->inserted_id().get_oid().value.to_string();
                return message;
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CreateMessage(message);
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(bsoncxx::oid oid) const {
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

    Entity::SNS::Message SNSDatabase::GetMessageById(const std::string &oid) const {
        return GetMessageById(bsoncxx::oid(oid));
    }

    long SNSDatabase::CountMessages(const std::string &topicArn) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

                bsoncxx::builder::basic::document query = {};
                if (!topicArn.empty()) {
                    query.append(kvp("topicArn", topicArn));
                }

                const long count = _messageCollection.count_documents(query.extract());
                log_trace << "Count messages, arn: " << topicArn << " result: " << count;
                return count;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountMessages(topicArn);
    }

    long SNSDatabase::CountMessagesByStatus(const std::string &region,
                                            const std::string &topicArn,
                                            Entity::SNS::MessageStatus status) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                const long count = _messageCollection.count_documents(make_document(
                        kvp("region", region),
                        kvp("topicArn", topicArn),
                        kvp("status", MessageStatusToString(status))));
                log_trace << "Count resources by state, region: " << region << " arn: " << topicArn << " result: " << count;
                return count;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountMessagesByStatus(region, topicArn, status);
    }

    long SNSDatabase::CountMessagesSize(const std::string &topicArn) const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

                mongocxx::pipeline p{};
                if (!topicArn.empty()) {
                    p.match(make_document(kvp("topicArn", topicArn)));
                }
                p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));

                auto totalSizeCursor = _messageCollection.aggregate(p);
                if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                    log_info << "Total size: " << t["totalSize"].get_int64().value;
                    return t["totalSize"].get_int64().value;
                }
                return 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountMessages(topicArn);
    }

    Entity::SNS::MessageList SNSDatabase::ListMessages(const std::string &region,
                                                       const std::string &topicArn,
                                                       int pageSize,
                                                       int pageIndex,
                                                       const std::vector<SortColumn> &sortColumns) const {
        if (HasDatabase()) {
            Entity::SNS::MessageList messageList;

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

            mongocxx::options::find opts;
            if (pageSize > 0) {
                opts.limit(pageSize);
            }
            if (pageIndex > 0) {
                opts.skip(pageIndex * pageSize);
            }
            bsoncxx::builder::basic::document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!topicArn.empty()) {
                query.append(kvp("topicArn", topicArn));
            }
            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                bsoncxx::builder::basic::document sort;
                for (const auto sortColumn: sortColumns) {
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
        return _memoryDb.ListMessages(region, topicArn);
    }

    Entity::SNS::Message SNSDatabase::UpdateMessage(Entity::SNS::Message &message) const {
        if (HasDatabase()) {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                message.modified = system_clock::now();
                const auto mResult = _messageCollection.find_one_and_update(
                        make_document(kvp("_id", bsoncxx::oid{message.oid})),
                        message.ToDocument(),
                        opts);
                log_trace << "Message updated, count: " << bsoncxx::to_json(mResult->view());
                session.commit_transaction();

                if (!mResult) {
                    throw Core::DatabaseException("Update message failed, oid: " + message.oid);
                }

                message.FromDocument(mResult->view());
                return message;
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateMessage(message);
    }

    Entity::SNS::Message SNSDatabase::CreateOrUpdateMessage(Entity::SNS::Message &message) const {
        if (MessageExists(message.messageId)) {
            return UpdateMessage(message);
        }
        return CreateMessage(message);
    }

    void SNSDatabase::DeleteMessage(const Entity::SNS::Message &message) const {
        if (HasDatabase()) {
            DeleteMessage(message.messageId);

            // Adjust all topic message counters
            AdjustMessageCounters(message.topicArn);
        } else {
            _memoryDb.DeleteMessage(message.messageId);
        }
    }

    void SNSDatabase::DeleteMessage(const std::string &messageId) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _messageCollection.delete_one(make_document(kvp("messageId", messageId)));
                log_debug << "Messages deleted, messageId: " << messageId << " count: " << result->deleted_count();
                session.commit_transaction();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteMessage(messageId);
        }
    }

    void SNSDatabase::DeleteMessages(const std::string &region,
                                     const std::string &topicArn,
                                     const std::vector<std::string> &receipts) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            array array{};
            for (const auto &receipt: receipts) {
                array.append(receipt);
            }

            try {
                session.start_transaction();
                const auto result = _messageCollection.delete_many(make_document(kvp("region", region),
                                                                                 kvp("topicArn", topicArn),
                                                                                 kvp("messageId", make_document(kvp("$in", array)))));
                log_debug << "Messages deleted, count: " << result->result().deleted_count();
                session.commit_transaction();

                // Adjust all topic message counters
                AdjustMessageCounters(topicArn);
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteMessages(region, topicArn, receipts);
        }
    }

    void SNSDatabase::DeleteOldMessages(const long timeout) const {
        const system_clock::time_point reset = system_clock::now() - std::chrono::days{timeout};

        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = messageCollection.delete_many(make_document(kvp("created", make_document(kvp("$lt", bsoncxx::types::b_date(reset))))));
                session.commit_transaction();
                if (static_cast<long>(result->deleted_count()) > 0) {
                    log_debug << "Old messages deleted, timeout: " << timeout << " count: " << static_cast<long>(result->deleted_count());
                }

                // Adjust all topic message counters
                AdjustAllMessageCounters();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteOldMessages(timeout);
        }
    }

    long SNSDatabase::DeleteAllMessages() const {
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                const auto result = _messageCollection.delete_many({});
                log_debug << "All resources deleted, count: " << result->deleted_count();

                // Adjust all topic message counters
                AdjustAllMessageCounters();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteAllMessages();
    }

    void SNSDatabase::AdjustAllMessageCounters() const {
        if (HasDatabase()) {
            const Entity::SNS::TopicList topicList = ListTopics();
            log_trace << "SNS adjust counter starting, count: " << topicList.size();

            // Loop over topics and synchronize counters
            for (auto &topic: topicList) {
                AdjustMessageCounters(topic.topicArn);
            }
        }
    }

    void SNSDatabase::AdjustMessageCounters(const std::string &topicArn) const {
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            auto topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {
                long total = CountMessages(topicArn);
                long size = GetTopicSize(topicArn);

                session.start_transaction();
                topicCollection.update_one(make_document(kvp("topicArn", topicArn)),
                                           make_document(kvp("$set",
                                                             make_document(
                                                                     kvp("size", bsoncxx::types::b_int64(size)),
                                                                     kvp("attributes.availableMessages", bsoncxx::types::b_int64(total))))));
                log_debug << topicArn << " total: " << total;
                session.commit_transaction();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
    }
}// namespace AwsMock::Database
