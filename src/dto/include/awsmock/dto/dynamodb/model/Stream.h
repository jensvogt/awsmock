//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_STREAM_H
#define AWSMOCK_DTO_DYNAMODB_STREAM_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::optional;
    using std::chrono::system_clock;

    /**
     * @brief DynamoDB stream
     *
     * Example:
     * @code(.json)
     * {
     *   "StreamArn": "string",
     *   "StreamLabel": "string",
     *   "TableName": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Stream final : Common::BaseCounter<Stream> {

        /**
         * The Amazon Resource Name (ARN) for the stream.
         */
        std::string streamArn;

        /**
         * A timestamp, in ISO 8601 format, for this stream. Note that LatestStreamLabel is not a unique identifier for the stream, because
         * it is possible that a stream from another table might have the same timestamp. However, the combination of the following three
         * elements is guaranteed to be unique:
         */
        std::string streamLabel;

        /**
         * The DynamoDB table with which the stream is associated.
         */
        std::string tableName;

      private:

        friend Stream tag_invoke(boost::json::value_to_tag<Stream>, boost::json::value const &v) {
            Stream r;
            r.streamArn = Core::Json::GetStringValue(v, "StreamArn");
            r.streamLabel = Core::Json::GetStringValue(v, "StreamLabel");
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Stream const &obj) {
            jv = {
                    {"StreamArn", obj.streamArn},
                    {"StreamLabel", obj.streamLabel},
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_STREAM_H
