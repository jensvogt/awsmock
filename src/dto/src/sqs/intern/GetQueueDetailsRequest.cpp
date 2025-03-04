//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/intern/GetQueueDetailsRequest.h>

namespace AwsMock::Dto::SQS {

    void GetQueueDetailsRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            queueArn = Core::Bson::BsonUtils::GetStringValue(document, "QueueArn");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string GetQueueDetailsRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "QueueArn", queueArn);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    [[nodiscard]] std::string GetQueueDetailsRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetQueueDetailsRequest &r) {
        os << "GetQueueDetailsRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
