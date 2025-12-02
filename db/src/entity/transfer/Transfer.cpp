//
// Created by vogje01 on 07/06/2023.
//

#include <awsmock/entity/transfer/Transfer.h>
#include <boost/json/object.hpp>

namespace AwsMock::Database::Entity::Transfer {

    bool Transfer::HasUser(const std::string &userName) {
        return std::ranges::find_if(users, [userName](const User &user) {
                   return user.userName == userName;
               }) != users.end();
    }

    bool Transfer::HasProtocol(const std::string &p) {
        return std::ranges::find_if(protocols, [p](const Protocol &protocol) {
                   return protocol == ProtocolFromString(p);
               }) != protocols.end();
    }

    bool Transfer::HasProtocol(const Protocol &p) {
        return std::ranges::find_if(protocols, [p](const Protocol &protocol) {
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

        document transferDoc;
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "region", region);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "serverId", serverId);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "arn", arn);
        Core::Bson::BsonUtils::SetIntValue(transferDoc, "concurrency", concurrency);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "listenAddress", listenAddress);
        Core::Bson::BsonUtils::SetStringValue(transferDoc, "state", ServerStateToString(state));
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "lastStarted", lastStarted);
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "created", created);
        Core::Bson::BsonUtils::SetDateValue(transferDoc, "modified", modified);

        // Users
        if (!users.empty()) {
            auto usersArray = array{};
            for (const auto &user: users) {
                usersArray.append(user.ToDocument());
            }
            transferDoc.append(kvp("users", usersArray));
        }

        // Protocols
        if (!protocols.empty()) {
            auto protocolsArray = array{};
            for (const auto &protocol: protocols) {
                protocolsArray.append(ProtocolToString(protocol));
            }
            transferDoc.append(kvp("protocols", protocolsArray));
        }

        // Ports
        if (!ports.empty()) {
            array portsArray;
            for (const auto &p: ports) {
                portsArray.append(p);
            }
            transferDoc.append(kvp("ports", portsArray));
        }

        // Tags
        if (!tags.empty()) {
            document tagsDocument;
            for (const auto &[fst, snd]: tags) {
                tagsDocument.append(kvp(fst, snd));
            }
            transferDoc.append(kvp("tags", tagsDocument));
        }

        return transferDoc.extract();
    }

    void Transfer::FromDocument(const view_or_value<view, value> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        serverId = Core::Bson::BsonUtils::GetStringValue(mResult, "serverId");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        state = ServerStateFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "state"));
        concurrency = Core::Bson::BsonUtils::GetIntValue(mResult, "concurrency");
        listenAddress = Core::Bson::BsonUtils::GetStringValue(mResult, "listenAddress");
        lastStarted = Core::Bson::BsonUtils::GetDateValue(mResult, "lastStarted");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Protocols
        if (mResult.view().find("protocols") != mResult.view().end()) {
            for (const bsoncxx::array::view jsonArray = mResult.view()["protocols"].get_array().value; auto &p: jsonArray) {
                if (Protocol protocol = ProtocolFromString(bsoncxx::string::to_string(p.get_string().value)); std::ranges::find(protocols.begin(), protocols.end(), protocol) == protocols.end()) {
                    protocols.emplace_back(ProtocolFromString(bsoncxx::string::to_string(p.get_string().value)));
                }
            }
        }

        // Ports
        if (mResult.view().find("ports") != mResult.view().end()) {
            for (const bsoncxx::array::view jsonArray = mResult.view()["ports"].get_array().value; auto &p: jsonArray) {
                if (const int port = p.get_int32().value; std::ranges::find(ports.begin(), ports.end(), port) == ports.end()) {
                    ports.emplace_back(p.get_int32().value);
                }
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

        // Tags
        if (mResult.view().find("tags") != mResult.view().end()) {
            for (const view tagsView = mResult.view()["tags"].get_document().value; const bsoncxx::document::element &tagsElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagsElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                tags.emplace(key, value);
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
