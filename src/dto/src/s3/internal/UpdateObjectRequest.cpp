
#include <awsmock/dto/s3/internal/UpdateObjectRequest.h>

namespace AwsMock::Dto::S3 {

    void UpdateObjectRequest::FromJson(const std::string &body) {

        try {

            const value rootDocument = bsoncxx::from_json(body);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "region");
            bucket = Core::Bson::BsonUtils::GetStringValue(rootDocument, "bucket");
            key = Core::Bson::BsonUtils::GetStringValue(rootDocument, "key");

            // Metadata
            if (rootDocument.view().find("metadata") != rootDocument.view().end()) {
                for (const auto jsonMetadataArray = rootDocument.view()["metadata"].get_array().value; const auto &element: jsonMetadataArray) {
                    auto jsonObject = element.get_document().value;
                    std::string key = bsoncxx::string::to_string(jsonObject["key"].get_string().value);
                    const std::string value = bsoncxx::string::to_string(jsonObject["value"].get_string().value);
                    metadata[key] = value;
                }
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateObjectRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "bucket", bucket);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "key", key);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string UpdateObjectRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UpdateObjectRequest &r) {
        os << "UpdateObjectRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3