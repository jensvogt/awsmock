//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/core/exception/DatabaseException.h"


#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    const std::map<std::string, IndexDefinition> DatabaseBase::indexDefinitions = {
            // SQS messages
            {"sqs_message_idx1", {"sqs_message", {{"queueUrl", 1}, {"status", 1}, {"reset", 1}}}},
            {"sqs_message_idx2", {"sqs_message", {{"queueUrl", 1}, {"status", 1}, {"retries", 1}}}},
            {"sqs_message_idx3", {"sqs_message", {{"receiptHandle", 1}}}},
            {"sqs_message_idx4", {"sqs_message", {{"queueArn", 1}, {"status", 1}}}},
            {"sqs_message_idx5", {"sqs_message", {{"queueArn", 1}, {"created", 1}}}},
            // SQS queues
            {"sqs_queue_idx1", {"sqs_queue", {{"region", 1}, {"name", 1}}}},
            {"sqs_queue_idx2", {"sqs_queue", {{"region", 1}, {"queueUrl", 1}}}},
            // SNS messages
            {"sns_message_idx1", {"sns_message", {{"region", 1}, {"topicArn", 1}}}},
            // SNS topics
            {"sns_topic_idx1", {"sns_topic", {{"region", 1}, {"topicName", 1}}}},
            // S3 buckets
            {"s3_bucket_idx1", {"s3_bucket", {{"region", 1}, {"name", 1}}}},
            // S3 objects
            {"s3_object_idx1", {"s3_object", {{"region", 1}, {"bucket", 1}, {"key", 1}}}},
            {"s3_object_idx2", {"s3_object", {{"internalName", 1}}}},
            // Modules
            {"module_idx1", {"module", {{"name", 1}, {"state", 1}}}},
            // KMS
            {"kms_idx1", {"kms", {{"region", 1}, {"keyId", 1}}}},
            // Applications
            {"application_idx1", {"apps_application", {{"region", 1}, {"name", 1}}}},
            {"application_idx2", {"apps_application", {{"region", 1}, {"name", 1}, {"status", 1}}}},
            // Monitoring
            {"monitoring_idx1", {"monitoring", {{"name", 1}, {"created", 1}}}},
            {"monitoring_idx2", {"monitoring", {{"name", 1}, {"labelName", 1}, {"labelValue", 1}, {"created", 1}}}},
            {"monitoring_idx3", {"monitoring", {{"name", 1}, {"labelName", 1}, {"labelValue", 1}}}},
    };

    DatabaseBase::DatabaseBase() : _useDatabase(false) {
        _useDatabase = Core::Configuration::instance().GetValue<bool>("awsmock.mongodb.active");
        _name = Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
    }

    mongocxx::database DatabaseBase::GetConnection() const {
        const std::optional _client = _pool->acquire();
        if (!_client) {
            log_error << "Database connection not available";
            throw Core::DatabaseException("Database connection not available");
        }
        return (*_client)[_name];
    }

    bool DatabaseBase::HasDatabase() {
        return Core::Configuration::instance().GetValue<bool>("awsmock.mongodb.active");
    }

    std::string DatabaseBase::GetDatabaseName() const {
        return _name;
    }

    void DatabaseBase::CreateIndexes() const {
        if (_useDatabase) {
            const auto client = ConnectionPool::instance().GetConnection();
            const mongocxx::database database = (*client)[_name];

            log_info << "Start creating indexes";

            for (const auto &indexName: std::views::keys(indexDefinitions)) {
                CreateIndex(database, indexName);
            }
            log_info << "Finished creating indexes, count: " << indexDefinitions.size();
        }
    }

    void DatabaseBase::CreateIndex(const mongocxx::database &database, const std::string &indexName) {

        log_trace << "Start creating index, name: " << indexName;
        auto [collectionName, indexColumns] = indexDefinitions.at(indexName);

        document queryDoc;
        for (const auto &[columns, direction]: indexColumns) {
            queryDoc.append(kvp(columns, direction));
        }
        document nameDoc;
        nameDoc.append(kvp("name", indexName));

        database[collectionName].create_index(queryDoc.extract(), nameDoc.extract());
        log_trace << "Database index created, name: " << indexName;
    }

}// namespace AwsMock::Database
