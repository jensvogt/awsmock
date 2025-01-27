//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/dto/lambda/model/FunctionCounter.h>

namespace AwsMock::Dto::Lambda {

    view_or_value<view, value> FunctionCounter::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionName", functionName);
            Core::Bson::BsonUtils::SetStringValue(document, "Handler", handler);
            Core::Bson::BsonUtils::SetStringValue(document, "Runtime", runtime);
            Core::Bson::BsonUtils::SetStringValue(document, "Description", description);
            Core::Bson::BsonUtils::SetLongValue(document, "CodeSize", codeSize);
            Core::Bson::BsonUtils::SetStringValue(document, "CodeSha256", codeSha256);
            Core::Bson::BsonUtils::SetStringValue(document, "Version", version);
            Core::Bson::BsonUtils::SetIntValue(document, "Timeout", timeout);
            Core::Bson::BsonUtils::SetStringValue(document, "State", state);
            Core::Bson::BsonUtils::SetStringValue(document, "StateReason", stateReason);
            Core::Bson::BsonUtils::SetStringValue(document, "StateReasonCode", stateReasonCode);
            Core::Bson::BsonUtils::SetStringValue(document, "LastUpdateStatus", lastUpdateStatusReason);
            Core::Bson::BsonUtils::SetStringValue(document, "LastUpdateStatusCode", lastUpdateStatusReasonCode);
            Core::Bson::BsonUtils::SetDateValue(document, "LastModified", lastModified);

            // Tags
            if (!tags.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: tags) {
                    bsoncxx::builder::basic::document jsonObject;
                    jsonObject.append(kvp(fst, snd));
                }
                document.append(kvp("Tags", jsonArray));
            }

            // Environment
            document.append(kvp("Environment", environment.ToDocument()));

            // Architectures
            if (!architectures.empty()) {
                array jsonArray;
                for (const auto &architecture: architectures) {
                    jsonArray.append(architecture);
                }
                document.append(kvp("Architectures", jsonArray));
            }

            // Dead letter config
            document.append(kvp("DeadLetterConfig", deadLetterConfig.ToDocument()));
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string FunctionCounter::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const FunctionCounter &f) {
        os << "FunctionCounter=" << to_json(f.ToDocument());
        return os;
    }
}// namespace AwsMock::Dto::Lambda
