//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sns/PublishRequest.h>

namespace AwsMock::Dto::SNS {

    std::string PublishRequest::ToJson() const {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TopicArn", topicArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TargetArn", targetArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Message", message);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string PublishRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const PublishRequest &r) {
        os << "PublishRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
