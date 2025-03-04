//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/intern/GetFunctionCountersResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string GetFunctionCountersResponse::ToJson() const {

        try {

            document document;

            Core::Bson::BsonUtils::SetStringValue(document, "id", id);
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "functionName", functionName);
            Core::Bson::BsonUtils::SetStringValue(document, "functionArn", functionArn);
            Core::Bson::BsonUtils::SetStringValue(document, "handler", handler);
            Core::Bson::BsonUtils::SetStringValue(document, "runtime", runtime);
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "zipFile", zipFile);
            Core::Bson::BsonUtils::SetStringValue(document, "s3Bucket", s3Bucket);
            Core::Bson::BsonUtils::SetStringValue(document, "s3Key", s3Key);
            Core::Bson::BsonUtils::SetStringValue(document, "s3ObjectVersion", s3ObjectVersion);
            Core::Bson::BsonUtils::SetStringValue(document, "role", role);
            Core::Bson::BsonUtils::SetLongValue(document, "size", size);
            Core::Bson::BsonUtils::SetStringValue(document, "version", version);
            Core::Bson::BsonUtils::SetLongValue(document, "concurrency", concurrency);
            Core::Bson::BsonUtils::SetLongValue(document, "invocations", invocations);
            Core::Bson::BsonUtils::SetLongValue(document, "averageRuntime", averageRuntime);
            Core::Bson::BsonUtils::SetDateValue(document, "lastInvocation", lastInvocation);
            Core::Bson::BsonUtils::SetDateValue(document, "lastStarted", lastStarted);
            Core::Bson::BsonUtils::SetDateValue(document, "created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);

            // Environment
            if (!environment.empty()) {
                bsoncxx::builder::basic::document envDoc;
                for (const auto &[fst, snd]: environment) {
                    envDoc.append(kvp(fst, snd));
                }
                document.append(kvp("environment", envDoc));
            }

            // Tags
            if (!tags.empty()) {
                bsoncxx::builder::basic::document tagDoc;
                for (const auto &[fst, snd]: tags) {
                    tagDoc.append(kvp(fst, snd));
                }
                document.append(kvp("tags", tagDoc));
            }
            return bsoncxx::to_json(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetFunctionCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetFunctionCountersResponse &r) {
        os << "GetFunctionCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
