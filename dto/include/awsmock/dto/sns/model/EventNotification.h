//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_SNS_EVENT_NOTIFICATION_H
#define AWSMOCK_DTO_SNS_EVENT_NOTIFICATION_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/sns/model/EventRecord.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief SNS event notification
     *
     * @code(.json)
     * {
     * "Records": [
     *   {
     *     "EventVersion": "1.0",
     *     "EventSubscriptionArn": "arn:aws:sns:us-east-1:123456789012:sns-lambda:21be56ed-a058-49f5-8c98-aedd2564c486",
     *     "EventSource": "aws:sns",
     *     "Sns": {
     *        "SignatureVersion": "1",
     *        "Timestamp": "2019-01-02T12:45:07.000Z",
     *        "Signature": "tcc6faL2yUC6dgZdmrwh1Y4cGa/ebXEkAi6RibDsvpi+tE/1+82j...65r==",
     *        "SigningCertURL": "https://sns.us-east-1.amazonaws.com/SimpleNotificationService-ac565b8b1a6c5d002d285f9598aa1d9b.pem",
     *        "MessageId": "95df01b4-ee98-5cb9-9903-4c221d41eb5e",
     *        "Message": "Hello from SNS!",
     *        "MessageAttributes": {
     *          "Test": {
     *          "Type": "String",
     *          "Value": "TestString"
     *        },
     *        "TestBinary": {
     *          "Type": "Binary",
     *          "Value": "TestBinary"
     *        }
     *      },
     *      "Type": "Notification",
     *      "UnsubscribeUrl": "https://sns.us-east-1.amazonaws.com/?Action=Unsubscribe&amp;SubscriptionArn=arn:aws:sns:us-east-1:123456789012:test-lambda:21be56ed-a058-49f5-8c98-aedd2564c486",
     *      "TopicArn":"arn:aws:sns:us-east-1:123456789012:sns-lambda",
     *      "Subject": "TestInvoke"
     *     }
     *   }
     *   ]
     * }
     * @endcode
     */
    struct EventNotification final : Common::BaseCounter<EventNotification> {

        /**
         * S3 event record
         */
        std::vector<Record> records;

      private:

        friend EventNotification tag_invoke(boost::json::value_to_tag<EventNotification>, boost::json::value const &v) {
            EventNotification r;
            if (Core::Json::AttributeExists(v, "Records")) {
                r.records = boost::json::value_to<std::vector<Record>>(v.at("Records"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventNotification const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"records", boost::json::value_from(obj.records)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_EVENT_NOTIFICATION_H
