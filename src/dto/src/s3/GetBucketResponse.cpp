//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/s3/GetBucketResponse.h>

namespace AwsMock::Dto::S3 {

    std::string GetBucketResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "id", id);
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "bucket", bucket);
            Core::Bson::BsonUtils::SetStringValue(document, "arn", arn);
            Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
            Core::Bson::BsonUtils::SetLongValue(document, "size", size);
            Core::Bson::BsonUtils::SetLongValue(document, "keys", keys);
            Core::Bson::BsonUtils::SetStringValue(document, "versionStatus", versionStatus);

            if (!lambdaConfigurations.empty()) {
                array jsonArray;
                for (const auto &lambdaNotification: lambdaConfigurations) {
                    jsonArray.append(lambdaNotification.ToDocument());
                }
                document.append(kvp("lambdaConfigurations", jsonArray));
            }

            if (!queueConfigurations.empty()) {
                array jsonArray;
                for (const auto &topicConfiguration: queueConfigurations) {
                    jsonArray.append(topicConfiguration.ToDocument());
                }
                document.append(kvp("queueConfigurations", jsonArray));
            }

            if (!topicConfigurations.empty()) {
                array jsonArray;
                for (const auto &topicConfiguration: topicConfigurations) {
                    jsonArray.append(topicConfiguration.ToDocument());
                }
                document.append(kvp("topicConfigurations", jsonArray));
            }
            Core::Bson::BsonUtils::SetDateValue(document, "created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetBucketResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetBucketResponse &r) {
        os << "GetBucketResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
