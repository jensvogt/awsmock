//
// Created by vogje01 on 12/18/23.
//

#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>

namespace AwsMock::Dto::Transfer {

    void IdentityProviderDetails::FromJson(const std::string &jsonString) {
        FromDocument(bsoncxx::from_json(jsonString));
    }

    std::string IdentityProviderDetails::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "DirectoryId", directoryId);
            Core::Bson::BsonUtils::SetStringValue(document, "Function", function);
            Core::Bson::BsonUtils::SetStringValue(document, "Url", url);
            Core::Bson::BsonUtils::SetStringValue(document, "InvocationRole", invocationRole);
            Core::Bson::BsonUtils::SetStringValue(document, "SftpAuthenticationMethods", SftpAuthenticationMethodToString(sftpAuthenticationMethod));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void IdentityProviderDetails::FromDocument(const view_or_value<view, value> &document) {

        try {
            directoryId = Core::Bson::BsonUtils::GetStringValue(document, "DirectoryId");
            function = Core::Bson::BsonUtils::GetStringValue(document, "Function");
            url = Core::Bson::BsonUtils::GetStringValue(document, "Url");
            invocationRole = Core::Bson::BsonUtils::GetStringValue(document, "InvocationRole");
            sftpAuthenticationMethod = SftpAuthenticationMethodFromString(Core::Bson::BsonUtils::GetStringValue(document, "SftpAuthenticationMethod"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string IdentityProviderDetails::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const IdentityProviderDetails &r) {
        os << "IdentityProviderDetails=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
