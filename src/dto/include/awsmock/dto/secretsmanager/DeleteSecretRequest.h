//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Delete secret request
     *
     * Example:
     * @code{.json}
     * {
     *   "Name": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteSecretRequest final : Common::BaseCounter<DeleteSecretRequest> {

        /**
         * Secret name
         */
        std::string name;

      private:

        friend DeleteSecretRequest tag_invoke(boost::json::value_to_tag<DeleteSecretRequest>, boost::json::value const &v) {
            DeleteSecretRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H
