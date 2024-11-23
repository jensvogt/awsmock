//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/secretsmanager/DeleteSecretRequest.h>

namespace AwsMock::Dto::SecretsManager {

    void DeleteSecretRequest::FromJson(const std::string &jsonString) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonString);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            // Attributes
            Core::JsonUtils::GetJsonValueString("Name", rootObject, name);

        } catch (Poco::Exception &exc) {
            throw Core::ServiceException(exc.message());
        }
    }

    std::string DeleteSecretRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const DeleteSecretRequest &r) {
        os << "DeleteSecretRequest={region='" << r.region << "', name='" << r.name << "'}";
        return os;
    }

}// namespace AwsMock::Dto::SecretsManager
