//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apps/Application.h>

namespace AwsMock::Database::Entity::Apps {

    view_or_value<view, value> Application::ToDocument() const {

        document userDocument;
        userDocument.append(kvp("region", region));
        userDocument.append(kvp("name", name));
        userDocument.append(kvp("runtime", runtime));
        userDocument.append(kvp("archive", archive));
        userDocument.append(kvp("version", version));
        userDocument.append(kvp("containerId", containerId));
        userDocument.append(kvp("enabled", enabled));
        userDocument.append(kvp("status", status));
        userDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        userDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));
        return userDocument.extract();
    }

    void Application::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        runtime = Core::Bson::BsonUtils::GetStringValue(mResult, "runtime");
        archive = Core::Bson::BsonUtils::GetStringValue(mResult, "archive");
        version = Core::Bson::BsonUtils::GetStringValue(mResult, "version");
        containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

    std::string Application::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Application::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Application &application) {
        os << "Application=" << application.ToJson();
        return os;
    }
}// namespace AwsMock::Database::Entity::Apps