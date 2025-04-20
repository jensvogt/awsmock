//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolRequest : Common::BaseRequest<ListUserPoolRequest> {

        /**
         * Page size
         */
        int pageSize{};

        /**
         * Page index
         */
        int pageIndex{};

        /**
         * Page index
         */
        std::vector<Core::SortColumn> sortColumns{};

        /**
         * Convert from a JSON object.
         *
         * @param jsonString json string object
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_REQUEST_H
