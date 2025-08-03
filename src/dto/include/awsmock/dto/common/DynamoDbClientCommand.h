//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Supported DynamoDB client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class DynamoDbCommandType {
        CREATE_TABLE,
        LIST_TABLES,
        LIST_TABLE_ARNS,
        DESCRIBE_TABLE,
        DELETE_TABLE,
        GET_ITEM,
        PUT_ITEM,
        QUERY,
        SCAN,
        DELETE_ITEM,
        LIST_TABLE_COUNTERS,
        LIST_ITEM_COUNTERS,
        LIST_STREAMS,
        LIST_STREAMS_COUNTERS,
        GET_TABLE_DETAIL_COUNTERS,
        UNKNOWN
    };

    static std::map<DynamoDbCommandType, std::string> DynamoDbCommandTypeNames{
            {DynamoDbCommandType::CREATE_TABLE, "create-table"},
            {DynamoDbCommandType::LIST_TABLES, "list-tables"},
            {DynamoDbCommandType::LIST_TABLE_ARNS, "list-table-arns"},
            {DynamoDbCommandType::DESCRIBE_TABLE, "describe-table"},
            {DynamoDbCommandType::DELETE_TABLE, "delete-table"},
            {DynamoDbCommandType::GET_ITEM, "get-item"},
            {DynamoDbCommandType::PUT_ITEM, "put-item"},
            {DynamoDbCommandType::QUERY, "query"},
            {DynamoDbCommandType::SCAN, "scan"},
            {DynamoDbCommandType::DELETE_ITEM, "delete-item"},
            {DynamoDbCommandType::LIST_TABLE_COUNTERS, "list-table-counters"},
            {DynamoDbCommandType::LIST_ITEM_COUNTERS, "list-item-counters"},
            {DynamoDbCommandType::LIST_STREAMS, "list-streams"},
            {DynamoDbCommandType::LIST_STREAMS_COUNTERS, "list-stream-counters"},
            {DynamoDbCommandType::GET_TABLE_DETAIL_COUNTERS, "get-table-detail-counters"},
            {DynamoDbCommandType::UNKNOWN, "unknown"}};

    [[maybe_unused]] static std::string DynamoDbCommandTypeToString(const DynamoDbCommandType &commandType) {
        return DynamoDbCommandTypeNames[commandType];
    }

    [[maybe_unused]] static DynamoDbCommandType DynamoDbCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: DynamoDbCommandTypeNames) {
            if (Core::StringUtils::StartsWith(commandType, snd)) {
                return fst;
            }
        }
        return DynamoDbCommandType::UNKNOWN;
    }

    /**
     * @brief The DynamoDB client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * @par
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DynamoDbClientCommand : BaseClientCommand {

        /**
         * @brief Client command
         */
        DynamoDbCommandType command{};

        /**
         * @brief Gets the client command from the HTTP target header.
         *
         * @param request HTTP request
         * @return client command from AWS target header
         */
        static std::string GetClientCommandFromHeader(const http::request<http::dynamic_body> &request);

        /**
         * @brief Getś the value from the user-agent string
         *
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const DynamoDbClientCommand &i);

        /**
         * @brief AWS secret access key
         *
         * @return output stream
         */
        std::string _secretAccessKey = Core::Configuration::instance().GetValue<std::string>("awsmock.access.secret-access-key");
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H
