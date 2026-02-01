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
            hostName = Core::Bson::BsonUtils::GetStringValue(mResult, "hostName");
            publicPort = Core::Bson::BsonUtils::GetIntValue(mResult, "publicPort");
            privatePort = Core::Bson::BsonUtils::GetIntValue(mResult, "privatePort");
            status = LambdaInstanceStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            lastInvocation = Core::Bson::BsonUtils::GetDateValue(mResult, "lastInvocation");

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
            instanceDoc.append(kvp("hostName", hostName));
            instanceDoc.append(kvp("publicPort", publicPort));
            instanceDoc.append(kvp("privatePort", privatePort));
            instanceDoc.append(kvp("status", LambdaInstanceStatusToString(status)));
            instanceDoc.append(kvp("created", bsoncxx::types::b_date(created)));
            instanceDoc.append(kvp("lastInvocation", bsoncxx::types::b_date(lastInvocation)));
            return instanceDoc.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::Lambda