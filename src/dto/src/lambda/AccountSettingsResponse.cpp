//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/AccountSettingsResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string AccountSettingsResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "User", user);

            document.append(kvp("AccountLimit", accountLimit.ToDocument()));
            document.append(kvp("AccountUsage", accountUsage.ToDocument()));

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Lambda
