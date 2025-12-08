//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Lisr FTP user request
     *
     * Example:
     * @code{.json}
     * {
     *   "MaxResults": 100,
     *   "NextToken": "eyJNYXJrZXIiOiBudWxsLCAiYm90b1X0cnVuU2F0ZV9hbW91bnQiOiAyfQ==",
     *   "ServerId": "s-01234567890abcdef"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersRequest final : Common::BaseCounter<ListUsersRequest> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * Maximal number of results
         */
        int maxResults{};

        /**
         * Token
         */
        std::string nextToken;

      private:

        friend ListUsersRequest tag_invoke(boost::json::value_to_tag<ListUsersRequest>, boost::json::value const &v) {
            ListUsersRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.maxResults = Core::Json::GetIntValue(v, "MaxResults");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"MaxResults", obj.maxResults},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H
