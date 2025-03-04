//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/CreateKeyResponse.h>

namespace AwsMock::Dto::KMS {

    void CreateKeyResponse::FromJson(const std::string &jsonString) {

        try {

            if (const value document = bsoncxx::from_json(jsonString); document.find("key") != document.end()) {
                key.FromDocument(document["key"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateKeyResponse::ToJson() const {

        try {

            document document;
            document.append(kvp("KeyMetadata", key.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateKeyResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateKeyResponse &r) {
        os << "CreateKeyResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::KMS
