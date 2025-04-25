//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateUserPoolDomainRequest.h>

namespace AwsMock::Dto::Cognito {

    void CreateUserPoolDomainRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            domain = Core::Bson::BsonUtils::GetStringValue(document, "Domain");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateUserPoolDomainRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "Domain", domain);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);

            document.append(kvp("CustomDomainConfig", customDomainConfig.ToDocument()));

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateUserPoolDomainRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateUserPoolDomainRequest &r) {
        os << "CreateUserPoolDomainRequest=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Cognito