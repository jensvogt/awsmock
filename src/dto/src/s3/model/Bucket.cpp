//
// Created by vogje01 on 12/18/23.
//

#include <awsmock/dto/s3/model/Bucket.h>

namespace AwsMock::Dto::S3 {

    void Bucket::FromDocument(const view_or_value<view, value> &jsonObject) {

        region = Core::Bson::BsonUtils::GetStringValue(jsonObject, "region");
        bucketName = Core::Bson::BsonUtils::GetStringValue(jsonObject, "bucketName");
        owner = Core::Bson::BsonUtils::GetStringValue(jsonObject, "owner");
        size = Core::Bson::BsonUtils::GetLongValue(jsonObject, "size");
        keys = Core::Bson::BsonUtils::GetLongValue(jsonObject, "keys");
        versionStatus = Core::Bson::BsonUtils::GetStringValue(jsonObject, "versionStatus");

        // SQS queues
        if (jsonObject.view().find("queueConfigurations") != jsonObject.view().end()) {
            for (const bsoncxx::array::view jsonQueueConfigurationArray = jsonObject.view()["queueConfigurations"].get_array().value; const auto &configuration: jsonQueueConfigurationArray) {
                QueueConfiguration queueConfiguration;
                queueConfiguration.FromDocument(configuration.get_document().value);
            }
        }

        // SNS topics
        if (jsonObject.view().find("topicConfigurations") != jsonObject.view().end()) {
            for (const bsoncxx::array::view jsonQueueConfigurationArray = jsonObject.view()["topicConfigurations"].get_array().value; const auto &configuration: jsonQueueConfigurationArray) {
                TopicConfiguration topicConfiguration;
                topicConfiguration.FromDocument(configuration.get_document().value);
            }
        }

        // Lambdas
        if (jsonObject.view().find("lambdaConfigurations") != jsonObject.view().end()) {
            for (const bsoncxx::array::view jsonQueueConfigurationArray = jsonObject.view()["lambdaConfigurations"].get_array().value; const auto &configuration: jsonQueueConfigurationArray) {
                LambdaConfiguration lambdaConfiguration;
                lambdaConfiguration.FromDocument(configuration.get_document().value);
            }
        }
    }

    // TODO: Remove
    /*    std::string Bucket::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }*/

    view_or_value<view, value> Bucket::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "name", bucketName);
            Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
            Core::Bson::BsonUtils::SetLongValue(document, "size", size);
            Core::Bson::BsonUtils::SetLongValue(document, "keys", keys);
            Core::Bson::BsonUtils::SetStringValue(document, "versionStatus", versionStatus);
            Core::Bson::BsonUtils::SetDateValue(document, "created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);

            // SQS queues
            if (!queueConfigurations.empty()) {
                array jsonArray;
                for (const auto &queueNotification: queueConfigurations) {
                    jsonArray.append(queueNotification.ToDocument());
                }
                document.append(kvp("queueConfigurations", jsonArray));
            }

            // SNS topics
            if (!topicConfigurations.empty()) {
                array jsonArray;
                for (const auto &topicNotification: topicConfigurations) {
                    jsonArray.append(topicNotification.ToDocument());
                }
                document.append(kvp("topicConfigurations", jsonArray));
            }

            // SNS topics
            if (!lambdaConfigurations.empty()) {
                array jsonArray;
                for (const auto &lambdaNotification: lambdaConfigurations) {
                    jsonArray.append(lambdaNotification.ToDocument());
                }
                document.append(kvp("topicConfigurations", jsonArray));
            }
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }
}// namespace AwsMock::Dto::S3
