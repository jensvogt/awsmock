//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolCountersRequest : Common::BaseDto<ListUserPoolCountersRequest> {

        /**
         * User pool name prefix
         */
        std::string prefix;

        /**
         * Maximal number of results
         */
        int pageSize{};

        /**
         * Page index
         */
        int pageIndex{};

        /**
         * Sort columns
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_REQUEST_H
