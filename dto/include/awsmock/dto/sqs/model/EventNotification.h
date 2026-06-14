//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/regex/v5/regex.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/sqs/model/EventRecord.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief SQS event notification
     *
     * @code(.json)
     * {
     *   "Records": [
     *     {
     *       "messageId": "059f36b4-87a3-44ab-83d2-661975830a7d",
     *       "receiptHandle": "AQEBwJnKyrHigUMZj6rYigCgxlaS3SLy0a...",
     *       "body": "Test message.",
     *       "attributes": {
     *         "ApproximateReceiveCount": "1",
     *         "SentTimestamp": "1545082649183",
     *         "SenderId": "AIDAIENQZJOLO23YVJ4VO",
     *         "ApproximateFirstReceiveTimestamp": "1545082649185"
     *       },
     *       "messageAttributes": {},
     *       "md5OfBody": "e4e68fb7bd0e697a0ae8f1bb342846b3",
     *       "eventSource": "aws:sqs",
     *       "eventSourceARN": "arn:aws:sqs:us-east-2:123456789012:my-queue",
     *       "awsRegion": "us-east-2"
     *     }
     *   ]
     * }
     * @endcode
     */
    struct EventNotification final : Common::BaseObject<EventNotification> {

        /**
         * S3 event record
         */
        std::vector<EventRecord> records;

      private:

        friend EventNotification tag_invoke(boost::json::value_to_tag<EventNotification>, boost::json::value const &v) {
            EventNotification r;
            if (Core::Json::AttributeExists(v, "Records")) {
                r.records = boost::json::value_to<std::vector<EventRecord>>(v.at("Records"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventNotification const &obj) {
            jv = {
                    {"Records", boost::json::value_from(obj.records)},
            };
        }
    };

}// namespace Awsmock::Dto::SQS
