//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
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
    struct SendMessageRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Message body
         */
        std::string body;

        /**
         * Delay seconds
         */
        int delaySeconds;

        /**
         * Attributes (system attributes)
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes (user attributes)
         */
        MessageAttributeList messageAttributes;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Sender ID
         */
        std::string senderId;

        /**
         * MD5 of request body
         */
        std::string md5sum;

        /**
         * Content type (can only be XML, JSON, TXT)
         */
        std::string contentType;

        /**
         * Request ID
         */
        std::string requestId;

        /**
         * @brief Converts the JSON string to a DTO
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString();

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, SendMessageRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_REQUEST_H
