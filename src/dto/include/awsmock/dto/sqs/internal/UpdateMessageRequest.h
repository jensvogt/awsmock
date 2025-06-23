//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UPDATE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_UPDATE_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Update message request
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
    struct UpdateMessageRequest final : Common::BaseCounter<UpdateMessageRequest> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Message attributes (user attributes)
         */
        std::map<std::string, MessageAttribute> messageAttributes;

      private:

        friend UpdateMessageRequest tag_invoke(boost::json::value_to_tag<UpdateMessageRequest>, boost::json::value const &v) {
            UpdateMessageRequest r;
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateMessageRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MessageId", obj.messageId},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UPDATE_MESSAGE_REQUEST_H
