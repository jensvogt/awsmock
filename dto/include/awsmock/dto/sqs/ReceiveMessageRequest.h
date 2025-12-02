//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

#define DEFAULT_WAIT_TIME 10
#define DEFAULT_MAX_MESSAGES 10
#define DEFAULT_VISIBILITY_TIMEOUT 40
#define DEFAULT_ATTRIBUTE_NAMES "All"
#define DEFAULT_MESSAGE_ATTRIBUTE_NAMES "All"
#define DEFAULT_MESSAGE_SYSTEM_ATTRIBUTE_NAMES "All"

namespace AwsMock::Dto::SQS {

    /**
     * @brief Receive message request.
     *
     * Example:
     * @code{.json}
     * {
     *   "AttributeNames": [ "string" ],
     *   "MaxNumberOfMessages": number,
     *   "MessageAttributeNames": [ "string" ],
     *   "MessageSystemAttributeNames": [ "string" ],
     *   "QueueUrl": "string",
     *   "ReceiveRequestAttemptId": "string",
     *   "VisibilityTimeout": number,
     *   "WaitTimeSeconds": number
     * }
     * @endcode
     *
     * @par
     * The wait time seconds parameter is the duration (in seconds) for which the call waits for a message to arrive in the queue before returning. If a message is available, the call returns sooner
     * than WaitTimeSeconds. If no messages are available and the wait time expires, the call does not return a message list. If you are using the Java SDK, it returns a ReceiveMessageResponse object,
     * which has an empty list instead of a Null object.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ReceiveMessageRequest final : Common::BaseCounter<ReceiveMessageRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Maximal number of resources
         */
        long maxMessages = DEFAULT_MAX_MESSAGES;

        /**
         * Visibility
         */
        long visibilityTimeout = DEFAULT_VISIBILITY_TIMEOUT;

        /**
         * Wait time in seconds
         */
        long waitTimeSeconds = DEFAULT_WAIT_TIME;

        /**
         * List of attribute names
         */
        std::vector<std::string> attributeNames = {DEFAULT_ATTRIBUTE_NAMES};

        /**
         * List of system attribute names
         */
        std::vector<std::string> systemAttributeNames = {DEFAULT_MESSAGE_SYSTEM_ATTRIBUTE_NAMES};

        /**
         * List of message attribute names
         */
        std::vector<std::string> messageAttributeNames = {DEFAULT_MESSAGE_ATTRIBUTE_NAMES};

      private:

        friend ReceiveMessageRequest tag_invoke(boost::json::value_to_tag<ReceiveMessageRequest>, boost::json::value const &v) {
            ReceiveMessageRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.maxMessages = Core::Json::GetLongValue(v, "MaxNumberOfMessages", DEFAULT_MAX_MESSAGES);
            r.visibilityTimeout = Core::Json::GetLongValue(v, "VisibilityTimeout", DEFAULT_VISIBILITY_TIMEOUT);
            r.waitTimeSeconds = Core::Json::GetLongValue(v, "WaitTimeSeconds", DEFAULT_WAIT_TIME);
            r.attributeNames = Core::Json::GetStringArrayValue(v, "AttributeNames", DEFAULT_ATTRIBUTE_NAMES);
            r.systemAttributeNames = Core::Json::GetStringArrayValue(v, "MessageSystemAttributeNames", DEFAULT_MESSAGE_SYSTEM_ATTRIBUTE_NAMES);
            r.messageAttributeNames = Core::Json::GetStringArrayValue(v, "MessageAttributeNames", DEFAULT_MESSAGE_ATTRIBUTE_NAMES);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ReceiveMessageRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"maxMessages", obj.maxMessages},
                    {"visibilityTimeout", obj.visibilityTimeout},
                    {"waitTimeSeconds", obj.waitTimeSeconds},
                    {"attributeNames", boost::json::value_from(obj.attributeNames)},
                    {"messageAttributeNames", boost::json::value_from(obj.messageAttributeNames)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
