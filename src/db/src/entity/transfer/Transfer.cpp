//
// Created by vogje01 on 07/06/2023.
//

#include <awsmock/entity/transfer/Transfer.h>

namespace AwsMock::Database::Entity::Transfer {

    bool Transfer::HasUser(const std::string &userName) {
        return std::ranges::find_if(users, [userName](const User &user) {
                   return user.userName == userName;
               }) != users.end();
    }

    bool Transfer::HasProtocol(const std::string &p) {
        return std::ranges::find_if(protocols, [p](const std::string &protocol) {
                   return protocol == p;
               }) != protocols.end();
    }

    User Transfer::GetUser(const std::string &userName) {

        const auto it = std::ranges::find_if(users, [userName](const User &user) {
            return user.userName == userName;
        });
        if (it != users.end()) {
            return *it;
        }
        return {};
    }

    view_or_value<view, value> Transfer::ToDocument() const {

        auto protocolsDoc = array{};
        for (const auto &protocol: protocols) {
            protocolsDoc.append(protocol);
        }

        auto usersDoc = array{};
        for (const auto &user: users) {
            usersDoc.append(user.ToDocument());
        }

        document transferDoc;
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "region", region);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "serverId", serverId);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "arn", arn);
        Core::Bson::BsonUtils::SetIntValue(transferDoc, "port", port);
        Core::Bson::BsonUtils::SetIntValue(transferDoc, "concurrency", concurrency);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "listenAddress", listenAddress);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "state", ServerStateToString(state));
        Core::Bson::BsonUtils::SetArrayValue(transferDoc, "protocols", protocolsDoc);
        Core::Bson::BsonUtils::SetArrayValue(transferDoc, "users", usersDoc);
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "lastStarted", lastStarted);
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "created", created);
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "modified", modified);

        return transferDoc.extract();
    }

    void Transfer::FromDocument(const view_or_value<view,value> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        serverId = Core::Bson::BsonUtils::GetStringValue(mResult, "serverId");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        state = ServerStateFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "state"));
        concurrency = Core::Bson::BsonUtils::GetIntValue(mResult, "concurrency");
        port = Core::Bson::BsonUtils::GetIntValue(mResult, "port");
        listenAddress = Core::Bson::BsonUtils::GetStringValue(mResult, "listenAddress");
        lastStarted = Core::Bson::BsonUtils::GetDateValue(mResult, "lastStarted");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Protocols
        if (mResult.view().find("protocols") != mResult.view().end()) {
            auto [value] = mResult.view()["protocols"].get_array();
            for (auto &p: value) {
                protocols.emplace_back(bsoncxx::string::to_string(p.get_string().value));
            }
        }

        // Users
        if (mResult.view().find("users") != mResult.view().end()) {
            for (const bsoncxx::array::view usersView{mResult.view()["users"].get_array().value}; const bsoncxx::array::element &userElement: usersView) {
                User user = {
                        .userName = bsoncxx::string::to_string(userElement["userName"].get_string().value),
                        .password = bsoncxx::string::to_string(userElement["password"].get_string().value),
                        .homeDirectory = bsoncxx::string::to_string(userElement["homeDirectory"].get_string().value),
                        .arn = bsoncxx::string::to_string(userElement["arn"].get_string().value)};
                users.push_back(user);
            }
        }
    }

    std::string Transfer::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Transfer::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Transfer &t) {
        os << "Transfer=" << t.ToJson();
        return os;
    }
}// namespace AwsMock::Database::Entity::Transfer
