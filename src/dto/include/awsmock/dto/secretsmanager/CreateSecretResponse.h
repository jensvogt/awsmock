//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_RESPONSE_H

// C++ standard includes
#include <iostream>
#include <sstream>
#include <string>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/JsonUtils.h>

namespace AwsMock::Dto::SecretsManager {

    struct CreateSecretResponse {

        /**
         * Region
         */
        std::string region;

        /**
         * Name
         */
        std::string name;

        /**
         * Secret ARN
         */
        std::string arn;

        /**
         * VersionId
         */
        std::string versionId;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const CreateSecretResponse &r);
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_RESPONSE_H
