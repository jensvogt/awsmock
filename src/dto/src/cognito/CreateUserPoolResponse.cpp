//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateUserPoolResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string CreateUserPoolResponse::ToJson() {

        try {

            document rootDocument;

            document userPoolObject;
            Core::Bson::BsonUtils::SetStringValue(userPoolObject, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(userPoolObject, "Name", name);
            Core::Bson::BsonUtils::SetStringValue(userPoolObject, "Arn", arn);
            Core::Bson::BsonUtils::SetStringValue(userPoolObject, "Id", userPoolId);
            rootDocument.append(kvp("UserPool", userPoolObject));

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito