//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_INVOCATION_TYPE_H
#define AWSMOCK_DB_ENTITY_LAMBDA_INVOCATION_TYPE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Lambda invocation type
     *
     * @par
     *  - REQUEST_RESPONSE (default) – Invoke the function synchronously. Keep the connection open until the function returns a response or times out. The API response
     * includes the function response and additional data.
     * - EVENT Invoke the function asynchronously. Send events that fail multiple times to the function's dead-letter queue (if one is configured). The API response
     * only includes a status code.
     * - DRY_RUN Validate parameter values and verify that the user or role has permission to invoke the function.
     * @author jens.vogt\@opitz-consulting.com
     */
    enum LambdaInvocationType {
        REQUEST_RESPONSE,
        EVENT,
        DRY_RUN,
        UNKNOWN
    };

    static std::map<LambdaInvocationType, std::string> LambdaInvocationTypeNames{
            {REQUEST_RESPONSE, "RequestResponse"},
            {EVENT, "Event"},
            {DRY_RUN, "DryRun"},
            {UNKNOWN, "Unknown"},
    };

    [[maybe_unused]] static std::string LambdaInvocationTypeToString(const LambdaInvocationType &lambdaInvocationType) {
        return LambdaInvocationTypeNames[lambdaInvocationType];
    }

    [[maybe_unused]] static LambdaInvocationType LambdaInvocationTypeFromString(const std::string &lambdaInvocationType) {
        for (auto &[fst, snd]: LambdaInvocationTypeNames) {
            if (snd == lambdaInvocationType) {
                return fst;
            }
        }
        return REQUEST_RESPONSE;
    }

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DB_ENTITY_LAMBDA_INVOCATION_TYPE_H
