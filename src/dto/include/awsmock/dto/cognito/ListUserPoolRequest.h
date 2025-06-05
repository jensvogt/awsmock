//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolRequest final : Common::BaseCounter<ListUserPoolRequest> {

        /**
         * Page size
         */
        long maxResults{};

      private:

        friend ListUserPoolRequest tag_invoke(boost::json::value_to_tag<ListUserPoolRequest>, boost::json::value const &v) {
            ListUserPoolRequest r;
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserPoolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MaxResults", obj.maxResults},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H
