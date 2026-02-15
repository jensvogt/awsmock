//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SQSDatabase.h>

#include <bsoncxx/builder/stream/helpers.hpp>

namespace AwsMock::Database {

    SQSDatabase::SQSDatabase() : _databaseName(GetDatabaseName()), _queueCollectionName("sqs_queue"), _messageCollectionName("sqs_message"), _memoryDb(SQSMemoryDb::instance()) {}

    bool SQSDatabase::QueueExists(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _queueCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;
        }
        return _memoryDb.QueueExists(region, name);
    }

    bool SQSDatabase::QueueUrlExists(const std::string &region, const std::string &queueUrl) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!queueUrl.empty()) {
                query.append(kvp("queueUrl", queueUrl));
            }

            return _queueCollection.count_documents(query.extract(), options) > 0;
        }
        return _memoryDb.QueueUrlExists(region, queueUrl);
    }

    bool SQSDatabase::QueueArnExists(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _queueCollection.count_documents(make_document(kvp("queueArn", queueArn)), options) > 0;
        }
        return _memoryDb.QueueArnExists(queueArn);
    }

    Entity::SQS::Queue SQSDatabase::CreateQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "create_exists");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _queueCollection.insert_one(queue.ToDocument());
                log_trace << "Queue created, oid: " << result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();

                queue.oid = result->inserted_id().get_oid().value.to_string();
                return queue;

            } catch (mongocxx::exception &e) {
                session.abort_transaction();
                log_error << "Collection transaction exception: " << e.what();
                throw Core::DatabaseException("Insert queue failed, region: " + queue.region + " queueUrl: " + queue.queueUrl + " message: " + e.what());
            }
        }
        return _memoryDb.CreateQueue(queue);
    }

    Entity::SQS::Queue SQSDatabase::GetQueueById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

        const auto mResult = _queueCollection.find_one(make_document(kvp("_id", oid)));
        if (mResult->empty()) {
            log_error << "Queue not found, oid: " << oid.to_string();
            throw Core::DatabaseException("Queue not found, oid: " + oid.to_string());
        }

        Entity::SQS::Queue result;
        result.FromDocument(mResult->view());
        return result;
    }

    Entity::SQS::Queue SQSDatabase::GetQueueById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetQueueById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetQueueById(oid);
    }

    Entity::SQS::Queue SQSDatabase::GetQueueByArn(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        Entity::SQS::Queue queue;
        if (HasDatabase()) {
            Entity::SQS::Queue result;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            const auto mResult = _queueCollection.find_one(make_document(kvp("queueArn", queueArn)));

            if (!mResult) {
                log_error << "Queue not found, queueArn: " << queueArn;
                throw Core::DatabaseException("Queue not found, queueArn: " + queueArn);
            }
            queue = result.FromDocument(mResult->view());
        } else {
            queue = _memoryDb.GetQueueByArn(queueArn);
        }
        return queue;
    }

    Entity::SQS::Queue SQSDatabase::GetQueueByDlq(const std::string &dlqQueueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        if (HasDatabase()) {
            Entity::SQS::Queue result;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            const auto mResult = _queueCollection.find_one(make_document(kvp("attributes.redrivePolicy.deadLetterTargetArn", dlqQueueArn)));

            if (!mResult) {
                log_error << "Queue not found, queueArn: " << dlqQueueArn;
                throw Core::DatabaseException("Queue not found, queueArn: " + dlqQueueArn);
            }
            return result.FromDocument(mResult->view());
        }
        return _memoryDb.GetQueueByDlq(dlqQueueArn);
    }

    Entity::SQS::Queue SQSDatabase::GetQueueByUrl(const std::string &region, const std::string &queueUrl) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!queueUrl.empty()) {
                query.append(kvp("queueUrl", queueUrl));
            }
            if (const auto mResult = _queueCollection.find_one(query.extract()); !mResult) {
                log_error << "Queue not found, region: " << region << " queueUrl: " << queueUrl;
                throw Core::DatabaseException("Queue not found, region: " + region + " queueUrl: " + queueUrl);
            } else {
                Entity::SQS::Queue result;
                return result.FromDocument(mResult->view());
            }
        }
        return _memoryDb.GetQueueByUrl(queueUrl);
    }

    Entity::SQS::Queue SQSDatabase::GetQueueByName(const std::string &region, const std::string &queueName) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!queueName.empty()) {
                query.append(kvp("name", queueName));
            }

            if (const auto mResult = _queueCollection.find_one(query.extract()); !mResult) {

                log_warning << "GetQueueByName failed, region: " << region << " name: " << queueName;
                throw Core::DatabaseException("Queue not found, region: " + region + " name: " + queueName);

            } else {
                Entity::SQS::Queue result;
                log_info << "GetQueueByName succeeded, region: " << region << " name: " << queueName;
                return result.FromDocument(mResult->view());
            }
        }
        return _memoryDb.GetQueueByName(region, queueName);
    }

    Entity::SQS::QueueList SQSDatabase::ListQueues(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_queues");

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            document query = {};
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
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

            for (auto queueCursor = _queueCollection.find(query.view(), opts); auto queue: queueCursor) {
                Entity::SQS::Queue result;
                result.FromDocument(queue);
                queueList.push_back(result);
            }
            log_trace << "Got queue list, size: " << queueList.size();
            return queueList;
        }
        return _memoryDb.ListQueues(region);
    }

    Entity::SQS::QueueList SQSDatabase::ExportQueues(const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "export_queues");

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            const document query = {};
            if (!sortColumns.empty()) {
                document sort;
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            } else {
                opts.sort(make_document(kvp("_id", 1)));
            }

            for (auto queueCursor = _queueCollection.find(query.view(), opts); auto queue: queueCursor) {
                Entity::SQS::Queue result;
                result.FromDocument(queue);
                queueList.push_back(result);
            }
            log_trace << "Got queue list, size: " << queueList.size();
            return queueList;
        }
        return _memoryDb.ExportQueues(sortColumns);
    }

    void SQSDatabase::ImportQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "import_queues");

        queue.queueUrl = Core::CreateSQSQueueUrl(queue.name);
        queue.modified = system_clock::now();
        queue.attributes.approximateNumberOfMessages = 0;
        queue.attributes.approximateNumberOfMessagesDelayed = 0;
        queue.attributes.approximateNumberOfMessagesNotVisible = 0;
        CreateOrUpdateQueue(queue);
        log_trace << "Queue imported, queueName: " << queue.name;
    }

    Entity::SQS::QueueList SQSDatabase::ListQueues(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_queues");

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            for (auto queueCursor = _queueCollection.find(query.extract()); auto queue: queueCursor) {
                Entity::SQS::Queue result;
                result.FromDocument(queue);
                queueList.push_back(result);
            }
            log_trace << "Got queue list, size: " << queueList.size();
            return queueList;
        }
        return _memoryDb.ListQueues(region);
    }

    long SQSDatabase::PurgeQueue(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "purge_queue");

        long purged = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = messageCollection.delete_many(make_document(kvp("queueArn", queueArn)));
                session.commit_transaction();
                log_debug << "Purged queue, count: " << result->deleted_count() << " queueArn: " << queueArn;

                purged = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            purged = _memoryDb.PurgeQueue(queueArn);
        }
        return purged;
    }

    Entity::SQS::Queue SQSDatabase::UpdateQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "update_queue");

        queue.modified = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            try {

                session.start_transaction();
                const auto mResult = _queueCollection.find_one_and_update(make_document(kvp("queueArn", queue.queueArn)), queue.ToDocument(), opts);
                session.commit_transaction();
                log_trace << "Queue updated: " << Core::Bson::BsonUtils::ToJsonString(queue.ToDocument());

                if (mResult) {
                    queue.FromDocument(mResult->view());
                    return queue;
                }

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.UpdateQueue(queue);
        }
        return {};
    }

    Entity::SQS::Queue SQSDatabase::CreateOrUpdateQueue(Entity::SQS::Queue &queue) const {

        if (QueueArnExists(queue.queueArn)) {

            return UpdateQueue(queue);
        }
        return CreateQueue(queue);
    }

    void SQSDatabase::UpdateQueueCounter(const std::string &queueArn, const long messages, const long size, const long initial, const long invisible, const long delayed) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "update_queue");

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();

                document filterQuery;
                filterQuery.append(kvp("queueArn", queueArn));

                document setQuery;
                setQuery.append(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));
                setQuery.append(kvp("attributes.approximateNumberOfMessages", static_cast<bsoncxx::types::b_int64>(messages)));
                setQuery.append(kvp("attributes.approximateNumberOfMessagesDelayed", static_cast<bsoncxx::types::b_int64>(delayed)));
                setQuery.append(kvp("attributes.approximateNumberOfMessagesNotVisible", static_cast<bsoncxx::types::b_int64>(invisible)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                _bucketCollection.update_one(filterQuery.extract(), updateQuery.extract());
                log_trace << "Bucket counter updated";
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.UpdateQueueCounter(queueArn, messages, size, initial, invisible, delayed);
    }

    long SQSDatabase::CountQueues(const std::string &prefix, const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_queues");

        long count;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];

            document query = {};
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }

            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            count = _queueCollection.count_documents(query.extract());

        } else {

            return _memoryDb.CountQueues(prefix, region);
        }
        log_trace << "Count queues, result: " << count;
        return count;
    }

    long SQSDatabase::GetQueueSize(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_queues");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = client->database(_databaseName)[_messageCollectionName];

            try {
                mongocxx::pipeline p{};
                p.match(make_document(kvp("queueArn", queueArn)));
                p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
                if (auto totalSizeCursor = _objectCollection.aggregate(p); !totalSizeCursor.begin()->empty()) {
                    if (const auto t = *totalSizeCursor.begin(); !t.empty() && t["totalSize"].type() == bsoncxx::type::k_int64) {
                        return t["totalSize"].get_int64().value;
                    }
                }
                return 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetQueueSize(queueArn);
    }

    long SQSDatabase::DeleteQueue(const Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_queue");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _queueCollection.delete_many(make_document(kvp("region", queue.region), kvp("queueUrl", queue.queueUrl)));
                session.commit_transaction();
                log_debug << "Queue deleted, count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteQueue(queue);
        }
        return deleted;
    }

    long SQSDatabase::DeleteAllQueues() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_all_queues");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _queueCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All queues deleted, count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {
            deleted = _memoryDb.DeleteAllQueues();
        }
        return deleted;
    }

    Entity::SQS::Message SQSDatabase::CreateMessage(Entity::SQS::Message &message) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "create_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = messageCollection.insert_one(message.ToDocument());
                session.commit_transaction();
                log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();

                message.oid = result->inserted_id().get_oid().value.to_string();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            message = _memoryDb.CreateMessage(message);
        }
        return message;
    }

    bool SQSDatabase::MessageExists(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "exists_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            try {
                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                document query;
                query.append(kvp("receiptHandle", receiptHandle));

                // 3. If count > 0, it exists
                return messageCollection.count_documents(query.view(), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.MessageExists(receiptHandle);
    }

    bool SQSDatabase::MessageExistsByMessageId(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "exists_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            try {

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                return messageCollection.count_documents(make_document(kvp("messageId", messageId)), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.MessageExistsByMessageId(messageId);
    }

    Entity::SQS::Message SQSDatabase::GetMessageById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = client->database(_databaseName)[_messageCollectionName];

        const auto mResult = messageCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SQS::Message result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::SQS::Message SQSDatabase::GetMessageByReceiptHandle(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            if (const auto mResult = messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle)))) {
                Entity::SQS::Message result;
                result.FromDocument(mResult->view());
                return result;
            }
            return {};
        }
        return _memoryDb.GetMessageByReceiptHandle(receiptHandle);
    }

    Entity::SQS::Message SQSDatabase::GetMessageByMessageId(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            if (const auto mResult = messageCollection.find_one(make_document(kvp("messageId", messageId)))) {
                Entity::SQS::Message result;
                result.FromDocument(mResult->view());
                return result;
            }
            return {};
        }
        return _memoryDb.GetMessageByMessageId(messageId);
    }

    Entity::SQS::Message SQSDatabase::GetMessageById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetMessageById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetMessageById(oid);
    }

    Entity::SQS::Message SQSDatabase::UpdateMessage(Entity::SQS::Message &message) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "update_message");

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                message.modified = system_clock::now();
                const auto mResult = messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    message.FromDocument(mResult->view());
                    log_trace << "Message updated: " << Core::Bson::BsonUtils::ToJsonString(message.ToDocument());
                    return message;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateMessage(message);
    }

    Entity::SQS::Message SQSDatabase::CreateOrUpdateMessage(Entity::SQS::Message &message) const {

        if (MessageExists(message.receiptHandle)) {

            return UpdateMessage(message);
        }
        return CreateMessage(message);
    }

    Entity::SQS::MessageList SQSDatabase::ListMessages(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_messages");

        if (HasDatabase()) {

            Entity::SQS::MessageList messageList;

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            for (auto messageCursor = messageCollection.find(query.extract()); auto &message: messageCursor) {
                Entity::SQS::Message result;
                result.FromDocument(message);
                messageList.push_back(result);
            }
            log_trace << "Got message list, size: " << messageList.size();
            return messageList;
        }
        return _memoryDb.ListMessages(region);
    }

    Entity::SQS::MessageList SQSDatabase::ListMessages(const std::string &queueArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_messages");

        Entity::SQS::MessageList messageList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            mongocxx::options::find opts;
            if (pageSize > 0) {
                opts.limit(pageSize);
                if (pageIndex > 0) {
                    opts.skip(pageSize * pageIndex);
                }
            }
            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                document sort;
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            }

            document query = {};
            if (!prefix.empty()) {
                query.append(kvp("messageId", make_document(kvp("$regex", "^" + prefix))));
            }

            if (!queueArn.empty()) {
                query.append(kvp("queueArn", queueArn));
            }

            for (auto messageCursor = messageCollection.find(query.extract(), opts); auto message: messageCursor) {
                Entity::SQS::Message result;
                result.FromDocument(message);
                messageList.push_back(result);
            }
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    void SQSDatabase::ImportMessages(const std::string &queueArn, const value &messageArray) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "import_messages");

        Entity::SQS::MessageList messageList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            const auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                auto collection = mongocxx::collection{messageCollection};
                auto bulk = collection.create_bulk_write();

                for (const auto &message: messageArray) {
                    const mongocxx::model::insert_one insert_op{message.get_document().view()};
                    bulk.append(insert_op);
                }
                const auto result = bulk.execute();
                session.commit_transaction();
                log_info << "Imported messages: " << result->inserted_count();

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
                throw Core::DatabaseException(e.what());
            }
        }
    }

    void SQSDatabase::ReceiveMessages(const std::string &queueArn, const long visibility, const long maxResult, const std::string &dlQueueArn, const long maxRetries, Entity::SQS::MessageList &messageList) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "receive_messages");

        const auto reset = system_clock::now() + std::chrono::seconds(visibility);

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                mongocxx::options::find opts;
                if (maxResult > 0) {
                    opts.limit(maxResult);
                }

                std::string dlqQueueUrl;
                std::string dlqQueueName;
                if (!dlQueueArn.empty()) {
                    Core::AwsUtils::ConvertSQSQueueArnToUrl(dlQueueArn);
                    Core::AwsUtils::ConvertSQSQueueArnToName(dlQueueArn);
                }

                auto queueBulk = messageCollection.create_bulk_write();
                auto dqlBulk = messageCollection.create_bulk_write();

                // Get the cursor
                for (auto messageCursor = messageCollection.find(make_document(kvp("queueArn", queueArn), kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))), opts); auto message: messageCursor) {

                    Entity::SQS::Message result;
                    result.FromDocument(message);

                    // Check retries
                    result.retries++;
                    if (!dlQueueArn.empty() && maxRetries > 0 && result.retries >= maxRetries) {

                        document filterQuery;
                        filterQuery.append(kvp("_id", message["_id"].get_oid()));

                        document setQuery;
                        setQuery.append(kvp("queueArn", dlQueueArn));
                        setQuery.append(kvp("queueUrl", dlqQueueUrl));
                        setQuery.append(kvp("queueName", dlqQueueName));
                        setQuery.append(kvp("receiptHandle", ""));
                        setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

                        document updateQuery;
                        updateQuery.append(kvp("$set", setQuery));

                        dqlBulk.append(mongocxx::model::update_one{filterQuery.extract(), updateQuery.extract()});

                        log_debug << "Message send to DQL, id: " << result.oid << " queueArn: " << dlQueueArn;

                    } else {

                        result.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
                        messageList.push_back(result);

                        document filterQuery;
                        filterQuery.append(kvp("_id", message["_id"].get_oid()));

                        document setQuery;
                        setQuery.append(kvp("reset", bsoncxx::types::b_date(reset)));
                        setQuery.append(kvp("retries", static_cast<bsoncxx::types::b_int64>(result.retries)));
                        setQuery.append(kvp("receiptHandle", result.receiptHandle));
                        setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)));

                        document updateQuery;
                        updateQuery.append(kvp("$set", setQuery));

                        queueBulk.append(mongocxx::model::update_one{filterQuery.extract(), updateQuery.extract()});
                        log_debug << "Message updated, id: " << result.oid << " queueArn: " << queueArn;
                    }
                }

                // Update dql
                if (!dqlBulk.empty()) {
                    session.start_transaction();
                    auto result = dqlBulk.execute();
                    log_debug << "Dl queue bulk update, count: " << result->modified_count();
                    session.commit_transaction();
                }

                // Queue bulk
                if (!queueBulk.empty()) {
                    session.start_transaction();
                    auto result = queueBulk.execute();
                    log_debug << "Queue bulk update, count: " << result->modified_count();
                    session.commit_transaction();
                }

                log_trace << "Messages received, queueArn: " << queueArn + " count: " << messageList.size();

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
            }

        } else {

            _memoryDb.ReceiveMessages(queueArn, visibility, maxResult, dlQueueArn, maxRetries, messageList);
        }
    }

    long SQSDatabase::ResetMessages(const std::string &queueArn, const long visibility) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "reset_messages");

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            const auto newReset = system_clock::now() + std::chrono::seconds{visibility};

            try {

                document filterQuery;
                filterQuery.append(kvp("queueArn", queueArn));
                filterQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)));
                filterQuery.append(kvp("reset", make_document(kvp("$lt", bsoncxx::types::b_date(system_clock::now())))));

                document setQuery;
                setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));
                setQuery.append(kvp("receiptHandle", ""));
                setQuery.append(kvp("reset", bsoncxx::types::b_date(newReset)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                session.start_transaction();
                const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());

                session.commit_transaction();
                log_debug << "Message reset, updated: " << result->modified_count() << " queueArn: " << queueArn;
                updated = result->modified_count();

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
                throw Core::DatabaseException(e.what());
            }
        } else {
            updated = _memoryDb.ResetMessages(queueArn, visibility);
        }
        return updated;
    }

    long SQSDatabase::RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "relocate_messages");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {
                std::string dlqQueueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(redrivePolicy.deadLetterTargetArn);
                std::string dlqQueueName = Core::AwsUtils::ConvertSQSQueueArnToName(redrivePolicy.deadLetterTargetArn);

                document filterQuery;
                filterQuery.append(kvp("queueArn", queueArn));
                filterQuery.append(kvp("retries", make_document(kvp("$gt", static_cast<bsoncxx::types::b_int64>(redrivePolicy.maxReceiveCount)))));

                document setQuery;
                setQuery.append(kvp("retries", 0));
                setQuery.append(kvp("queueArn", redrivePolicy.deadLetterTargetArn));
                setQuery.append(kvp("queueUrl", dlqQueueUrl));
                setQuery.append(kvp("queueName", dlqQueueName));
                setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                session.start_transaction();
                const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());
                session.commit_transaction();
                log_trace << "Message re-drive, arn: " << redrivePolicy.deadLetterTargetArn << " updated: " << result->modified_count() << " queueArn: " << queueArn;

                return result->modified_count();

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
                throw Core::DatabaseException(e.what());
            }
        }
        return _memoryDb.RelocateToDlqMessages(queueArn, redrivePolicy);
    }

    long SQSDatabase::ResetDelayedMessages(const std::string &queueArn, const long delay) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "reset_delayed_messages");

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {
                const auto now = system_clock::now();

                document filterQuery;
                filterQuery.append(kvp("queueArn", queueArn));
                filterQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::DELAYED)));
                filterQuery.append(kvp("reset", make_document(kvp("$lt", bsoncxx::types::b_date(now)))));

                document setQuery;
                setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                session.start_transaction();
                const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());
                session.commit_transaction();
                log_trace << "Delayed message reset, updated: " << result->upserted_count() << " queueArn: " << queueArn;
                updated = result->upserted_count();

            } catch (mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Collection transaction exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            updated = _memoryDb.ResetDelayedMessages(queueArn);
        }
        return updated;
    }

    long SQSDatabase::RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "redrive_messages");

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            const auto newReset = system_clock::now() + std::chrono::seconds{originalQueue.attributes.visibilityTimeout};
            try {

                document filterQuery;
                filterQuery.append(kvp("queueArn", dlqQueue.queueArn));

                document setQuery;
                setQuery.append(kvp("queueArn", originalQueue.queueArn));
                setQuery.append(kvp("queueUrl", originalQueue.queueUrl));
                setQuery.append(kvp("queueName", originalQueue.name));
                setQuery.append(kvp("retries", 0));
                setQuery.append(kvp("reset", bsoncxx::types::b_date(newReset)));
                setQuery.append(kvp("receiptHandle", ""));
                setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                session.start_transaction();
                const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());
                session.commit_transaction();
                log_trace << "Message re-drive, arn: " << dlqQueue.queueArn << " updated: " << result->modified_count();

                updated = result->modified_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            updated = _memoryDb.RedriveMessages(originalQueue, dlqQueue);
        }
        return updated;
    }

    long SQSDatabase::MessageRetention(const std::string &queueArn, const long retentionPeriod) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "retention_messages");

        const auto reset = system_clock::now() - std::chrono::seconds{retentionPeriod};

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                document filterQuery;
                filterQuery.append(kvp("queueArn", queueArn));
                filterQuery.append(kvp("created", make_document(kvp("$lt", bsoncxx::types::b_date(reset)))));

                session.start_transaction();
                const auto result = messageCollection.delete_many(filterQuery.extract());
                session.commit_transaction();

                log_trace << "Message retention reset, deleted: " << result->deleted_count() << " queue: " << queueArn;
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.MessageRetention(queueArn, retentionPeriod);
        }
        return deleted;
    }

    long SQSDatabase::CountMessages(const std::string &queueArn, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_messages");

        if (HasDatabase()) {

            long count = 0;
            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            document query = {};
            if (!queueArn.empty()) {
                query.append(kvp("queueArn", queueArn));
            }
            if (!prefix.empty()) {
                query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
            }

            count = messageCollection.count_documents(query.extract());
            log_trace << "Count messages, queueArn: " << queueArn << " result: " << count;

            return count;
        }
        return _memoryDb.CountMessages(queueArn);
    }

    long SQSDatabase::CountMessageSize(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_messages");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            mongocxx::pipeline p{};
            if (!queueArn.empty()) {
                p.match(make_document(kvp("queueArn", queueArn)));
            }
            p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
            p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
            auto totalSizeCursor = messageCollection.aggregate(p);
            if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                log_info << "Total size: " << t["totalSize"].get_int64().value;
                return t["totalSize"].get_int64().value;
            }

            return 0;
        }
        return _memoryDb.CountMessages(queueArn);
    }

    long SQSDatabase::CountMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_messages");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                auto messageCollection = client->database(_databaseName)[_messageCollectionName];

                const long count = messageCollection.count_documents(make_document(kvp("queueArn", queueArn), kvp("status", MessageStatusToString(status))));
                log_trace << "Count resources by status, status: " << MessageStatusToString(status) << " result: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountMessagesByStatus(queueArn, status);
    }

    Entity::SQS::MessageWaitTime SQSDatabase::GetAverageMessageWaitingTime() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "waiting_time_messages");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            try {
                Entity::SQS::MessageWaitTime waitTime;
                mongocxx::options::find opts;
                opts.limit(1);

                for (const Entity::SQS::QueueList queueList = ListQueues(); const auto &queue: queueList) {

                    opts.sort(make_document(kvp("created", -1)));

                    system_clock::time_point firstTimestamp = system_clock::now();
                    system_clock::time_point lastTimestamp = system_clock::now();

                    if (const auto first = messageCollection.find_one(make_document(kvp("queueArn", queue.queueArn)), opts)) {
                        Entity::SQS::Message firstMessage;
                        firstMessage.FromDocument(first->view());
                        firstTimestamp = firstMessage.created;
                    }

                    opts.sort(make_document(kvp("created", 1)));

                    if (auto last = messageCollection.find_one(make_document(kvp("queueArn", queue.queueArn)), opts); last.has_value()) {
                        Entity::SQS::Message lastMessage;
                        lastMessage.FromDocument(last->view());
                        lastTimestamp = lastMessage.created;
                    }

                    const double max = std::chrono::duration<double, std::milli>(system_clock::now() - firstTimestamp).count();
                    if (const double min = std::chrono::duration<double, std::milli>(system_clock::now() - lastTimestamp).count(); max + min > 5) {
                        waitTime.waitTime[queue.name] = (max + min) / 2.0;
                    } else {
                        waitTime.waitTime[queue.name] = 0.0;
                    }
                }
                return waitTime;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetAverageMessageWaitingTime();
    }

    long SQSDatabase::DeleteMessages(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = messageCollection.delete_many(make_document(kvp("queueArn", queueArn)));
                session.commit_transaction();
                log_debug << "Messages deleted, queueArn: " << queueArn << " count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteMessages(queueArn);
        }
        return deleted;
    }

    long SQSDatabase::DeleteMessage(const Entity::SQS::Message &message) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_message");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = messageCollection.delete_one(make_document(kvp("receiptHandle", message.receiptHandle)));
                session.commit_transaction();

                log_debug << "Messages deleted, receiptHandle: " << message.receiptHandle << ", count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteMessage(message);
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return deleted;
    }

    long SQSDatabase::DeleteMessage(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];

            try {

                auto result = messageCollection.delete_one(make_document(kvp("receiptHandle", receiptHandle)));
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteMessage(receiptHandle);
    }

    long SQSDatabase::DeleteAllMessages() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = client->database(_databaseName)[_messageCollectionName];
            auto queueCollection = client->database(_databaseName)[_queueCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = messageCollection.delete_many({});
                deleted = result->deleted_count();
                session.commit_transaction();
                log_debug << "All messages deleted, count: " << deleted;


            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteAllMessages();
        }

        // Update monitoring counters
        AdjustMessageCounters();

        return deleted;
    }

    void SQSDatabase::AdjustMessageCounters() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "adjust_message_counter");

        using bsoncxx::builder::basic::make_array;

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto queueCollection = (*client)[_databaseName][_queueCollectionName];
            auto session = client->start_session();

            try {
                mongocxx::pipeline p{};
                p.group(make_document(
                        kvp("_id", "$queueArn"),
                        kvp("size", make_document(kvp("$sum", "$size"))),
                        kvp("total", make_document(kvp("$sum", 1))),
                        kvp("initial", make_document(kvp("$sum",
                                                         make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)))), 1, 0)))))),
                        kvp("invisible", make_document(kvp("$sum",
                                                           make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$status", MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)))), 1, 0)))))),
                        kvp("delayed", make_document(kvp("$sum",
                                                         make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$status", MessageStatusToString(Entity::SQS::MessageStatus::DELAYED)))), 1, 0))))))));

                document projectDocument;
                projectDocument.append(kvp("_id", 0),
                                       kvp("queueArn", "$_id"),
                                       kvp("total", 1),
                                       kvp("initial", 1),
                                       kvp("invisible", 1),
                                       kvp("delayed", 1),
                                       kvp("size", 1));
                p.project(projectDocument.extract());

                session.start_transaction();

                // Initialize all queues with zero message counts
                queueCollection.update_many({}, make_document(kvp("$set", make_document(
                                                                                  kvp("size", bsoncxx::types::b_int64()),
                                                                                  kvp("attributes.approximateNumberOfMessages", bsoncxx::types::b_int64()),
                                                                                  kvp("attributes.approximateNumberOfMessagesDelayed", bsoncxx::types::b_int64()),
                                                                                  kvp("attributes.approximateNumberOfMessagesNotVisible", bsoncxx::types::b_int64())))));

                // Create a bulk write operation for queues with messages
                auto bulk = queueCollection.create_bulk_write();
                for (auto cursor = messageCollection.aggregate(p); const auto t: cursor) {
                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("queueArn", Core::Bson::BsonUtils::GetStringValue(t, "queueArn"))),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                              kvp("attributes.approximateNumberOfMessages", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "initial"))),
                                                              kvp("attributes.approximateNumberOfMessagesDelayed", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "delayed"))),
                                                              kvp("attributes.approximateNumberOfMessagesNotVisible", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "invisible"))))))));

                    log_debug << "Queue: " << Core::Bson::BsonUtils::GetStringValue(t, "queueArn")
                              << ", size: " << Core::Bson::BsonUtils::GetLongValue(t, "size")
                              << ", visible: " << Core::Bson::BsonUtils::GetLongValue(t, "initial")
                              << ", invisible: " << Core::Bson::BsonUtils::GetLongValue(t, "invisible")
                              << ", delayed: " << Core::Bson::BsonUtils::GetLongValue(t, "delayed");
                }

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
