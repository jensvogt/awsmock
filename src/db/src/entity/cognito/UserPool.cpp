//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/cognito/UserPool.h>

namespace AwsMock::Database::Entity::Cognito {

    UserPoolClient UserPool::GetClient(const std::string &clientId) {
        const auto it = std::ranges::find_if(userPoolClients,
                                             [clientId](const UserPoolClient &client) {
                                                 return client.clientId == clientId;
                                             });
        if (it != userPoolClients.end()) {
            return *it;
        }
        return {};
    }

    view_or_value<view, value> UserPool::ToDocument() const {

        // Domain
        document domainDoc{};
        domainDoc.append(kvp("domain", domain.domain));

        // Clients
        auto userPoolClientsDoc = array{};
        for (const auto &userPoolClient: userPoolClients) {
            userPoolClientsDoc.append(userPoolClient.ToDocument());
        }

        // Tags
        auto tagsDoc = document{};
        for (const auto &[fst, snd]: tags) {
            tagsDoc.append(kvp(fst, snd));
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

    void UserPool::FromDocument(const std::optional<view> &mResult) {

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
                for (const bsoncxx::array::view userPoolClientView{mResult.value()["userPoolClients"].get_array().value}; const bsoncxx::array::element &userPoolClientElement: userPoolClientView) {
                    UserPoolClient userPoolClient;
                    userPoolClient.FromDocument(userPoolClientElement.get_document().view());
                    userPoolClients.push_back(userPoolClient);
                }
            }

            // Get tags
            if (mResult.value().find("tags") != mResult.value().end()) {
                for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
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

}// namespace AwsMock::Database::Entity::Cognito