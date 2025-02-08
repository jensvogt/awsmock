//
// Created by vogje01 on 31/05/2023.
//

#include <awsmock/dto/lambda/intern/StopFunctionRequest.h>

namespace AwsMock::Dto::Lambda {

    void StopFunctionRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionArn = Core::Bson::BsonUtils::GetStringValue(document, "FunctionArn");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StopFunctionRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StopFunctionRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const StopFunctionRequest &r) {
        os << "StopFunctionRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
