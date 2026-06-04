//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/sqs/SQSMongoRepository.h>

namespace Awsmock::Database {

    bool SQSMongoRepository::queueExists(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        mongocxx::options::count options;
        options.limit(1);

        return queueCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;
    }

    bool SQSMongoRepository::queueUrlExists(const std::string &region, const std::string &queueUrl) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        mongocxx::options::count options;
        options.limit(1);

        document query;
        if (!region.empty()) {
            query.append(kvp("region", region));
        }
        if (!queueUrl.empty()) {
            query.append(kvp("url", queueUrl));
        }

        return queueCollection.count_documents(query.view(), options) > 0;
    }

    bool SQSMongoRepository::queueArnExists(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "queue_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        mongocxx::options::count options;
        options.limit(1);

        return queueCollection.count_documents(make_document(kvp("arn", queueArn)), options) > 0;
    }

    Entity::SQS::Queue SQSMongoRepository::createQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "create_exists");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto queueCollection = (*client)[_databaseName][_queueCollectionName];
            const auto result = queueCollection.insert_one(queue.ToDocument());
            log_trace << "Queue created, oid: " << result->inserted_id().get_oid().value.to_string();

            queue.oid = result->inserted_id().get_oid().value.to_string();
            return queue;

        } catch (const std::exception &e) {
            throw Core::DatabaseException("Insert queue failed, region: " + queue.region + " queueUrl: " + queue.url + " message: " + e.what());
        }
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueById(const bsoncxx::oid &oid) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        const auto mResult = queueCollection.find_one(make_document(kvp("_id", oid)));
        if (mResult->empty()) {
            log_error << "Queue not found, oid: " << oid.to_string();
            throw Core::DatabaseException("Queue not found, oid: " + oid.to_string());
        }

        Entity::SQS::Queue result;
        result.FromDocument(mResult->view());
        return result;
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueById(const std::string &oid) const {
        return getQueueById(bsoncxx::oid(oid));
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueByUrl(const std::string &region, const std::string &queueUrl) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        document query;
        if (!region.empty()) {
            query.append(kvp("region", region));
        }
        if (!queueUrl.empty()) {
            query.append(kvp("url", queueUrl));
        }
        if (const auto mResult = queueCollection.find_one(query.extract()); !mResult) {
            log_error << "Queue not found, region: " << region << " queueUrl: " << queueUrl;
            throw Core::DatabaseException("Queue not found, region: " + region + " queueUrl: " + queueUrl);
        } else {
            Entity::SQS::Queue result;
            return result.FromDocument(mResult->view());
        }
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueByArn(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        if (const auto mResult = queueCollection.find_one(make_document(kvp("arn", queueArn)))) {
            Entity::SQS::Queue queue;
            queue.FromDocument(mResult->view());
            return queue;
        }
        log_error << "Queue not found, queueArn: " << queueArn;
        throw Core::DatabaseException("Queue not found, queueArn: " + queueArn);
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueByDlq(const std::string &dlqQueueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        Entity::SQS::Queue result;
        const auto mResult = queueCollection.find_one(make_document(kvp("attributes.redrivePolicy.deadLetterTargetArn", dlqQueueArn)));

        if (!mResult) {
            log_error << "Queue not found, queueArn: " << dlqQueueArn;
            throw Core::DatabaseException("Queue not found, queueArn: " + dlqQueueArn);
        }
        return result.FromDocument(mResult->view());
    }

    std::vector<Entity::SQS::Queue> SQSMongoRepository::isDlq(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "is_dlq");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        document query = {};
        if (!queueArn.empty()) {
            query.append(kvp("attributes.redrivePolicy.deadLetterTargetArn", queueArn));
        }

        Entity::SQS::QueueList queueList;
        for (auto queueCursor = queueCollection.find(query.view()); auto queue: queueCursor) {
            Entity::SQS::Queue result;
            result.FromDocument(queue);
            queueList.push_back(result);
        }
        return queueList;
    }

    Entity::SQS::Queue SQSMongoRepository::getQueueByName(const std::string &region, const std::string &queueName) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        document query;
        if (!region.empty()) {
            query.append(kvp("region", region));
        }
        if (!queueName.empty()) {
            query.append(kvp("name", queueName));
        }

        if (const auto mResult = queueCollection.find_one(query.extract()); !mResult) {

            log_warning << "GetQueueByName failed, region: " << region << " name: " << queueName;
            throw Core::DatabaseException("Queue not found, region: " + region + " name: " + queueName);

        } else {
            Entity::SQS::Queue result;
            log_info << "GetQueueByName succeeded, region: " << region << " name: " << queueName;
            return result.FromDocument(mResult->view());
        }
    }

    Entity::SQS::QueueList SQSMongoRepository::listQueues(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_queues");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];

        document query = {};
        if (!prefix.empty()) {
            query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
        }
        if (!region.empty()) {
            query.append(kvp("region", region));
        }

        mongocxx::options::find opts;
        if (pageSize > 0) {
            opts.limit(pageSize);
        }
        if (pageIndex > 0) {
            opts.skip(pageIndex * pageSize);
        }

        if (!sortColumns.empty()) {
            document sort;
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        }

        Entity::SQS::QueueList queueList;
        for (auto queueCursor = queueCollection.find(query.view(), opts); auto queue: queueCursor) {
            Entity::SQS::Queue result;
            result.FromDocument(queue);
            queueList.push_back(result);
        }
        log_trace << "Got queue list, size: " << queueList.size();
        return queueList;
    }

    Entity::SQS::QueueList SQSMongoRepository::exportQueues(const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "export_queues");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];

        const document query = {};

        mongocxx::options::find opts;
        if (!sortColumns.empty()) {
            document sort;
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        } else {
            opts.sort(make_document(kvp("_id", 1)));
        }

        Entity::SQS::QueueList queueList;
        for (auto queueCursor = queueCollection.find(query.view(), opts); auto queue: queueCursor) {
            Entity::SQS::Queue result;
            result.FromDocument(queue);
            queueList.push_back(result);
        }
        log_trace << "Got queue list, size: " << queueList.size();
        return queueList;
    }

    void SQSMongoRepository::importQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "import_queues");

        queue.url = Core::CreateSQSQueueUrl(queue.name);
        queue.modified = system_clock::now();
        queue.attributes.approximateNumberOfMessages = 0;
        queue.attributes.approximateNumberOfMessagesDelayed = 0;
        queue.attributes.approximateNumberOfMessagesNotVisible = 0;
        queue = createOrUpdateQueue(queue);
        log_trace << "Queue imported, queueName: " << queue.name;
    }

    Entity::SQS::QueueList SQSMongoRepository::listQueues(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_queues");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];

        document query = {};
        if (!region.empty()) {
            query.append(kvp("region", region));
        }

        Entity::SQS::QueueList queueList;
        for (auto queueCursor = queueCollection.find(query.view()); auto queue: queueCursor) {
            Entity::SQS::Queue result;
            result.FromDocument(queue);
            queueList.push_back(result);
        }
        log_trace << "Got queue list, size: " << queueList.size();
        return queueList;
    }

    long SQSMongoRepository::purgeQueue(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "purge_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = messageCollection.delete_many(make_document(kvp("queueArn", queueArn)));

            log_debug << "Purged queue, count: " << result->deleted_count() << " queueArn: " << queueArn;
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SQS::Queue SQSMongoRepository::updateQueue(Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "update_queue");

        queue.modified = system_clock::now();

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto queueCollection = (*client)[_databaseName][_queueCollectionName];

            const auto mResult = queueCollection.find_one_and_update(make_document(kvp("arn", queue.arn)), queue.ToDocument(), opts);
            log_trace << "Queue updated: " << Core::Bson::BsonUtils::ToJsonString(queue.ToDocument());

            if (mResult) {
                queue.FromDocument(mResult->view());
                return queue;
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
        return {};
    }

    Entity::SQS::Queue SQSMongoRepository::createOrUpdateQueue(Entity::SQS::Queue &queue) const {

        if (queueArnExists(queue.arn)) {

            return updateQueue(queue);
        }
        return createQueue(queue);
    }

    long SQSMongoRepository::countQueues(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_queues");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];

        auto builder = document{};

        if (!prefix.empty()) {
            builder.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
        }

        if (!region.empty()) {
            builder.append(kvp("region", region));
        }

        return queueCollection.count_documents(builder.view());
    }

    long SQSMongoRepository::deleteQueue(const Entity::SQS::Queue &queue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_queue");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = queueCollection.delete_many(make_document(kvp("region", queue.region), kvp("url", queue.url)));
            messageCollection.delete_many(make_document(kvp("queueArn", queue.arn)));

            log_debug << "Queue deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::deleteAllQueues() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_all_queues");

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = queueCollection.delete_many({});
            messageCollection.delete_many({});

            log_debug << "All queues deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SQS::Message SQSMongoRepository::createMessage(Entity::SQS::Message &message) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "create_message");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            const auto result = messageCollection.insert_one(message.ToDocument());
            log_trace << "Message created, oid: " << result->inserted_id().get_oid().value.to_string();

            message.oid = result->inserted_id().get_oid().value.to_string();
            return message;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool SQSMongoRepository::messageExists(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "exists_message");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            mongocxx::options::count options;
            options.limit(1);

            document query;
            query.append(kvp("receiptHandle", receiptHandle));

            return messageCollection.count_documents(query.view(), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool SQSMongoRepository::messageExistsByMessageId(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "exists_message");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            mongocxx::options::count options;
            options.limit(1);

            return messageCollection.count_documents(make_document(kvp("messageId", messageId)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SQS::Message SQSMongoRepository::getMessageById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        const auto mResult = messageCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SQS::Message result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::SQS::Message SQSMongoRepository::getMessageByReceiptHandle(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        if (const auto mResult = messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle)))) {
            Entity::SQS::Message result;
            result.FromDocument(mResult->view());
            return result;
        }
        return {};
    }

    Entity::SQS::Message SQSMongoRepository::getMessageByMessageId(const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "get_message");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        if (const auto mResult = messageCollection.find_one(make_document(kvp("messageId", messageId)))) {
            Entity::SQS::Message result;
            result.FromDocument(mResult->view());
            return result;
        }
        return {};
    }

    Entity::SQS::Message SQSMongoRepository::getMessageById(const std::string &oid) const {
        return getMessageById(bsoncxx::oid(oid));
    }

    Entity::SQS::Message SQSMongoRepository::updateMessage(Entity::SQS::Message &message) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "update_message");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            message.modified = system_clock::now();
            if (const auto mResult = messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts)) {
                message.FromDocument(mResult->view());
                log_trace << "Message updated: " << Core::Bson::BsonUtils::ToJsonString(message.ToDocument());
                return message;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SQS::Message SQSMongoRepository::createOrUpdateMessage(Entity::SQS::Message &message) const {

        if (messageExists(message.receiptHandle)) {

            return updateMessage(message);
        }
        return createMessage(message);
    }

    Entity::SQS::MessageList SQSMongoRepository::listMessages(const std::string &region) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        document query;
        if (!region.empty()) {
            query.append(kvp("region", region));
        }

        Entity::SQS::MessageList messageList;
        for (auto messageCursor = messageCollection.find(query.view()); auto &message: messageCursor) {
            Entity::SQS::Message result;
            result.FromDocument(message);
            messageList.push_back(result);
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    Entity::SQS::MessageList SQSMongoRepository::listMessages(const std::string &queueArn, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "list_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        mongocxx::options::find opts;
        if (pageSize > 0) {
            opts.limit(pageSize);
            if (pageIndex > 0) {
                opts.skip(pageSize * pageIndex);
            }
        }

        if (!sortColumns.empty()) {
            document sort;
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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

        Entity::SQS::MessageList messageList;
        for (auto messageCursor = messageCollection.find(query.extract(), opts); auto message: messageCursor) {
            Entity::SQS::Message result;
            result.FromDocument(message);
            messageList.push_back(result);
        }
        return messageList;
    }

    void SQSMongoRepository::importMessages(const std::string &queueArn, const value &messageArray) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "import_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            auto bulk = messageCollection.create_bulk_write();

            for (const auto &message: messageArray) {
                const mongocxx::model::insert_one insert_op{message.get_document().view()};
                bulk.append(insert_op);
            }
            const auto result = bulk.execute();
            log_info << "Imported messages: " << result->inserted_count();

        } catch (mongocxx::exception &e) {
            log_error << "queueCollection exception: " << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    void SQSMongoRepository::receiveMessages(const std::string &queueArn, const long visibility, const long maxResult, const std::string &dlQueueArn, const long maxRetries, Entity::SQS::MessageList &messageList) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "receive_messages");

        const auto reset = system_clock::now() + std::chrono::seconds(visibility);
        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            while (maxResult <= 0 || static_cast<long>(messageList.size()) < maxResult) {

                // Atomically claim one INITIAL message: increment retries and mark INVISIBLE
                const auto mResult = messageCollection.find_one_and_update(
                        make_document(kvp("queueArn", queueArn), kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))),
                        make_document(
                                kvp("$set", make_document(
                                                    kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
                                                    kvp("reset", bsoncxx::types::b_date(reset)),
                                                    kvp("receiptHandle", Core::AwsUtils::CreateSqsReceiptHandler()))),
                                kvp("$inc", make_document(kvp("retries", 1)))),
                        opts);

                if (!mResult) {
                    break;
                }

                Entity::SQS::Message result;
                result.FromDocument(mResult->view());

                if (!dlQueueArn.empty() && maxRetries > 0 && result.retries >= maxRetries) {
                    messageCollection.update_one(
                            make_document(kvp("_id", bsoncxx::oid{result.oid})),
                            make_document(kvp("$set", make_document(
                                                              kvp("queueArn", dlQueueArn),
                                                              kvp("receiptHandle", ""),
                                                              kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))))));
                    log_debug << "Message send to DLQ, id: " << result.oid << " queueArn: " << dlQueueArn;
                } else {
                    messageList.push_back(result);
                    log_debug << "Message received, id: " << result.oid << " queueArn: " << queueArn;
                }
            }

            log_trace << "Messages received, queueArn: " << queueArn << " count: " << messageList.size();

        } catch (mongocxx::exception &e) {
            log_error << "queueCollection exception: " << e.what();
        }
    }

    long SQSMongoRepository::resetMessages(const std::string &queueArn, const long visibility) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "reset_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            const auto filter = make_document(
                    kvp("queueArn", queueArn),
                    kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
                    kvp("reset", make_document(kvp("$lt", bsoncxx::types::b_date(system_clock::now())))));

            const auto update = make_document(
                    kvp("$set", make_document(
                                        kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)),
                                        kvp("receiptHandle", ""),
                                        kvp("reset", bsoncxx::types::b_date(system_clock::now() + std::chrono::seconds{visibility})))));

            const auto result = messageCollection.update_many(filter.view(), update.view());

            log_debug << "Message reset, updated: " << result->modified_count() << " queueArn: " << queueArn;
            return result->modified_count();

        } catch (mongocxx::exception &e) {
            log_error << "queueCollection exception: " << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    long SQSMongoRepository::resetDelayedMessages(const std::string &queueArn, const long delay) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "reset_delayed_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {
            const auto filter = make_document(
                    kvp("arn", queueArn),
                    kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::DELAYED)),
                    kvp("reset", make_document(kvp("$lt", bsoncxx::types::b_date(system_clock::now())))));

            const auto update = make_document(
                    kvp("$set", make_document(
                                        kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)))));

            const auto result = messageCollection.update_many(filter.view(), update.view());

            log_trace << "Delayed message reset, updated: " << result->upserted_count() << " queueArn: " << queueArn;
            return result->upserted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "queueCollection exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::redriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "redrive_message");

        const auto newReset = system_clock::now() + std::chrono::seconds{originalQueue.attributes.visibilityTimeout};
        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            document filterQuery;
            filterQuery.append(kvp("queueArn", dlqQueue.arn));
            filterQuery.append(kvp("messageId", messageId));

            document setQuery;
            setQuery.append(kvp("queueArn", originalQueue.arn));
            setQuery.append(kvp("queueUrl", originalQueue.url));
            setQuery.append(kvp("name", originalQueue.name));
            setQuery.append(kvp("retries", 0));
            setQuery.append(kvp("reset", bsoncxx::types::b_date(newReset)));
            setQuery.append(kvp("receiptHandle", ""));
            setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

            document updateQuery;
            updateQuery.append(kvp("$set", setQuery));

            const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());

            log_trace << "Message redrive, arn: " << dlqQueue.arn << ", messageId: " << messageId << ", updated: " << result->modified_count();
            return result->modified_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::redriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "redrive_messages");

        const auto newReset = system_clock::now() + std::chrono::seconds{originalQueue.attributes.visibilityTimeout};
        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            document filterQuery;
            filterQuery.append(kvp("queueArn", dlqQueue.arn));

            document setQuery;
            setQuery.append(kvp("queueArn", originalQueue.arn));
            setQuery.append(kvp("queueUrl", originalQueue.url));
            setQuery.append(kvp("name", originalQueue.name));
            setQuery.append(kvp("retries", 0));
            setQuery.append(kvp("reset", bsoncxx::types::b_date(newReset)));
            setQuery.append(kvp("receiptHandle", ""));
            setQuery.append(kvp("status", MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)));

            document updateQuery;
            updateQuery.append(kvp("$set", setQuery));

            const auto result = messageCollection.update_many(filterQuery.extract(), updateQuery.extract());

            log_trace << "Message re-drive, arn: " << dlqQueue.arn << " updated: " << result->modified_count();
            return result->modified_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::messageRetention(const std::string &queueArn, const long retentionPeriod) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "retention_messages");

        const auto reset = system_clock::now() - std::chrono::seconds{retentionPeriod};
        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            document filterQuery;
            filterQuery.append(kvp("queueArn", queueArn));
            filterQuery.append(kvp("created", make_document(kvp("$lt", bsoncxx::types::b_date(reset)))));

            const auto result = messageCollection.delete_many(filterQuery.extract());

            log_trace << "Message retention reset, deleted: " << result->deleted_count() << " queue: " << queueArn;
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::countMessages(const std::string &queueArn, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "count_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        document query = {};
        if (!queueArn.empty()) {
            query.append(kvp("queueArn", queueArn));
        }
        if (!prefix.empty()) {
            query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
        }
        return messageCollection.count_documents(query.view());
    }

    Entity::SQS::MessageWaitTime SQSMongoRepository::getAverageMessageWaitingTime() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "waiting_time_messages");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            Entity::SQS::MessageWaitTime waitTime;
            const auto now = system_clock::now();

            mongocxx::pipeline p{};
            p.group(make_document(
                    kvp("_id", "$queueArn"),
                    kvp("minCreated", make_document(kvp("$min", "$created"))),
                    kvp("maxCreated", make_document(kvp("$max", "$created")))));
            p.lookup(make_document(
                    kvp("from", _queueCollectionName),
                    kvp("localField", "_id"),
                    kvp("foreignField", "arn"),
                    kvp("as", "queue")));
            p.unwind(std::string("$queue"));
            p.project(make_document(
                    kvp("_id", 0),
                    kvp("queueName", "$queue.name"),
                    kvp("minCreated", 1),
                    kvp("maxCreated", 1)));

            for (auto cursor = messageCollection.aggregate(p); const auto &t: cursor) {
                const auto queueName = Core::Bson::BsonUtils::GetStringValue(t, "queueName");
                const auto newestCreated = system_clock::time_point(std::chrono::milliseconds(t["maxCreated"].get_date().value));
                const auto oldestCreated = system_clock::time_point(std::chrono::milliseconds(t["minCreated"].get_date().value));

                const double waitForNewest = std::chrono::duration<double, std::milli>(now - newestCreated).count();
                const double waitForOldest = std::chrono::duration<double, std::milli>(now - oldestCreated).count();

                waitTime.waitTime[queueName] = (waitForNewest + waitForOldest > 5.0) ? (waitForNewest + waitForOldest) / 2.0 : 0.0;
            }
            return waitTime;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::deleteMessages(const std::string &queueArn) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = messageCollection.delete_many(make_document(kvp("queueArn", queueArn)));

            log_debug << "Messages deleted, queueArn: " << queueArn << " count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::deleteMessage(const Entity::SQS::Message &message) const {
        return deleteMessage(message.receiptHandle);
    }

    long SQSMongoRepository::deleteMessage(const std::string &receiptHandle) const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto messageCollection = (*client)[_databaseName][_messageCollectionName];

            auto result = messageCollection.delete_one(make_document(kvp("receiptHandle", receiptHandle)));
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SQSMongoRepository::deleteAllMessages() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "delete_messages");

        const auto client = ConnectionPool::instance().GetConnection();
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

        try {

            const auto result = messageCollection.delete_many({});

            const long deleted = result->deleted_count();
            log_debug << "All messages deleted, count: " << deleted;
            return deleted;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void SQSMongoRepository::adjustMessageCounters() const {
        Monitoring::MonitoringTimer measure(SQS_DATABASE_TIMER, SQS_DATABASE_COUNTER, "action", "adjust_message_counter");

        using bsoncxx::builder::basic::make_array;

        const auto client = ConnectionPool::instance().GetConnection();
        auto queueCollection = (*client)[_databaseName][_queueCollectionName];
        auto messageCollection = (*client)[_databaseName][_messageCollectionName];

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

            // Get all queue ARNs from the aggregation result
            std::set<std::string> queuesWithMessages;
            auto bulk = queueCollection.create_bulk_write();
            for (auto cursor = messageCollection.aggregate(p); const auto t: cursor) {
                const auto queueArn = Core::Bson::BsonUtils::GetStringValue(t, "queueArn");
                queuesWithMessages.insert(queueArn);
                bulk.append(mongocxx::model::update_one(
                        make_document(kvp("arn", queueArn)),
                        make_document(kvp("$set", make_document(
                                                          kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                          kvp("attributes.approximateNumberOfMessages", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "initial"))),
                                                          kvp("attributes.approximateNumberOfMessagesDelayed", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "delayed"))),
                                                          kvp("attributes.approximateNumberOfMessagesNotVisible", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "invisible"))))))));
            }

            // Zero out queues with no messages
            for (auto queueCursor = queueCollection.find({}); const auto &q: queueCursor) {
                if (const auto queueArn = Core::Bson::BsonUtils::GetStringValue(q, "arn"); !queuesWithMessages.contains(queueArn)) {
                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("arn", queueArn)),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64()),
                                                              kvp("attributes.approximateNumberOfMessages", bsoncxx::types::b_int64()),
                                                              kvp("attributes.approximateNumberOfMessagesDelayed", bsoncxx::types::b_int64()),
                                                              kvp("attributes.approximateNumberOfMessagesNotVisible", bsoncxx::types::b_int64()))))));
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
