//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/intern/UpdateFunctionTagRequest.h>

namespace AwsMock::Dto::Lambda {

    void UpdateFunctionTagRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");
            tagKey = Core::Bson::BsonUtils::GetStringValue(document, "Key");
            tagValue = Core::Bson::BsonUtils::GetStringValue(document, "Value");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateFunctionTagRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(document, "Key", tagKey);
            Core::Bson::BsonUtils::SetStringValue(document, "Value", tagValue);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateFunctionTagRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UpdateFunctionTagRequest &r) {
        os << "UpdateFunctionTagRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
