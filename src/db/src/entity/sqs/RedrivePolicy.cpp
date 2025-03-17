//
// Created by vogje01 on 12/10/23.
//

#include <awsmock/entity/sqs/RedrivePolicy.h>

namespace AwsMock::Database::Entity::SQS {

    std::string RedrivePolicy::ToJson() const {

        try {

            return Core::Bson::BsonUtils::ToJsonString(ToDocument());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void RedrivePolicy::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            deadLetterTargetArn = Core::Bson::BsonUtils::GetStringValue(document, "deadLetterTargetArn");
            maxReceiveCount = std::stoi(Core::Bson::BsonUtils::GetStringValue(document, "maxReceiveCount"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> RedrivePolicy::ToDocument() const {

        document redrivePolicyDoc;
        Core::Bson::BsonUtils::SetStringValue(redrivePolicyDoc, "deadLetterTargetArn", deadLetterTargetArn);
        Core::Bson::BsonUtils::SetIntValue(redrivePolicyDoc, "maxReceiveCount", maxReceiveCount);

        return redrivePolicyDoc.extract();
    }

    void RedrivePolicy::FromDocument(const std::optional<view> &mResult) {

        deadLetterTargetArn = Core::Bson::BsonUtils::GetStringValue(mResult, "deadLetterTargetArn");
        maxReceiveCount = Core::Bson::BsonUtils::GetIntValue(mResult, "maxReceiveCount");
    }

    std::string RedrivePolicy::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const RedrivePolicy &r) {
        os << "RedrivePolicy=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Database::Entity::SQS
