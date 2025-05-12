//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_SQS_NOTIFICATION_REQUEST_H
#define AWSMOCK_DTO_SNS_SQS_NOTIFICATION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>

namespace AwsMock::Dto::SNS {

    /**
     * SQS notification request, used to send a notification from an SNS topic to a SQS queue
     *
     * <p>
     * Example:
     * @code{.json}
     * {
     *   "Type" : "Notification",
     *   "MessageId" : "467f295c-b4fe-5e24-a627-74c9652562a8",
     *   "TopicArn" : "arn:aws:sns:eu-central-1:012096702153:pim-prod-protokollierung-topic",
     *   "Message" : "{\"protokollMeldung\":{\"verarbeitungsschritt\":\"KATALOGDATENDB\",\"dateiverarbeitungStatus\":\"ERFOLGREICH\",\"isKlaerfall\":false,\"isDatenlieferantFeedback\":false},\"anlieferungsContext\":{\"datenlieferantId\":\"DLI385\",\"anlieferungsdatum\":\"2023-10-02T18:15:02.9\",\"filePathOriginaldatei\":\"DLI385/ACdelta23267_02102023181502925.xml\",\"produktId\":\"082106559\"}}",
     *   "Timestamp" : "2023-10-03T08:23:55.183Z",
     *   "SignatureVersion" : "1",
     *   "Signature" : "OI5myFtSfUP8dOymRI0cW48FTAmb/VHYI2N10DwQ7X/fkHtJmG7hJNC4qjOkYUYC6kTATshpvo/VR5v0nnIHBFYgxrb9diLUMG2AsI6z2F02nGGWSTOjnH0XFpCGdyOB+2En6EZhzoUTOdFpGRXsijFDmXfxt2vS/RaGJLz/6Ibz+KeEYosgElQ2R1Nr9lz8jHCeaSdPzW7rTyIXv0b+cBDd8xHFc79IkSGmuepidAseTKkLX+oZ5Ry3Q1iTtTMOziXoz6kZhXXZHy/Q/clYCMzTP7BabDG5LK11gNfA0ytO/KDyiWxGykUommXnk6rt7FcCnNDW5/qrAmPHzbaO3w==",
     *   "SigningCertURL" : "https://sns.eu-central-1.amazonaws.com/SimpleNotificationService-01d088a6f77103d0fe307c0069e40ed6.pem",
     *   "UnsubscribeURL" : "https://sns.eu-central-1.amazonaws.com/?Action=Unsubscribe&SubscriptionArn=arn:aws:sns:eu-central-1:012096702153:pim-prod-protokollierung-topic:a278c03b-c5de-42a4-9c0e-f9c3412de940",
     *   "MessageAttributes": {
     *      "Type": {
     *      "Type": "String",
     *      "Value": "Orchestration.Services.Model.Pollution.PollutionMessage"
     *   }
     * }
     * @endcode
     */
    struct SqsNotificationRequest final : Common::BaseCounter<SqsNotificationRequest> {

        /**
         * Notification type
         */
        std::string type = "Notification";

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Source SNS topic ARN
         */
        std::string topicArn;

        /**
         * SQS message body
         */
        std::string message;

        /**
         * Send time stamp
         */
        long timestamp{};

        /**
         * Signature
         */
        std::string signature;

        /**
         * Signature version
         */
        std::string signatureVersion = "1";

        /**
         * Signing CERT URL
         */
        std::string signingCertURL;

        /**
         * Unsubscribe URL
         */
        std::string unsubscribeURL;

        /**
         * Attributes
         */
        std::map<std::string, MessageAttribute> messageAttributes;

      private:

        friend SqsNotificationRequest tag_invoke(boost::json::value_to_tag<SqsNotificationRequest>, boost::json::value const &v) {
            SqsNotificationRequest r;
            r.type = Core::Json::GetStringValue(v, "Type");
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.message = Core::Json::GetStringValue(v, "Message");
            r.timestamp = Core::Json::GetLongValue(v, "Timestamp");
            r.signature = Core::Json::GetStringValue(v, "Signature");
            r.signatureVersion = Core::Json::GetStringValue(v, "SignatureVersion");
            r.signingCertURL = Core::Json::GetStringValue(v, "SigningCertURL");
            r.unsubscribeURL = Core::Json::GetStringValue(v, "UnsubscribeURL");
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SqsNotificationRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Type", obj.type},
                    {"MessageId", obj.messageId},
                    {"Message", obj.message},
                    {"Timestamp", obj.timestamp},
                    {"Signature", obj.signature},
                    {"SignatureVersion", obj.signatureVersion},
                    {"SigningCertURL", obj.signingCertURL},
                    {"UnsubscribeURL", obj.unsubscribeURL},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_SQS_NOTIFICATION_REQUEST_H
