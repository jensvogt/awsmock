//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_MESSAGES_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief List messages request
     *
     * @par
     * This is not an AWS SDK request, but only available in awsmock
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * @brief Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListMessagesRequest &r);
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_PUBLISH_REQUEST_H
