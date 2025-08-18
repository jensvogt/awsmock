//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user pool request
     *
     * Request to create a new user pool.
     *
     * Example:
     * @code{.json}
     * {
     *   "AccountRecoverySetting": {
     *     "RecoveryMechanisms": [
     *        {
     *           "Name": "string",
     *           "Priority": number
     *        }
     *     ]
     *   },
     *   "AdminCreateUserConfig": {
     *     "AllowAdminCreateUserOnly": boolean,
     *     "InviteMessageTemplate": {
     *        "EmailMessage": "string",
     *        "EmailSubject": "string",
     *        "SMSMessage": "string"
     *     },
     *     "UnusedAccountValidityDays": number
     *  },
     *  "AliasAttributes": [ "string" ],
     *  "AutoVerifiedAttributes": [ "string" ],
     *  "DeletionProtection": "string",
     *  "DeviceConfiguration": {
     *     "ChallengeRequiredOnNewDevice": boolean,
     *     "DeviceOnlyRememberedOnUserPrompt": boolean
     *   },
     *   "EmailConfiguration": {
     *      "ConfigurationSet": "string",
     *      "EmailSendingAccount": "string",
     *      "From": "string",
     *      "ReplyToEmailAddress": "string",
     *      "SourceArn": "string"
     *   },
     *   "EmailVerificationMessage": "string",
     *   "EmailVerificationSubject": "string",
     *   "LambdaConfig": {
     *      "CreateAuthChallenge": "string",
     *      "CustomEmailSender": {
     *        "LambdaArn": "string",
     *        "LambdaVersion": "string"
     *      },
     *      "CustomMessage": "string",
     *      "CustomSMSSender": {
     *      "LambdaArn": "string",
     *        "LambdaVersion": "string"
     *      },
     *      "DefineAuthChallenge": "string",
     *      "KMSKeyID": "string",
     *      "PostAuthentication": "string",
     *        "PostConfirmation": "string",
     *        "PreAuthentication": "string",
     *        "PreSignUp": "string",
     *        "PreTokenGeneration": "string",
     *        "PreTokenGenerationConfig": {
     *          "LambdaArn": "string",
     *          "LambdaVersion": "string"
     *        },
     *        "UserMigration": "string",
     *        "VerifyAuthChallengeResponse": "string"
     *      },
     *       "MfaConfiguration": "string",
     *       "Policies": {
     *          "PasswordPolicy": {
     *             "MinimumLength": number,
     *             "RequireLowercase": boolean,
     *             "RequireNumbers": boolean,
     *             "RequireSymbols": boolean,
     *             "RequireUppercase": boolean,
     *             "TemporaryPasswordValidityDays": number
     *          }
     *       },
     *       "PoolName": "string",
     *       "Schema": [
     *          {
     *             "AttributeDataType": "string",
     *             "DeveloperOnlyAttribute": boolean,
     *             "Mutable": boolean,
     *             "Name": "string",
     *             "NumberAttributeConstraints": {
     *                "MaxValue": "string",
     *                "MinValue": "string"
     *             },
     *             "Required": boolean,
     *             "StringAttributeConstraints": {
     *                "MaxLength": "string",
     *                "MinLength": "string"
     *             }
     *          }
     *       ],
     *       "SmsAuthenticationMessage": "string",
     *       "SmsConfiguration": {
     *          "ExternalId": "string",
     *          "SnsCallerArn": "string",
     *          "SnsRegion": "string"
     *       },
     *       "SmsVerificationMessage": "string",
     *       "UserAttributeUpdateSettings": {
     *       "AttributesRequireVerificationBeforeUpdate": [ "string" ]
     *    },
     *    "UsernameAttributes": [ "string" ],
     *    "UsernameConfiguration": {
     *       "CaseSensitive": boolean
     *    },
     *    "UserPoolAddOns": {
     *       "AdvancedSecurityMode": "string"
     *    },
     *    "UserPoolTags": {
     *      "string" : "string"
     *    },
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
    struct CreateUserPoolRequest final : Common::BaseCounter<CreateUserPoolRequest> {

        /**
         * Name of the user pool
         */
        std::string name;

        /**
         * Domain of the user pool
         */
        std::string domain;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateUserPoolRequest tag_invoke(boost::json::value_to_tag<CreateUserPoolRequest>, boost::json::value const &v) {
            CreateUserPoolRequest r;
            r.name = Core::Json::GetStringValue(v, "PoolName");
            r.domain = Core::Json::GetStringValue(v, "Domain");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserPoolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"PoolName", obj.name},
                    {"Domain", obj.domain},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_REQUEST_H
