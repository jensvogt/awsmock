//
// Created by vogje01 on 4/20/24.
//

#include <awsmock/dto/cognito/model/PreTokenGenerationConfig.h>

namespace AwsMock::Dto::Cognito {

    std::string PreTokenGenerationConfig::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "lambdaArn", lambdaArn);
            Core::Bson::BsonUtils::SetStringValue(document, "lambdaVersion", lambdaVersion);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> PreTokenGenerationConfig::ToDocument() const {
        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "lambdaArn", lambdaArn);
            Core::Bson::BsonUtils::SetStringValue(document, "lambdaVersion", lambdaVersion);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string PreTokenGenerationConfig::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const PreTokenGenerationConfig &r) {
        os << "PreTokenGenerationConfig=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Cognito