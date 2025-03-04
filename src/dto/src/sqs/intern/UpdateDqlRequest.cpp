//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/intern/UpdateDqlRequest.h>

namespace AwsMock::Dto::SQS {

    void UpdateDqlRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            queueArn = Core::Bson::BsonUtils::GetStringValue(document, "QueueArn");
            targetArn = Core::Bson::BsonUtils::GetStringValue(document, "TargetArn");
            retries = Core::Bson::BsonUtils::GetIntValue(document, "Retries");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateDqlRequest::ToJson() {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "QueueArn", queueArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TargetArn", targetArn);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "Retries", retries);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string UpdateDqlRequest::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, UpdateDqlRequest &r) {
        os << "UpdateDqlRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
