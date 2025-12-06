//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETMANAGER_DELETE_SECRET_RESPONSE_H
#define AWSMOCK_DTO_SECRETMANAGER_DELETE_SECRET_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Return structure for the delete secret request.
     *
     * Example:
     * @code{.json}
     * {
     *   "ARN": "string",
     *   "DeletionDate": number,
     *   "Name": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteSecretResponse final : Common::BaseCounter<DeleteSecretResponse> {

        /**
         * Secret name
         */
        std::string name;

        /**
         * Secret ARN
         */
        std::string arn;

        /**
         * Secret deletion date
         */
        system_clock::time_point deletionDate;

      private:

        friend DeleteSecretResponse tag_invoke(boost::json::value_to_tag<DeleteSecretResponse>, boost::json::value const &v) {
            DeleteSecretResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.deletionDate = Core::Json::GetUnixTimestampValue(v, "DeletionDate");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteSecretResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"DeletionDate", Core::DateTimeUtils::UnixTimestamp(obj.deletionDate)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETMANAGER_DELETE_SECRET_RESPONSE_H
