//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SNSDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    SNSDatabase::SNSDatabase() : _memoryDb(SNSMemoryDb::instance()), _databaseName(GetDatabaseName()), _topicCollectionName("sns_topic"), _messageCollectionName("sns_message") {}

    bool SNSDatabase::TopicExists(const std::string &topicArn) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                int64_t count = _topicCollection.count_documents(make_document(kvp("topicArn", topicArn)));
                log_trace << "Topic exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.TopicExists(topicArn);
        }
    }

    bool SNSDatabase::TopicExists(const std::string &region, const std::string &topicName) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                int64_t
                        count = _topicCollection.count_documents(make_document(kvp("region", region), kvp("topicName", topicName)));
                log_trace << "Topic exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.TopicExists(region, topicName);
        }
    }

    Entity::SNS::Topic SNSDatabase::CreateTopic(Entity::SNS::Topic &topic) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _topicCollection.insert_one(topic.ToDocument());
                session.commit_transaction();
                log_trace << "Topic created, oid: " << result->inserted_id().get_oid().value.to_string();

                topic.oid = result->inserted_id().get_oid().value.to_string();
                return topic;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CreateTopic(topic);
        }
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(bsoncxx::oid oid) {

        try {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            mongocxx::stdx::optional<bsoncxx::document::value>
                    mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
            if (mResult) {

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

    Entity::SNS::Topic SNSDatabase::GetTopicById(const std::string &oid) {

        if (HasDatabase()) {

            return GetTopicById(bsoncxx::oid(oid));

        } else {

            return _memoryDb.GetTopicById(oid);
        }
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByArn(const std::string &topicArn) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("topicArn", topicArn)));

                Entity::SNS::Topic result;
                result.FromDocument(mResult->view());
                return result;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.GetTopicByArn(topicArn);
        }
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByName(const std::string &region, const std::string &topicName) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("region", region), kvp("topicName", topicName)));

                if (!mResult->empty()) {
                    Entity::SNS::Topic result;
                    result.FromDocument(mResult->view());
                    return result;
                } else {
                    log_warning << "Topic not found, region: " << region << " name: " << topicName;
                    return {};
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.GetTopicByName(region, topicName);
        }
    }

    Entity::SNS::TopicList SNSDatabase::GetTopicsBySubscriptionArn(const std::string &subscriptionArn) {

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                auto queueCursor = _topicCollection.find(make_document(kvp("subscriptions.subscriptionArn", subscriptionArn)));
                for (auto topic: queueCursor) {
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

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &region) {

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                if (region.empty()) {

                    auto queueCursor = _topicCollection.find({});
                    for (auto topic: queueCursor) {
                        Entity::SNS::Topic result;
                        result.FromDocument(topic);
                        topicList.push_back(result);
                    }

                } else {

                    auto queueCursor = _topicCollection.find(make_document(kvp("region", region)));
                    for (auto topic: queueCursor) {
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

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &prefix, int pageSize, int pageIndex, const std::vector<Core::SortColumn> &sortColumns, const std::string &region) {

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {

            try {

                mongocxx::options::find opts;

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

                bsoncxx::builder::basic::document query;
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
                    for (const auto &sortColumn: sortColumns) {
                        sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                auto queueCursor = _queueCollection.find(query.view(), opts);
                for (auto queue: queueCursor) {
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

    Entity::SNS::Topic SNSDatabase::UpdateTopic(Entity::SNS::Topic &topic) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            try {

                session.start_transaction();
                topic.modified = system_clock::now();
                auto mResult = _topicCollection.find_one_and_update(make_document(kvp("region", topic.region), kvp("topicArn", topic.topicArn)), topic.ToDocument(), opts);
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

    Entity::SNS::Topic SNSDatabase::CreateOrUpdateTopic(Entity::SNS::Topic &topic) {

        if (TopicExists(topic.region, topic.topicName)) {

            return UpdateTopic(topic);

        } else {

            return CreateTopic(topic);
        }
    }

    long SNSDatabase::CountTopics(const std::string &region, const std::string &prefix) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                bsoncxx::builder::basic::document query;

                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("topicName", make_document(kvp("$regex", "^" + prefix))));
                }

                long count = _topicCollection.count_documents(query.extract());
                log_trace << "Count topics, result: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CountTopics();
        }
    }

    void SNSDatabase::DeleteTopic(const Entity::SNS::Topic &topic) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
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

    void SNSDatabase::DeleteAllTopics() {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _topicCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All topics deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteAllTopics();
        }
    }

    bool SNSDatabase::MessageExists(const std::string &messageId) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                int64_t count = _messageCollection.count_documents(make_document(kvp("messageId", messageId)));
                log_trace << "Message exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.MessageExists(messageId);
        }
    }

    Entity::SNS::Message SNSDatabase::CreateMessage(Entity::SNS::Message &message) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _messageCollection.insert_one(message.ToDocument());
                log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();

                message.oid = result->inserted_id().get_oid().value.to_string();
                return message;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CreateMessage(message);
        }
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(bsoncxx::oid oid) {
        try {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            mongocxx::stdx::optional<bsoncxx::document::value> mResult = _messageCollection.find_one(make_document(kvp("_id", oid)));
            Entity::SNS::Message result;
            result.FromDocument(mResult->view());

            return result;
        } catch (const mongocxx::exception &exc) {
            log_error << "SNS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(const std::string &oid) {
        return GetMessageById(bsoncxx::oid(oid));
    }

    long SNSDatabase::CountMessages(const std::string &topicArn) {

        if (HasDatabase()) {

            try {

                long count;
                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

                bsoncxx::builder::basic::document query;
                if (!topicArn.empty()) {
                    query.append(kvp("topicArn", topicArn));
                }

                count = _messageCollection.count_documents(query.extract());
                log_trace << "Count messages, arn: " << topicArn << " result: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CountMessages(topicArn);
        }
    }

    long SNSDatabase::CountMessagesByStatus(const std::string &region, const std::string &topicArn, Entity::SNS::MessageStatus status) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                long count = _messageCollection.count_documents(make_document(kvp("region", region),
                                                                              kvp("topicArn", topicArn),
                                                                              kvp("status",
                                                                                  Entity::SNS::MessageStatusToString(status))));
                log_trace << "Count resources by state, region: " << region << " arn: " << topicArn
                          << " result: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CountMessagesByStatus(region, topicArn, status);
        }
    }

    long SNSDatabase::CountMessagesSize(const std::string &topicArn) {

        if (HasDatabase()) {

            try {

                long count;
                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

                mongocxx::pipeline p{};
                if (!topicArn.empty()) {
                    p.match(make_document(kvp("topicArn", topicArn)));
                }
                p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));

                auto totalSizeCursor = _messageCollection.aggregate(p);
                auto t = *totalSizeCursor.begin();
                if (!t.empty()) {
                    log_info << "Total size: " << t["totalSize"].get_int64().value;
                    return t["totalSize"].get_int64().value;
                }
                return 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CountMessages(topicArn);
        }
    }

    Entity::SNS::MessageList SNSDatabase::ListMessages(const std::string &region, const std::string &topicArn) {

        Entity::SNS::MessageList messageList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

            bsoncxx::builder::basic::document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            if (!topicArn.empty()) {
                query.append(kvp("topicArn", topicArn));
            }

            auto messageCursor = _messageCollection.find(query.extract());
            for (auto message: messageCursor) {
                Entity::SNS::Message result;
                result.FromDocument(message);
                messageList.push_back(result);
            }

        } else {

            messageList = _memoryDb.ListMessages(region, topicArn);
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    Entity::SNS::MessageList SNSDatabase::ListMessages(const std::string &region, const std::string &topicArn, int pageSize, int pageIndex) {

        Entity::SNS::MessageList messageList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

            mongocxx::options::find opts;
            if (pageSize > 0) {
                opts.limit(pageSize);
            }
            if (pageIndex > 0) {
                opts.skip(pageIndex * pageSize);
            }
            bsoncxx::builder::basic::document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!topicArn.empty()) {
                query.append(kvp("topicArn", topicArn));
            }

            auto messageCursor = _messageCollection.find(query.extract(), opts);
            for (auto message: messageCursor) {
                Entity::SNS::Message result;
                result.FromDocument(message);
                messageList.push_back(result);
            }

        } else {

            messageList = _memoryDb.ListMessages(region, topicArn);
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    Entity::SNS::Message SNSDatabase::UpdateMessage(Entity::SNS::Message &message) {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                message.modified = system_clock::now();
                auto mResult = _messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts);
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

        } else {

            return _memoryDb.UpdateMessage(message);
        }
    }

    Entity::SNS::Message SNSDatabase::CreateOrUpdateMessage(Entity::SNS::Message &message) {

        if (MessageExists(message.messageId)) {

            return UpdateMessage(message);

        } else {

            return CreateMessage(message);
        }
    }

    void SNSDatabase::DeleteMessage(const Entity::SNS::Message &message) {

        if (HasDatabase()) {

            DeleteMessage(message.messageId);

        } else {

            _memoryDb.DeleteMessage(message.messageId);
        }
    }

    void SNSDatabase::DeleteMessage(const std::string &messageId) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _messageCollection.delete_one(make_document(kvp("messageId", messageId)));
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

    void SNSDatabase::DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &receipts) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            bsoncxx::builder::basic::array array{};
            for (const auto &receipt: receipts) {
                array.append(receipt);
            }

            try {

                session.start_transaction();
                auto result = _messageCollection.delete_many(make_document(kvp("region", region),
                                                                           kvp("topicArn", topicArn),
                                                                           kvp("messageId", make_document(kvp("$in", array)))));
                log_debug << "Messages deleted, count: " << result->result().deleted_count();
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteMessages(region, topicArn, receipts);
        }
    }

    void SNSDatabase::DeleteOldMessages(long timeout) {

        const std::chrono::system_clock::time_point reset = std::chrono::system_clock::now() - std::chrono::days{timeout};

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = messageCollection.delete_many(make_document(kvp("created", make_document(kvp("$lt", bsoncxx::types::b_date(reset))))));
                session.commit_transaction();
                if (static_cast<long>(result->deleted_count()) > 0) {
                    log_debug << "Old messages deleted, timeout: " << timeout << " count: " << static_cast<long>(result->deleted_count());
                }

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

        } else {

            _memoryDb.DeleteOldMessages(timeout);
        }
    }

    void SNSDatabase::DeleteAllMessages() {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
                auto result = _messageCollection.delete_many({});
                log_debug << "All resources deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteAllMessages();
        }
    }

}// namespace AwsMock::Database
