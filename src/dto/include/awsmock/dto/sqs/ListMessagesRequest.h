//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS messages request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Sort column
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListMessagesRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H
