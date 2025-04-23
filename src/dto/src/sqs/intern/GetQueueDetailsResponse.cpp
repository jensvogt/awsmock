//
// Created by vogje01 on 30/05/2023.
//

// AwsMock includes
#include <awsmock/dto/sqs/internal/GetQueueDetailsResponse.h>

namespace AwsMock::Dto::SQS {

    std::string GetQueueDetailsResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "queueName", queueName);
            Core::Bson::BsonUtils::SetStringValue(document, "queueUrl", queueUrl);
            Core::Bson::BsonUtils::SetStringValue(document, "queueArn", queueArn);
            Core::Bson::BsonUtils::SetLongValue(document, "messageCount", messageCount);
            Core::Bson::BsonUtils::SetLongValue(document, "retentionPeriod", retentionPeriod);
            Core::Bson::BsonUtils::SetLongValue(document, "size", size);
            Core::Bson::BsonUtils::SetLongValue(document, "delay", delay);
            Core::Bson::BsonUtils::SetLongValue(document, "maxMessageSize", maxMessageSize);
            Core::Bson::BsonUtils::SetLongValue(document, "visibilityTimeout", visibilityTimeout);
            Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
            Core::Bson::BsonUtils::SetLongValue(document, "available", available);
            Core::Bson::BsonUtils::SetLongValue(document, "delayed", delayed);
            Core::Bson::BsonUtils::SetStringValue(document, "dlqArn", dlqArn);
            Core::Bson::BsonUtils::SetIntValue(document, "dlqMaxReceive", dlqMaxReceive);
            Core::Bson::BsonUtils::SetDateValue(document, "created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetQueueDetailsResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetQueueDetailsResponse &r) {
        os << "GetQueueDetailsResponse=" + r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
