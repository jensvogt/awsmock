//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/GetFunctionResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string GetFunctionResponse::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "User", user);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "Timeout", timeout);

            // Configuration
            rootDocument.append(kvp("Configuration", configuration.ToDocument()));

            // Code
            rootDocument.append(kvp("Code", code.ToDocument()));

            // Tags
            if (!tags.empty()) {
                document jsonObject;
                for (const auto &[fst, snd]: tags) {
                    jsonObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("Tags", jsonObject));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetFunctionResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetFunctionResponse &r) {
        os << "GetFunctionResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
