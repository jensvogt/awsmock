//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/intern/ResendMessageRequest.h>

namespace AwsMock::Dto::SQS {

    void ResendMessageRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            queueArn = Core::Bson::BsonUtils::GetStringValue(document, "QueueArn");
            messageId = Core::Bson::BsonUtils::GetStringValue(document, "MessageId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ResendMessageRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "QueueArn", queueArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "MessageId", messageId);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string ResendMessageRequest::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, ResendMessageRequest &r) {
        os << "ResendMessageRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
