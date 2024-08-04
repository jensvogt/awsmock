//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_FUNCTION_H
#define AWSMOCK_DTO_LAMBDA_FUNCTION_H

// C++ standard includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/lambda/model/DeadLetterConfig.h>
#include <awsmock/dto/lambda/model/Environment.h>

namespace AwsMock::Dto::Lambda {

    using std::chrono::system_clock;

    struct Function {

        /**
         * AWS region
         */
        std::string region = {};

        /**
         * Architectures
         */
        std::vector<std::string> architectures = {};

        /**
         * SHA256 checksum
         */
        std::string codeSha256 = {};

        /**
         * Code size
         */
        long codeSize = 0;

        /**
         * Dead letter config
         */
        DeadLetterConfig deadLetterConfig = {};

        /**
         * Description
         */
        std::string description = {};
        // TODO: Environment, EpheremalStorage, FileSystemConfig

        /**
         * Function ARN
         */
        std::string functionArn = {};

        /**
         * Function name
         */
        std::string functionName = {};

        /**
         * Function handler
         */
        std::string handler = {};

        /**
         * Function runtime
         */
        std::string runtime = {};

        // TODO: ImageConfigResponse

        /**
         * Last modification datetime
         */
        system_clock::time_point lastModified = {};

        /**
         * Last update state
         */
        std::string lastUpdateStatus = {};

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReason = {};

        /**
         * Last update state reason
         */
        std::string lastUpdateStatusReasonCode = {};

        // TODO: Layers

        /**
         * State
         */
        std::string state = {};

        /**
         * State reason
         */
        std::string stateReason = {};

        /**
         * State reason code
         */
        std::string stateReasonCode = {};

        /**
         * Timeout
         */
        int timeout = 15;

        /**
         * Version
         */
        std::string version = {};

        /**
         * Environment
         */
        EnvironmentVariables environment = {};

        /**
         * Converts the DTO to a JSON representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * Converts the DTO to a JSON string.
         *
         * @return DTO as JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const Function &f);
    };

    typedef std::vector<Function> FunctionList;

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_FUNCTION_H
