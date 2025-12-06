//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H
#define AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/Secret.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief List secrets response
     *
     * Example:
     * @code{.json}
     * {
     *    "NextToken": "string",
     *    "SecretList": [
     *    .....
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListSecretsResponse final : Common::BaseCounter<ListSecretsResponse> {

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * List of secrets
         */
        std::vector<Secret> secretList;

      private:

        friend ListSecretsResponse tag_invoke(boost::json::value_to_tag<ListSecretsResponse>, boost::json::value const &v) {
            ListSecretsResponse r;
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.secretList = boost::json::value_to<std::vector<Secret>>(v.at("SecretList"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"NextToken", obj.nextToken},
                    {"SecretList", boost::json::value_from(obj.secretList)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H
