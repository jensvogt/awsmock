//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Updates an existing user pool request
     *
     * Request to update an existing user pool.
     *
     * Example:
     * @code{.json}
     * {
     *   "AccountRecoverySetting": {
     *      "RecoveryMechanisms": [
     *         {
     *            "Name": "string",
     *            "Priority": number
     *         }
     *      ]
     *   },
     *   "AdminCreateUserConfig": {
     *      "AllowAdminCreateUserOnly": boolean,
     *      "InviteMessageTemplate": {
     *         "EmailMessage": "string",
     *         "EmailSubject": "string",
     *         "SMSMessage": "string"
     *      },
     *      "UnusedAccountValidityDays": number
     *   },
     *   "AutoVerifiedAttributes": [ "string" ],
     *   "DeletionProtection": "string",
     *   "DeviceConfiguration": {
     *      "ChallengeRequiredOnNewDevice": boolean,
     *      "DeviceOnlyRememberedOnUserPrompt": boolean
     *   },
     *   "EmailConfiguration": {
     *      "ConfigurationSet": "string",
     *      "EmailSendingAccount": "string",
     *      "From": "string",
     *      "ReplyToEmailAddress": "string",
     *      "SourceArn": "string"
     *   },
     *   "EmailVerificationMessage": "string",
     v   "EmailVerificationSubject": "string",
     *   "LambdaConfig": {
     *      "CreateAuthChallenge": "string",
     *      "CustomEmailSender": {
     *         "LambdaArn": "string",
     *         "LambdaVersion": "string"
     *      },
     *      "CustomMessage": "string",
     *      "CustomSMSSender": {
     *         "LambdaArn": "string",
     *         "LambdaVersion": "string"
     *      },
     *      "DefineAuthChallenge": "string",
     *      "KMSKeyID": "string",
     *      "PostAuthentication": "string",
     *      "PostConfirmation": "string",
     *      "PreAuthentication": "string",
     *      "PreSignUp": "string",
     *      "PreTokenGeneration": "string",
     *      "PreTokenGenerationConfig": {
     *         "LambdaArn": "string",
     *         "LambdaVersion": "string"
     *      },
     *      "UserMigration": "string",
     *      "VerifyAuthChallengeResponse": "string"
     *   },
     *   "MfaConfiguration": "string",
     *   "Policies": {
     *      "PasswordPolicy": {
     *         "MinimumLength": number,
     *         "RequireLowercase": boolean,
     *         "RequireNumbers": boolean,
     *         "RequireSymbols": boolean,
     *         "RequireUppercase": boolean,
     *         "TemporaryPasswordValidityDays": number
     *      }
     *   },
     *   "SmsAuthenticationMessage": "string",
     *   "SmsConfiguration": {
     *      "ExternalId": "string",
     *      "SnsCallerArn": "string",
     *      "SnsRegion": "string"
     *   },
     *   "SmsVerificationMessage": "string",
     *   "UserAttributeUpdateSettings": {
     *      "AttributesRequireVerificationBeforeUpdate": [ "string" ]
     *   },
     *   "UserPoolAddOns": {
     *      "AdvancedSecurityMode": "string"
     *   },
     *   "UserPoolId": "string",
     *   "UserPoolTags": {
     *      "string" : "string"
     *   },
     *   "VerificationMessageTemplate": {
     *      "DefaultEmailOption": "string",
     *      "EmailMessage": "string",
     *      "EmailMessageByLink": "string",
     *      "EmailSubject": "string",
     *      "EmailSubjectByLink": "string",
     *      "SmsMessage": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateUserPoolRequest final : Common::BaseCounter<UpdateUserPoolRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend UpdateUserPoolRequest tag_invoke(boost::json::value_to_tag<UpdateUserPoolRequest>, boost::json::value const &v) {
            UpdateUserPoolRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateUserPoolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_REQUEST_H
