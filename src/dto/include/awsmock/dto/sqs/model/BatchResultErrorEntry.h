//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H
#define AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H

// C++ includes
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Send when an delete batch error occurs.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BatchResultErrorEntry {

        /**
         * Id
         */
        std::string id;

        /**
         * Code
         */
        std::string code;

        /**
         * Sender fault
         */
        bool senderFault;

        /**
         * Error message
         */
        std::string message;

        /**
         * @brief Converts the DTO to a JSON string.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param object JSON object.
         */
        void FromJson(const Poco::JSON::Object::Ptr &object);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const BatchResultErrorEntry &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H
