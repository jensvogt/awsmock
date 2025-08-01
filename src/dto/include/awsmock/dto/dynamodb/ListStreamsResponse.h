//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/Stream.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListStreamsResponse final : Common::BaseCounter<ListStreamsResponse> {

        /**
         * The stream ARN of the item where the operation stopped, inclusive of the previous result set. Use this value to start a
         * new operation, excluding this value in the new request. If LastEvaluatedStreamArn is empty, then the "last page" of results
         * has been processed and there is no more data to be retrieved. If LastEvaluatedStreamArn is not empty, it does not necessarily
         * mean that there is more data in the result set. The only way to know when you have reached the end of the result set is when
         * LastEvaluatedStreamArn is empty.
         */
        std::string lastEvaluatedStreamArn;

        /**
         * A list of stream descriptors associated with the current account and endpoint.
         */
        std::vector<Stream> streams;

      private:

        friend ListStreamsResponse tag_invoke(boost::json::value_to_tag<ListStreamsResponse>, boost::json::value const &v) {
            ListStreamsResponse r;
            r.lastEvaluatedStreamArn = Core::Json::GetStringValue(v, "LastEvaluatedStreamArn");
            if (Core::Json::AttributeExists(v, "Streams")) {
                r.streams = boost::json::value_to<std::vector<Stream>>(v.at("Streams"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListStreamsResponse const &obj) {
            jv = {
                    {"LastEvaluatedStreamArn", obj.lastEvaluatedStreamArn},
                    {"Streams", boost::json::value_from(obj.streams)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_RESPONSE_H
