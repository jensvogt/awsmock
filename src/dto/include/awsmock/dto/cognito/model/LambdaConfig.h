//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H
#define AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/CustomEmailSender.h>
#include <awsmock/dto/cognito/model/CustomSmsSender.h>
#include <awsmock/dto/cognito/model/PreTokenGenerationConfig.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Cognito lambda configuration
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
    struct LambdaConfig final : Common::BaseCounter<LambdaConfig> {

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

      private:

        friend LambdaConfig tag_invoke(boost::json::value_to_tag<LambdaConfig>, boost::json::value const &v) {
            LambdaConfig r;
            r.createAuthChallenge = Core::Json::GetStringValue(v, "CreateAuthChallenge");
            r.customMessage = Core::Json::GetStringValue(v, "CustomMessage");
            r.customEmailSender = boost::json::value_to<CustomEmailSender>(v.at("CustomMessage"));
            r.customSmsSender = boost::json::value_to<CustomSmsSender>(v.at("CustomSmsSender"));
            r.defineAuthChallenge = Core::Json::GetStringValue(v, "DefineAuthChallenge");
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            r.postAuthentication = Core::Json::GetStringValue(v, "PostAuthentication");
            r.postConfirmation = Core::Json::GetStringValue(v, "PostConfirmation");
            r.preAuthentication = Core::Json::GetStringValue(v, "PreAuthentication");
            r.preSignUp = Core::Json::GetStringValue(v, "PreSignUp");
            r.preTokenGeneration = Core::Json::GetStringValue(v, "PreTokenGeneration");
            r.preTokenGenerationConfig = boost::json::value_to<PreTokenGenerationConfig>(v.at("PreTokenGenerationConfig"));
            r.userMigration = Core::Json::GetStringValue(v, "UserMigration");
            r.verifyAuthChallengeResponse = Core::Json::GetStringValue(v, "VerifyAuthChallengeResponse");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaConfig const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"CreateAuthChallenge", obj.createAuthChallenge},
                    {"CustomMessage", obj.customMessage},
                    {"CustomEmailSender", boost::json::value_from(obj.customEmailSender)},
                    {"CustomSmsSender", boost::json::value_from(obj.customSmsSender)},
                    {"DefineAuthChallenge", obj.defineAuthChallenge},
                    {"KmsKeyId", obj.kmsKeyId},
                    {"PostAuthentication", obj.postAuthentication},
                    {"PostConfirmation", obj.postConfirmation},
                    {"PreAuthentication", obj.preAuthentication},
                    {"PreSignUp", obj.preSignUp},
                    {"PreTokenGeneration", obj.preTokenGeneration},
                    {"PreTokenGenerationConfig", boost::json::value_from(obj.preTokenGenerationConfig)},
                    {"UserMigration", obj.userMigration},
                    {"VerifyAuthChallengeResponse", obj.verifyAuthChallengeResponse},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LAMBDACONFIG_H
