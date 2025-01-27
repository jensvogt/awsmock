//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/UploadFunctionCodeRequest.h>

namespace AwsMock::Dto::Lambda {

    std::string UploadFunctionCodeRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "FunctionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "FunctionCode", functionCode);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Version", version);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void UploadFunctionCodeRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");
            functionCode = Core::Bson::BsonUtils::GetStringValue(document, "FunctionCode");
            version = Core::Bson::BsonUtils::GetStringValue(document, "Version");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UploadFunctionCodeRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UploadFunctionCodeRequest &r) {
        os << "UploadFunctionCodeRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
