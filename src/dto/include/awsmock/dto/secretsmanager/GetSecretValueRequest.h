//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMoc includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/JsonUtils.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * Get a secret value
     *
     * Example:
     * @code{.json}
     * {
     *    "SecretId": "string",
     *    "VersionId": "string",
     *    "VersionStage": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetSecretValueRequest {

        /**
     * Region
     */
        std::string region;

        /**
     * Secret ID
     */
        std::string secretId;

        /**
     * Version ID
     */
        std::string versionId;

        /**
     * Version ID
     */
        std::string versionStage;

        /**
     * REQUEST ID
     */
        std::string requestId;

        /**
     * Converts the DTO to a JSON representation.
     *
     * @return DTO as string for logging.
     */
        [[nodiscard]] std::string ToJson() const;

        /**
     * Converts the JSON string to DTO.
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
        friend std::ostream &operator<<(std::ostream &os, const GetSecretValueRequest &r);
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H
