//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/intern/AddFunctionTagRequest.h>

namespace AwsMock::Dto::Lambda {

    void AddFunctionTagRequest::FromJson(const std::string &jsonString) {

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

    std::string AddFunctionTagRequest::ToJson() const {

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

    std::string AddFunctionTagRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AddFunctionTagRequest &r) {
        os << "AddFunctionTagRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
