//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// Poco includes
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/XML/XMLWriter.h>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::SQS {

    struct PurgeQueueRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Resource
         */
        std::string resource = "Unknown resource";

        /**
         * Resource
         */
        std::string requestId = Poco::UUIDGenerator().createRandom().toString();

        /**
         * Converts the JSON string to a DTO
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const PurgeQueueRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H
