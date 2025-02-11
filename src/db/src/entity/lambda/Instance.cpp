//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Instance.h>

namespace AwsMock::Database::Entity::Lambda {

    void Instance::FromDocument(const std::optional<view> &mResult) {

        try {

            instanceId = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
            containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
            containerName = Core::Bson::BsonUtils::GetStringValue(mResult, "containerName");
            hostPort = Core::Bson::BsonUtils::GetIntValue(mResult, "hostPort");
            status = LambdaInstanceStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> Instance::ToDocument() const {

        try {

            document instanceDoc{};
            instanceDoc.append(kvp("id", instanceId));
            instanceDoc.append(kvp("containerId", containerId));
            instanceDoc.append(kvp("containerName", containerName));
            instanceDoc.append(kvp("hostPort", hostPort));
            instanceDoc.append(kvp("status", LambdaInstanceStatusToString(status)));
            instanceDoc.append(kvp("created", bsoncxx::types::b_date(created)));
            return instanceDoc.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> Instance::ToDocument() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "id", instanceId);
            Core::Bson::BsonUtils::SetStringValue(document, "status", LambdaInstanceStatusToString(status));
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string Instance::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Instance &t) {
        os << "Instance=" << to_json(t.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Lambda