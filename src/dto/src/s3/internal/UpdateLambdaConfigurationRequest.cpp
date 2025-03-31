
#include <awsmock/dto/s3/internal/UpdateLambdaConfigurationRequest.h>

namespace AwsMock::Dto::S3 {

    void UpdateLambdaConfigurationRequest::FromJson(const std::string &body) {

        try {

            const value rootDocument = bsoncxx::from_json(body);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "region");
            bucket = Core::Bson::BsonUtils::GetStringValue(rootDocument, "bucket");

            // Lambda configuration
            if (rootDocument.view().find("lambdaConfiguration") != rootDocument.view().end()) {
                lambdaConfiguration.FromDocument(rootDocument.view()["lambdaConfiguration"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateLambdaConfigurationRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "bucket", bucket);
            Core::Bson::BsonUtils::SetDocumentValue(rootDocument, "lambdaConfiguration", lambdaConfiguration.ToDocument());
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string UpdateLambdaConfigurationRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UpdateLambdaConfigurationRequest &r) {
        os << "UpdateLambdaConfigurationRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3