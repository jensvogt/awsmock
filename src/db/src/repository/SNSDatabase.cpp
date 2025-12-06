//
// Created by vogje01 on 29/05/2023.
//

#include "../../../dto/include/awsmock/dto/sns/model/MessageStatus.h"
#include "awsmock/core/monitoring/MonitoringTimer.h"


#include <awsmock/repository/SNSDatabase.h>

namespace AwsMock::Database {

    SNSDatabase::SNSDatabase() : _databaseName(GetDatabaseName()), _topicCollectionName("sns_topic"), _messageCollectionName("sns_message"), _memoryDb(SNSMemoryDb::instance()) {}

    bool SNSDatabase::TopicExists(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_topic");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_topic");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "create_topic");

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

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            topic = _memoryDb.CreateTopic(topic);
        }
        return topic;
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(bsoncxx::oid oid) const {
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

    Entity::SNS::Topic SNSDatabase::GetTopicById(const std::string &oid) const {
        if (HasDatabase()) {
            return GetTopicById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetTopicById(oid);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByArn(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "get_topic");

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
                for (auto queueCursor = _topicCollection.find(make_document(kvp("subscriptions.subscriptionArn", subscriptionArn))); const auto topic: queueCursor) {
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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_topic");

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                for (auto queueCursor = _topicCollection.find(query.extract()); const auto topic: queueCursor) {
                    Entity::SNS::Topic result;
                    result.FromDocument(topic);
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

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_topic");

        Entity::SNS::TopicList topicList;
        if (HasDatabase()) {
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
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                for (auto topicCursor = _queueCollection.find(query.view(), opts); const auto &topic: topicCursor) {
                    Entity::SNS::Topic result;
                    result.FromDocument(topic);
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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "export_topics");

        if (HasDatabase()) {
            try {
                mongocxx::options::find opts;

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _queueCollection = (*client)[_databaseName][_topicCollectionName];

                const document query = {};

                opts.sort(make_document(kvp("_id", 1)));
                if (!sortColumns.empty()) {
                    document sort;
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_topics");

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

    void SNSDatabase::UpdateTopicCounter(const std::string &topicArn, const long messages, const long size, const long initial, const long send, const long resend) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_topics");

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();

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
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.UpdateTopicCounter(topicArn, messages, size, initial, send, resend);
    }

    void SNSDatabase::ImportTopic(Entity::SNS::Topic &topic) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "import_topics");

        topic.topicAttribute.availableMessages = 0;
        CreateOrUpdateTopic(topic);
        log_trace << "Topic imported, topicName: " << topic.topicName;
    }

    long SNSDatabase::CountTopics(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_topics");

        if (HasDatabase()) {
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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "purge_topics");

        long purged = 0;
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
                log_debug << "Topic purge, count: " << result->deleted_count();
                session.commit_transaction();
                purged = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            purged = _memoryDb.PurgeTopic(topic);
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return purged;
    }

    long SNSDatabase::GetTopicSize(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_topics");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_topics");

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

        // Update monitoring counters
        AdjustMessageCounters();
    }

    long SNSDatabase::DeleteAllTopics() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_topics");

        long deleted = 0;
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _topicCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All topics deleted, count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteAllTopics();
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return deleted;
    }

    bool SNSDatabase::MessageExists(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "exists_message");

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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "create_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto result = _messageCollection.insert_one(message.ToDocument());
                session.commit_transaction();
                log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();
                message.oid = result->inserted_id().get_oid().value.to_string();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            message = _memoryDb.CreateMessage(message);
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return message;
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(bsoncxx::oid oid) const {
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

    Entity::SNS::Message SNSDatabase::GetMessageById(const std::string &oid) const {
        return GetMessageById(bsoncxx::oid(oid));
    }

    Entity::SNS::Message SNSDatabase::GetMessageByMessageId(const std::string &messageId) const {
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

    long SNSDatabase::CountMessages(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_messages");

        if (HasDatabase()) {
            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];

                document query = {};
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

    long SNSDatabase::CountMessagesByStatus(const std::string &topicArn, const Entity::SNS::MessageStatus status) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_messages");

        if (HasDatabase()) {
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
        return _memoryDb.CountMessagesByStatus(topicArn, status);
    }

    long SNSDatabase::CountMessagesSize(const std::string &topicArn) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "count_messages");

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

    Entity::SNS::MessageList SNSDatabase::ListMessages(const std::string &region, const std::string &topicArn, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "list_messages");

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
            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!topicArn.empty()) {
                query.append(kvp("topicArn", topicArn));
            }
            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                document sort;
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
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
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "update_messages");

        if (HasDatabase()) {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                message.modified = system_clock::now();
                const auto mResult = _messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts);
                log_trace << "Message updated, count: " << bsoncxx::to_json(mResult->view());
                session.commit_transaction();

                if (mResult) {
                    message.FromDocument(mResult->view());
                    return message;
                }

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

    void SNSDatabase::SetMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "set_status_messages");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

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

                session.start_transaction();
                _messageCollection.update_one(filterQuery.extract(), updateQuery.extract());
                session.commit_transaction();
                log_trace << "SNS message status updated, oid: " << message.oid << ", status: " << MessageStatusToString(status);

            } catch (mongocxx::exception::system_error &e) {
                session.abort_transaction();
                log_error << "Set SNS message status failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetMessageStatus(message, status);
        }
    }

    void SNSDatabase::DeleteMessage(const Entity::SNS::Message &message) const {

        if (HasDatabase()) {

            DeleteMessage(message.messageId);

        } else {
            _memoryDb.DeleteMessage(message.messageId);
        }

        // Update monitoring counters
        AdjustMessageCounters();
    }

    void SNSDatabase::DeleteMessage(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

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

        // Update monitoring counters
        AdjustMessageCounters();
    }

    void SNSDatabase::DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            array array{};
            for (const auto &receipt: messageIds) {
                array.append(receipt);
            }

            try {

                session.start_transaction();
                const auto result = _messageCollection.delete_many(make_document(kvp("region", region), kvp("topicArn", topicArn), kvp("messageId", make_document(kvp("$in", array)))));
                log_debug << "Messages deleted, count: " << result->result().deleted_count();
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SNS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteMessages(region, topicArn, messageIds);
        }

        // Update monitoring counters
        AdjustMessageCounters();
    }

    void SNSDatabase::DeleteOldMessages(const long timeout) const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

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

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            _memoryDb.DeleteOldMessages(timeout);
        }

        // Update monitoring counters
        AdjustMessageCounters();
    }

    long SNSDatabase::DeleteAllMessages() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "delete_messages");

        long deleted = 0;
        if (HasDatabase()) {
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
        } else {
            deleted = _memoryDb.DeleteAllMessages();
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return deleted;
    }

    void SNSDatabase::AdjustMessageCounters() const {
        Monitoring::MonitoringTimer measure(SNS_DATABASE_TIMER, SNS_DATABASE_COUNTER, "action", "adjust_message_counter");
        using bsoncxx::builder::basic::make_array;

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto topicCollection = (*client)[_databaseName][_topicCollectionName];
            auto session = client->start_session();

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

                session.start_transaction();

                // Initialize all topics with zero message counts
                topicCollection.update_many({}, make_document(kvp("$set", make_document(
                                                                          kvp("size", bsoncxx::types::b_int64()),
                                                                          kvp("messages", bsoncxx::types::b_int64()),
                                                                          kvp("messagesSend", bsoncxx::types::b_int64()),
                                                                          kvp("messagesResend", bsoncxx::types::b_int64())))));

                auto bulk = topicCollection.create_bulk_write();
                for (auto cursor = messageCollection.aggregate(p); const auto t: cursor) {
                    bulk.append(mongocxx::model::update_one(make_document(kvp("topicArn", Core::Bson::BsonUtils::GetStringValue(t, "topicArn"))),
                                                            make_document(kvp("$set", make_document(
                                                                                      kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                                                      kvp("messages", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "total"))),
                                                                                      kvp("messagesSend", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "send"))),
                                                                                      kvp("messagesResend", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "resend"))))))));
                    log_debug << "Topic: " << Core::Bson::BsonUtils::GetStringValue(t, "topicArn")
                              << ", size: " << Core::Bson::BsonUtils::GetLongValue(t, "size")
                              << ", messages: " << Core::Bson::BsonUtils::GetLongValue(t, "total")
                              << ", messagesSend: " << Core::Bson::BsonUtils::GetLongValue(t, "send")
                              << ", messagesResend: " << Core::Bson::BsonUtils::GetLongValue(t, "resend");
                }

                // Bul updates
                if (!bulk.empty()) {
                    try {
                        auto result = bulk.execute();
                        log_debug << "Bulk write result: " << result->modified_count();
                    } catch (const mongocxx::exception &exc) {
                        log_error << "Bulk write failed: " << exc.what();
                        throw Core::DatabaseException(exc.what());
                    }
                }
                session.commit_transaction();


            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
    }
}// namespace AwsMock::Database
