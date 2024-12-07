//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/cognito/UserPool.h>

namespace AwsMock::Database::Entity::Cognito {

    UserPoolClient UserPool::GetClient(const std::string &clientId) {
        auto it = find_if(userPoolClients.begin(),
                          userPoolClients.end(),
                          [clientId](const Entity::Cognito::UserPoolClient &client) {
                              return client.clientId == clientId;
                          });
        if (it != userPoolClients.end()) {
            return *it;
        }
        return {};
    }

    view_or_value<view, value> UserPool::ToDocument() const {

        // Domain
        bsoncxx::builder::basic::document domainDoc{};
        domainDoc.append(kvp("domain", domain.domain));

        // Clients
        auto userPoolClientsDoc = bsoncxx::builder::basic::array{};
        for (const auto &userPoolClient: userPoolClients) {
            userPoolClientsDoc.append(userPoolClient.ToDocument());
        }

        // Tags
        auto tagsDoc = bsoncxx::builder::basic::document{};
        for (const auto &tag: tags) {
            tagsDoc.append(kvp(tag.first, tag.second));
        }

        view_or_value<view, value> userPoolDocument = make_document(
                kvp("region", region),
                kvp("userPoolId", userPoolId),
                kvp("name", name),
                kvp("arn", arn),
                kvp("kmsKey", kmsKey),
                kvp("domain", domainDoc.extract()),
                kvp("userPoolClients", userPoolClientsDoc),
                kvp("tags", tagsDoc),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));
        return userPoolDocument;
    }

    void UserPool::FromDocument(std::optional<bsoncxx::document::view> mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(mResult, "userPoolId");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
            kmsKey = Core::Bson::BsonUtils::GetStringValue(mResult, "kmsKey");
            arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get domain
            domain.domain = Core::Bson::BsonUtils::GetStringValue(mResult, "domain");

            // Get clients
            if (mResult.value().find("userPoolClients") != mResult.value().end()) {
                bsoncxx::array::view userPoolClientView{mResult.value()["userPoolClients"].get_array().value};
                for (const bsoncxx::array::element &userPoolClientElement: userPoolClientView) {
                    UserPoolClient userPoolClient;
                    userPoolClient.FromDocument(userPoolClientElement.get_document().view());
                    userPoolClients.push_back(userPoolClient);
                }
            }

            // Get tags
            if (mResult.value().find("tags") != mResult.value().end()) {
                bsoncxx::document::view tagsView = mResult.value()["tags"].get_document().value;
                for (const bsoncxx::document::element &tagElement: tagsView) {
                    std::string key = bsoncxx::string::to_string(tagElement.key());
                    std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                    tags.emplace(key, value);
                }
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    std::string UserPool::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UserPool &u) {
        os << "UserPool=" << bsoncxx::to_json(u.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Cognito