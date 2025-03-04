//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H
#define AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/DeadLetterConfig.h>
#include <awsmock/dto/lambda/model/Environment.h>

namespace AwsMock::Dto::Lambda {

    using std::chrono::system_clock;

    struct FunctionCounter {

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
         * Name of the base64 ZIP file
         */
        std::string zipFile = {};

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
         * Timeout in minutes
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
         * Invocations
         */
        long invocations = 0;

        /**
         * Average runtime in milliseconds
         */
        long averageRuntime = 0;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * @brief Converts the DTO to a BSON document
         *
         * @return DTO as BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const FunctionCounter &f);
    };

    typedef std::vector<FunctionCounter> FunctionCounterList;

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_FUNCTION_COUNTER_H
