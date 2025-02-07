
#include <awsmock/dto/s3/internal/TouchObjectRequest.h>

namespace AwsMock::Dto::S3 {

    void TouchObjectRequest::FromJson(const std::string &body) {

        try {

            const value document = bsoncxx::from_json(body);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            bucket = Core::Bson::BsonUtils::GetStringValue(document, "bucket");
            key = Core::Bson::BsonUtils::GetStringValue(document, "key");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string TouchObjectRequest::ToJson() const {

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

    [[nodiscard]] std::string TouchObjectRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const TouchObjectRequest &r) {
        os << "TouchObjectRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3