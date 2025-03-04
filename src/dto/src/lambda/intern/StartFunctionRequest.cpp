//
// Created by vogje01 on 31/05/2023.
//

#include <awsmock/dto/lambda/intern/StartFunctionRequest.h>

namespace AwsMock::Dto::Lambda {

    void StartFunctionRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StartFunctionRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StartFunctionRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const StartFunctionRequest &r) {
        os << "StartFunctionRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
