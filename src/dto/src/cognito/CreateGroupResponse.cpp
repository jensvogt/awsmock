//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateGroupResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string CreateGroupResponse::ToJson() {

        try {

            document document;
            document.append(kvp("Group", group.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito