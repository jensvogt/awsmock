//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H

// C++ standard includes
#include <string>
#include <map>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Query request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueryRequest : public Dto::Common::BaseRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Original HTTP request body
         */
        std::string body;

        /**
         * Original HTTP request headers
         */
        std::map<std::string, std::string> headers;

        /**
         * Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Parse a JSON stream
         *
         * @param jsonBody JSON string
         */
        void FromJson(const std::string &jsonBody);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const QueryRequest &r);
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
