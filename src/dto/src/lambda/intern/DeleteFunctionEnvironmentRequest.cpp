//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/intern/DeleteFunctionEnvironmentRequest.h>

namespace AwsMock::Dto::Lambda {

    void DeleteFunctionEnvironmentRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");
            environmentKey = Core::Bson::BsonUtils::GetStringValue(document, "Key");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteFunctionEnvironmentRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(document, "Key", environmentKey);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteFunctionEnvironmentRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const DeleteFunctionEnvironmentRequest &r) {
        os << "DeleteFunctionEnvironmentRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
