//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/s3/GetObjectRequest.h>

namespace AwsMock::Dto::S3 {

    std::string GetObjectRequest::ToJson() const {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "bucket", bucket);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "key", key);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "versionId", versionId);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "min", min);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "max", max);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::S3
