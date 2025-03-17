//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queue tags request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueueTagsRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * @brief Sort column
         */
        std::vector<Core::SortColumn> sortColumns;

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
        friend std::ostream &operator<<(std::ostream &os, const ListQueueTagsRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H
