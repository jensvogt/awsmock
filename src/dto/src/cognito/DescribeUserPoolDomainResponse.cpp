//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/DescribeUserPoolDomainResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string DescribeUserPoolDomainResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "Domain", domain);
            Core::Bson::BsonUtils::SetStringValue(document, "Status", status);
            Core::Bson::BsonUtils::SetStringValue(document, "S3Bucket", s3Bucket);
            Core::Bson::BsonUtils::SetStringValue(document, "Version", version);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito