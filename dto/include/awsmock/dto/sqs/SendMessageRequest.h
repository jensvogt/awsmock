//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"


#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * Send message request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "QueueUrl":"http://localhost:4566/000000000000/artikel-input",
     *   "MessageBody":"{
     *     "id":"94169598-3b07-4913-aa09-7bab3babd18d",
     *     "ursprungsDatei":"SourceFile",
     *     "ursprungsFormat":"ONIX 3.0",
     *     "standDatum":"2024-05-13T18:19:09.340807234",
     *     "anlieferungsDatum":"2024-05-13T18:19:09.340820701",
     *     "lieferantenId":"DLI-0",
     *     "l2000Datenquelle":42,
     *     "produktId":"04ca0fe2-c999-4c7c-bacd-d40cb1b98f90",
     *     "status":"aktiv",
     *     "artikelNummern":[
     *       {
     *         "typ":"EAN",
     *         "nummer":"2380123456785"
     *       },
     *       {
     *         "typ":"Librinummer","nummer":"1234569"
     *       },
     *       {
     *         "typ":"Proprietaer","nummer":"2380123456789-1"
     *       }
     *     ],
     *     "produktFormAngaben":{
     *       "produktForm":"AB",
     *       "produktFormDetailliert":["A101"]
     *     },
     *     "barcode":"barcodiert",
     *     "orgaNummer":"3332",
     *     "lieferantenName":"Springer Verlag"
     *   },
     *   MessageAttributes":{
     *     "retryContext":{
     *       "StringValue":"[
     *         {
     *           "verarbeitungsschritt":"PARSEN",
     *           "internalId":"qwhz3etz",
     *           "queueName":"produktmeldung-retry-queue"
     *         },
     *         {
     *           "verarbeitungsschritt":"ZERLEGEN",
     *           "internalId":"pKR4aWII",
     *           "queueName":"originalmeldung-retry-queue"
     *         }
     *       ]",
     *       "DataType":"String"
     *     },
     *     "contentType":{
     *       "StringValue":"application/json",
     *       "DataType":"String"
     *     }
     *   },
     *   "MessageSystemAttributes":{}
     * }
     * @endcode
     */
    struct SendMessageRequest final : Common::BaseCounter<SendMessageRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Message body
         */
        std::string body;

        /**
         * Delay seconds
         */
        long delaySeconds{};

        /**
         * Content type
         */
        std::string contentType{};

        /**
         * Attributes (system attributes)
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes (user attributes)
         */
        std::map<std::string, MessageAttribute> messageAttributes;

      private:

        friend SendMessageRequest tag_invoke(boost::json::value_to_tag<SendMessageRequest>, boost::json::value const &v) {
            SendMessageRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.body = Core::Json::GetStringValue(v, "MessageBody");
            r.contentType = Core::Json::GetStringValue(v, "ContentType");
            r.delaySeconds = Core::Json::GetLongValue(v, "DelaySeconds");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("Attributes"));
            }
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SendMessageRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"MessageBody", obj.body},
                    {"ContentType", obj.contentType},
                    {"DelaySeconds", obj.delaySeconds},
                    {"Attributes", boost::json::value_from(obj.attributes)},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H
