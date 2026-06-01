//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/module/Module.h>

namespace AwsMock::Database::Entity::Module {

    view_or_value<view, value> Module::ToDocument() const {

        view_or_value<view, value> objectDoc = make_document(
            kvp("name", name),
            kvp("port", port),
            kvp("state", ModuleStateToString(state)),
            kvp("status", ModuleStatusToString(status)),
            kvp("logChannel", logChannel),
            kvp("logLevel", logLevel),
            kvp("created", bsoncxx::types::b_date(created)),
            kvp("modified", bsoncxx::types::b_date(modified)));

        return objectDoc;
    }

    void Module::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        port = Core::Bson::BsonUtils::GetIntValue(mResult, "port");
        state = ModuleStateFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "state"));
        status = ModuleStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
        logChannel = Core::Bson::BsonUtils::GetStringValue(mResult, "logChannel");
        logLevel = Core::Bson::BsonUtils::GetStringValue(mResult, "logLevel");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

} // namespace AwsMock::Database::Entity::Module
