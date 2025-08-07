//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SQSDatabase.h>

namespace AwsMock::Database {

    SQSDatabase::SQSDatabase() : _databaseName(GetDatabaseName()), _queueCollectionName("sqs_queue"), _messageCollectionName("sqs_message"), _memoryDb(SQSMemoryDb::instance()) {

        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        _sqsCounterMap = _segment.find<SqsCounterMapType>(SQS_COUNTER_MAP_NAME).first;
        if (!_sqsCounterMap) {
            _sqsCounterMap = _segment.construct<SqsCounterMapType>(SQS_COUNTER_MAP_NAME)(std::less<std::string>(), _segment.get_segment_manager());
        }

        // Initialize the counters
        InitializeCounters();
    }

    void SQSDatabase::InitializeCounters() const {
        for (const auto &queue: ListQueues()) {
            QueueMonitoringCounter counter;
            counter.initial = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);
            counter.invisible = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INVISIBLE);
            counter.delayed = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::DELAYED);
            counter.messages = CountMessages(queue.queueArn);
            counter.size = GetQueueSize(queue.queueArn);
            _sqsCounterMap->insert_or_assign(queue.queueArn, counter);
        }
        log_debug << "SQS queues counters initialized: size: " << _sqsCounterMap->size();
    }

    bool SQSDatabase::QueueExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

            const int64_t count = _queueCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
            log_trace << "Queue exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.QueueExists(region, name);
    }

    bool SQSDatabase::QueueUrlExists(const std::string &region, const std::string &queueUrl) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!queueUrl.empty()) {
                query.append(kvp("queueUrl", queueUrl));
            }

            const int64_t count = _queueCollection.count_documents(query.extract());
            log_trace << "Queue exists: " << std::boolalpha << (count > 0);
            return count > 0;
        }
        return _memoryDb.QueueUrlExists(region, queueUrl);
    }

    bool SQSDatabase::QueueArnExists(const std::string &queueArn) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

            const int64_t count = _queueCollection.count_documents(make_document(kvp("queueArn", queueArn)));
            log_trace << "Queue exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.QueueArnExists(queueArn);
    }

    Entity::SQS::Queue SQSDatabase::CreateQueue(Entity::SQS::Queue &queue) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];
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

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        Entity::SQS::Queue queue;
        if (HasDatabase()) {
            Entity::SQS::Queue result;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

            const auto mResult = _queueCollection.find_one(make_document(kvp("queueArn", queueArn)));

            if (!mResult) {
                log_error << "Queue not found, queueArn: " << queueArn;
                throw Core::DatabaseException("Queue not found, queueArn: " + queueArn);
            }
            queue = result.FromDocument(mResult->view());
        } else {
            queue = _memoryDb.GetQueueByArn(queueArn);
        }

        // Update counters
        queue.size = (*_sqsCounterMap)[queueArn].size;
        queue.attributes.approximateNumberOfMessages = (*_sqsCounterMap)[queueArn].messages;
        queue.attributes.approximateNumberOfMessagesNotVisible = (*_sqsCounterMap)[queueArn].invisible;
        queue.attributes.approximateNumberOfMessagesDelayed = (*_sqsCounterMap)[queueArn].delayed;
        return queue;
    }

    Entity::SQS::Queue SQSDatabase::GetQueueByDlq(const std::string &dlqQueueArn) const {

        if (HasDatabase()) {
            Entity::SQS::Queue result;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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
                result.size = (*_sqsCounterMap)[result.queueArn].size;
                result.attributes.approximateNumberOfMessages = (*_sqsCounterMap)[result.queueArn].messages;
                result.attributes.approximateNumberOfMessagesNotVisible = (*_sqsCounterMap)[result.queueArn].invisible;
                result.attributes.approximateNumberOfMessagesDelayed = (*_sqsCounterMap)[result.queueArn].delayed;
                queueList.push_back(result);
            }
            log_trace << "Got queue list, size: " << queueList.size();
            return queueList;
        }
        return _memoryDb.ListQueues(region);
    }

    Entity::SQS::QueueList SQSDatabase::ExportQueues(const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            mongocxx::options::find opts;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        queue.queueUrl = Core::CreateSQSQueueUrl(queue.name);
        queue.modified = system_clock::now();
        queue.attributes.approximateNumberOfMessages = 0;
        queue.attributes.approximateNumberOfMessagesDelayed = 0;
        queue.attributes.approximateNumberOfMessagesNotVisible = 0;
        CreateOrUpdateQueue(queue);
        log_trace << "Queue imported, queueName: " << queue.name;
    }

    Entity::SQS::QueueList SQSDatabase::ListQueues(const std::string &region) const {

        if (HasDatabase()) {

            Entity::SQS::QueueList queueList;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        long purged = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (purged > 0) {
            (*_sqsCounterMap)[queueArn].size = 0;
            (*_sqsCounterMap)[queueArn].messages = 0;
            (*_sqsCounterMap)[queueArn].initial = 0;
            (*_sqsCounterMap)[queueArn].invisible = 0;
            (*_sqsCounterMap)[queueArn].delayed = 0;
        }
        return purged;
    }

    Entity::SQS::Queue SQSDatabase::UpdateQueue(Entity::SQS::Queue &queue) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];
            auto session = client->start_session();

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            try {

                session.start_transaction();
                queue.modified = system_clock::now();
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

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_queueCollectionName];
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

        long count;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_messageCollectionName];

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

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];
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

        // Update the counter-map
        _sqsCounterMap->erase(queue.queueArn);
        return deleted;
    }

    long SQSDatabase::DeleteAllQueues() const {
        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _queueCollection = (*client)[_databaseName][_queueCollectionName];
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

        // Clear the counter-map
        _sqsCounterMap->clear();

        return deleted;
    }

    Entity::SQS::Message SQSDatabase::CreateMessage(Entity::SQS::Message &message) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        (*_sqsCounterMap)[message.queueArn].messages++;
        (*_sqsCounterMap)[message.queueArn].initial++;
        (*_sqsCounterMap)[message.queueArn].size += message.size;

        return message;
    }

    bool SQSDatabase::MessageExists(const std::string &receiptHandle) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            try {

                document query;
                query.append(kvp("receiptHandle", receiptHandle));

                const auto result = messageCollection.find_one(query.extract());
                log_trace << "Message exists: " << std::boolalpha << result->empty();
                return result && !result->empty();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.MessageExists(receiptHandle);
    }

    bool SQSDatabase::MessageExistsByMessageId(const std::string &messageId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            try {

                const auto result = messageCollection.find_one(make_document(kvp("messageId", messageId)));
                log_trace << "Message exists: " << std::boolalpha << result.has_value();
                return result.has_value();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.MessageExistsByMessageId(messageId);
    }

    Entity::SQS::Message SQSDatabase::GetMessageById(bsoncxx::oid oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        const auto mResult = messageCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SQS::Message result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::SQS::Message SQSDatabase::GetMessageByReceiptHandle(const std::string &receiptHandle) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        if (HasDatabase()) {

            Entity::SQS::MessageList messageList;

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        Entity::SQS::MessageList messageList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        Entity::SQS::MessageList messageList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            const auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

                // Update the counter-map
                InitializeCounters();

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
                throw Core::DatabaseException(e.what());
            }
        }
    }

    void SQSDatabase::ReceiveMessages(const std::string &queueArn, const long visibility, const long maxResult, const std::string &dlQueueArn, const long maxRetries, Entity::SQS::MessageList &messageList) const {

        const auto reset = system_clock::now() + std::chrono::seconds(visibility);

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto session = client->start_session();

            try {

                mongocxx::options::find opts;
                if (maxResult > 0) {
                    opts.limit(maxResult);
                }

                session.start_transaction();

                // Get the cursor
                for (auto messageCursor = messageCollection.find(make_document(kvp("queueArn", queueArn), kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))), opts); auto message: messageCursor) {

                    Entity::SQS::Message result;
                    result.FromDocument(message);

                    // Check retries
                    result.retries++;
                    if (maxRetries > 0 && result.retries >= maxRetries && !dlQueueArn.empty()) {

                        std::string dlqQueueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(dlQueueArn);
                        std::string dlqQueueName = Core::AwsUtils::ConvertSQSQueueArnToName(dlQueueArn);

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

                        messageCollection.update_one(filterQuery.extract(), updateQuery.extract());
                        log_debug << "Message send to DQL, id: " << result.oid << " queueArn: " << dlQueueArn;
                        (*_sqsCounterMap)[queueArn].initial--;
                        (*_sqsCounterMap)[queueArn].messages--;
                        (*_sqsCounterMap)[queueArn].size -= result.size;
                        (*_sqsCounterMap)[dlQueueArn].initial++;
                        (*_sqsCounterMap)[dlQueueArn].messages++;
                        (*_sqsCounterMap)[dlQueueArn].size += result.size;

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

                        messageCollection.update_one(filterQuery.extract(), updateQuery.extract());
                        log_debug << "Message updated, id: " << result.oid << " queueArn: " << queueArn;
                        (*_sqsCounterMap)[queueArn].initial--;
                        (*_sqsCounterMap)[queueArn].invisible++;
                    }
                }

                // Commit
                session.commit_transaction();
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

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (updated > 0) {
            (*_sqsCounterMap)[queueArn].initial += updated;
            (*_sqsCounterMap)[queueArn].invisible -= updated;
        }

        return updated;
    }

    long SQSDatabase::RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (updated > 0) {
            (*_sqsCounterMap)[queueArn].delayed -= updated;
            (*_sqsCounterMap)[queueArn].initial += updated;
        }
        return updated;
    }

    long SQSDatabase::RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const {

        long updated = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (updated > 0) {
            const long totalOriginal = CountMessages(originalQueue.queueArn);
            const long totalDql = CountMessages(dlqQueue.queueArn);
            const long sizeOriginal = GetQueueSize(originalQueue.queueArn);
            const long sizeDql = GetQueueSize(dlqQueue.queueArn);
            (*_sqsCounterMap)[originalQueue.queueArn].messages += totalOriginal;
            (*_sqsCounterMap)[originalQueue.queueArn].initial += totalOriginal;
            (*_sqsCounterMap)[originalQueue.queueArn].size += sizeOriginal;
            (*_sqsCounterMap)[dlqQueue.queueArn].messages -= totalDql;
            (*_sqsCounterMap)[dlqQueue.queueArn].initial -= totalDql;
            (*_sqsCounterMap)[dlqQueue.queueArn].size -= sizeDql;
        }
        return updated;
    }

    long SQSDatabase::MessageRetention(const std::string &queueArn, const long retentionPeriod) const {

        const auto reset = system_clock::now() - std::chrono::seconds{retentionPeriod};

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (deleted > 0) {
            QueueMonitoringCounter counter;
            counter.initial = CountMessagesByStatus(queueArn, Entity::SQS::MessageStatus::INITIAL);
            counter.invisible = CountMessagesByStatus(queueArn, Entity::SQS::MessageStatus::INVISIBLE);
            counter.delayed = CountMessagesByStatus(queueArn, Entity::SQS::MessageStatus::DELAYED);
            counter.messages = CountMessages(queueArn);
            counter.size = GetQueueSize(queueArn);
            _sqsCounterMap->insert_or_assign(queueArn, counter);
        }
        return deleted;
    }

    long SQSDatabase::CountMessages(const std::string &queueArn, const std::string &prefix) const {

        if (HasDatabase()) {

            long count = 0;
            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (deleted > 0) {
            (*_sqsCounterMap)[queueArn].messages = 0;
            (*_sqsCounterMap)[queueArn].initial = 0;
            (*_sqsCounterMap)[queueArn].invisible = 0;
            (*_sqsCounterMap)[queueArn].delayed = 0;
            (*_sqsCounterMap)[queueArn].size = 0;
        }
        return deleted;
    }

    long SQSDatabase::DeleteMessage(const Entity::SQS::Message &message) const {

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
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

        // Update the counter-map
        if (deleted > 0) {
            (*_sqsCounterMap)[message.queueArn].size -= message.size;
            (*_sqsCounterMap)[message.queueArn].messages -= deleted;
            if (message.status == Entity::SQS::MessageStatus::INITIAL) {
                (*_sqsCounterMap)[message.queueArn].initial -= deleted;
            } else if (message.status == Entity::SQS::MessageStatus::DELAYED) {
                (*_sqsCounterMap)[message.queueArn].delayed -= deleted;
            } else if (message.status == Entity::SQS::MessageStatus::INVISIBLE) {
                (*_sqsCounterMap)[message.queueArn].invisible -= deleted;
            }
        }
        return deleted;
    }

    long SQSDatabase::DeleteMessage(const std::string &receiptHandle) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            try {

                if (const auto findResult = messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle))); !findResult->empty()) {
                    Entity::SQS::Message message;
                    message.FromDocument(findResult->view());
                    return DeleteMessage(message);
                }
                return 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteMessage(receiptHandle);
    }

    long SQSDatabase::DeleteAllMessages() const {

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];
            auto queueCollection = (*client)[_databaseName][_queueCollectionName];
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

        // Update the counter-map
        _sqsCounterMap->clear();
        return deleted;
    }
}// namespace AwsMock::Database
