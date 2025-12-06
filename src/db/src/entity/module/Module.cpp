//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/module/Module.h>

namespace AwsMock::Database::Entity::Module {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    view_or_value<view, value> Module::ToDocument() const {

        view_or_value<view, value> objectDoc = make_document(
                kvp("name", name),
                kvp("port", port),
                kvp("state", ModuleStateToString(state)),
                kvp("status", ModuleStatusToString(status)),
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
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

    std::string Module::ToJson() const {
        return to_json(ToDocument());
    }

    std::string Module::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Module &m) {
        os << "Module=" << m.ToJson();
        return os;
    }

}// namespace AwsMock::Database::Entity::Module