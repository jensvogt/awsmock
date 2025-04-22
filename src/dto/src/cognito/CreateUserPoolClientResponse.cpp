//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateUserPoolClientResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string CreateUserPoolClientResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            document.append(kvp("UserGroupClient", userGroupClient.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito