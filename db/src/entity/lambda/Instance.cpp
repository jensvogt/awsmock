//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Instance.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Lambda"};
}

namespace Awsmock::Database::Entity::Lambda {

    void Instance::FromDocument(const std::optional<view> &mResult) {

        try {

            instanceId = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
            containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
            containerName = Core::Bson::BsonUtils::GetStringValue(mResult, "containerName");
            hostName = Core::Bson::BsonUtils::GetStringValue(mResult, "hostName");
            publicPort = Core::Bson::BsonUtils::GetIntValue(mResult, "publicPort");
            privatePort = Core::Bson::BsonUtils::GetIntValue(mResult, "privatePort");
            status = RuntimeStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
            invocations = Core::Bson::BsonUtils::GetLongValue(mResult, "invocations");
            avgDuration = Core::Bson::BsonUtils::GetDoubleValue(mResult, "avgDuration");
            lastStart = Core::Bson::BsonUtils::GetDateValue(mResult, "lastStart");
            lastStop = Core::Bson::BsonUtils::GetDateValue(mResult, "lastStop");
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
            instanceDoc.append(kvp("invocations", invocations));
            instanceDoc.append(kvp("avgDuration", avgDuration));
            instanceDoc.append(kvp("status", RuntimeStatusToString(status)));
            instanceDoc.append(kvp("lastStart", bsoncxx::types::b_date(lastStart)));
            instanceDoc.append(kvp("lastInvocation", bsoncxx::types::b_date(lastInvocation)));
            instanceDoc.append(kvp("lastStop", bsoncxx::types::b_date(lastStop)));
            return instanceDoc.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace Awsmock::Database::Entity::Lambda