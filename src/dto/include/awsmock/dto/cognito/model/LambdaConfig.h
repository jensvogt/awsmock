//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H
#define AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/CustomEmailSender.h>
#include <awsmock/dto/cognito/model/CustomSmsSender.h>
#include <awsmock/dto/cognito/model/PreTokenGenerationConfig.h>

namespace AwsMock::Dto::Cognito {

    /**
     * Cognito lambda configuration
     *
     * Example:
     * @code{.json}
     * "LambdaConfig": {
     *    "CreateAuthChallenge": "string",
     *    "CustomEmailSender": {
     *       "LambdaArn": "string",
     *       "LambdaVersion": "string"
     *    },
     *    "CustomMessage": "string",
     *    "CustomSMSSender": {
     *       "LambdaArn": "string",
     *       "LambdaVersion": "string"
     *    },
     *    "DefineAuthChallenge": "string",
     *    "KMSKeyID": "string",
     *    "PostAuthentication": "string",
     *    "PostConfirmation": "string",
     *    "PreAuthentication": "string",
     *    "PreSignUp": "string",
     *    "PreTokenGeneration": "string",
     *    "PreTokenGenerationConfig": {
     *       "LambdaArn": "string",
     *       "LambdaVersion": "string"
     *    },
     *    "UserMigration": "string",
     *    "VerifyAuthChallengeResponse": "string"
     * },
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaConfig {

        /**
         * Create authentication challenge
         */
        std::string createAuthChallenge;

        /**
         * Custom message
         */
        std::string customMessage;

        /**
         * Custom email sender
         */
        CustomEmailSender customEmailSender;

        /**
         * Custom SMS sender
         */
        CustomSmsSender customSmsSender;

        /**
         * Define authentication challenge
         */
        std::string defineAuthChallenge;

        /**
         * KMS key
         */
        std::string kmsKeyId;

        /**
         * Post authentication
         */
        std::string postAuthentication;

        /**
         * Post confirmation
         */
        std::string postConfirmation;

        /**
         * Pre confirmation
         */
        std::string preAuthentication;

        /**
         * Pre signup
         */
        std::string preSignUp;

        /**
         * Pre token generation
         */
        std::string preTokenGeneration;

        /**
         * Pre token generation config
         */
        PreTokenGenerationConfig preTokenGenerationConfig;

        /**
         * User migration
         */
        std::string userMigration;

        /**
         * Verify auth challenge
         */
        std::string verifyAuthChallengeResponse;

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
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const LambdaConfig &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H
