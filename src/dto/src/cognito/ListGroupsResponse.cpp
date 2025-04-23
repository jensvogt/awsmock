//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListGroupsResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string ListGroupsResponse::ToJson() const {

        try {

            document document;
            if (!groups.empty()) {
                array jsonArray;
                for (const auto &group: groups) {
                    jsonArray.append(group.ToDocument());
                }
                document.append(kvp("Groups", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito