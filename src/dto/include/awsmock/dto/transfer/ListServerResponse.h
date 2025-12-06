//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_SERVER_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_SERVER_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/Server.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief List transfer servers response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListServerResponse final : Common::BaseCounter<ListServerResponse> {

        /**
         * Next token ID
         */
        std::string nextToken{};

        /**
         * Server list
         */
        std::vector<Server> servers;

      private:

        friend ListServerResponse tag_invoke(boost::json::value_to_tag<ListServerResponse>, boost::json::value const &v) {
            ListServerResponse r;
            if (!r.nextToken.empty()) {
                r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListServerResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"NextToken", obj.nextToken},
                    {"Servers", boost::json::value_from(obj.servers)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_SERVER_RESPONSE_H
