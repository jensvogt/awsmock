//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/ScheduleKeyDeletionRequest.h>

namespace AwsMock::Dto::KMS {

    void ScheduleKeyDeletionRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");
            pendingWindowInDays = Core::Bson::BsonUtils::GetIntValue(document, "PendingWindowInDays");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ScheduleKeyDeletionRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetIntValue(document, "PendingWindowInDays", pendingWindowInDays);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
