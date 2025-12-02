//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/UserPool.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user pool response
     *
     * Example:
     * @code{.json}
     * {
     *   "UserPool": {
     *     "AccountRecoverySetting": {
     *       "RecoveryMechanisms": [
     *         {
     *           "Name": "string",
     *           "Priority": number
     *         }
     *       ]
     *    },
     *    "AdminCreateUserConfig": {
     *      "AllowAdminCreateUserOnly": boolean,
     *      "InviteMessageTemplate": {
     *        "EmailMessage": "string",
     *        "EmailSubject": "string",
     *        "SMSMessage": "string"
     *      },
     *      "UnusedAccountValidityDays": number
     *    },
     *    "AliasAttributes": [ "string" ],
     *    "Arn": "string",
     *    "AutoVerifiedAttributes": [ "string" ],
     *    "CreationDate": number,
     *    "CustomDomain": "string",
     *    "DeletionProtection": "string",
     *    "DeviceConfiguration": {
     *      "ChallengeRequiredOnNewDevice": boolean,
     *      "DeviceOnlyRememberedOnUserPrompt": boolean
     *   },
     *   "Domain": "string",
     *   "EmailConfiguration": {
     *      "ConfigurationSet": "string",
     *      "EmailSendingAccount": "string",
     *      "From": "string",
     *      "ReplyToEmailAddress": "string",
     *      "SourceArn": "string"
     *     },
     *     "EmailConfigurationFailure": "string",
     *      "EmailVerificationMessage": "string",
     *      "EmailVerificationSubject": "string",
     *      "EstimatedNumberOfUsers": number,
     *      "Id": "string",
     *      "LambdaConfig": {
     *        "CreateAuthChallenge": "string",
     *          "CustomEmailSender": {
     *          "LambdaArn": "string",
     *          "LambdaVersion": "string"
     *        },
     *        "CustomMessage": "string",
     *        "CustomSMSSender": {
     *           "LambdaArn": "string",
     *           "LambdaVersion": "string"
     *        },
     *         "DefineAuthChallenge": "string",
     *         "KMSKeyID": "string",
     *         "PostAuthentication": "string",
     *         "PostConfirmation": "string",
     *         "PreAuthentication": "string",
     *         "PreSignUp": "string",
     *         "PreTokenGeneration": "string",
     *         "PreTokenGenerationConfig": {
     *           "LambdaArn": "string",
     *           "LambdaVersion": "string"
     *          },
     *         "UserMigration": "string",
     *         "VerifyAuthChallengeResponse": "string"
     *        },
     *        "LastModifiedDate": number,
     *        "MfaConfiguration": "string",
     *        "Name": "string",
     *        "Policies": {
     *        "PasswordPolicy": {
     *          "MinimumLength": number,
     *          "RequireLowercase": boolean,
     *          "RequireNumbers": boolean,
     *          "RequireSymbols": boolean,
     *          "RequireUppercase": boolean,
     *          "TemporaryPasswordValidityDays": number
     *        }
     *      },
     *      "SchemaAttributes": [
     *        {
     *           "AttributeDataType": "string",
     *           "DeveloperOnlyAttribute": boolean,
     *           "Mutable": boolean,
     *           "Name": "string",
     *           "NumberAttributeConstraints": {
     *              "MaxValue": "string",
     *              "MinValue": "string"
     *           },
     *           "Required": boolean,
     *           "StringAttributeConstraints": {
     *              "MaxLength": "string",
     *              "MinLength": "string"
     *           }
     *         }
     *       ],
     *       "SmsAuthenticationMessage": "string",
     *       "SmsConfiguration": {
     *         "ExternalId": "string",
     *         "SnsCallerArn": "string",
     *         "SnsRegion": "string"
     *       },
     *       "SmsConfigurationFailure": "string",
     *       "SmsVerificationMessage": "string",
     *       "Status": "string",
     *       "UserAttributeUpdateSettings": {
     *         "AttributesRequireVerificationBeforeUpdate": [ "string" ]
     *       },
     *       "UsernameAttributes": [ "string" ],
     *       "UsernameConfiguration": {
     *         "CaseSensitive": boolean
     *       },
     *       "UserPoolAddOns": {
     *         "AdvancedSecurityMode": "string"
     *       },
     *       "UserPoolTags": {
     *         "string" : "string"
     *       },
     *       "VerificationMessageTemplate": {
     *       "DefaultEmailOption": "string",
     *       "EmailMessage": "string",
     *       "EmailMessageByLink": "string",
     *       "EmailSubject": "string",
     *       "EmailSubjectByLink": "string",
     *       "SmsMessage": "string"
     *     }
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeUserPoolResponse final : Common::BaseCounter<DescribeUserPoolResponse> {

        /**
         * User pool
         */
        UserPool userPool;

      private:

        friend DescribeUserPoolResponse tag_invoke(boost::json::value_to_tag<DescribeUserPoolResponse>, boost::json::value const &v) {
            DescribeUserPoolResponse r;
            r.userPool = boost::json::value_to<UserPool>(v.at("UserPool"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPool", boost::json::value_from(obj.userPool)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_RESPONSE_H
