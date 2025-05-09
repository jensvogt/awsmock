//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_SERVER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_LIST_SERVER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief List transfer servers request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListServerRequest final : Common::BaseCounter<ListServerRequest> {

        /**
         * Maximal number of results
         */
        long maxResults = 0;

        /**
         * Token
         */
        std::string nextToken{};

      private:

        friend ListServerRequest tag_invoke(boost::json::value_to_tag<ListServerRequest>, boost::json::value const &v) {
            ListServerRequest r;
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListServerRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MaxResults", obj.maxResults},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_SERVER_REQUEST_H
