//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/intern/UpdateFunctionEnvironmentRequest.h>

namespace AwsMock::Dto::Lambda {

    void UpdateFunctionEnvironmentRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");
            environmentKey = Core::Bson::BsonUtils::GetStringValue(document, "Key");
            environmentValue = Core::Bson::BsonUtils::GetStringValue(document, "Value");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateFunctionEnvironmentRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(document, "Key", environmentKey);
            Core::Bson::BsonUtils::SetStringValue(document, "Value", environmentValue);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateFunctionEnvironmentRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UpdateFunctionEnvironmentRequest &r) {
        os << "UpdateFunctionEnvironmentRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
