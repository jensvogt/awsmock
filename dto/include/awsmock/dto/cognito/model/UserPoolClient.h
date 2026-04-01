//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H
#define AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/AuthFlow.h>
#include <awsmock/dto/cognito/model/TokenValidityUnits.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user pool client
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolClient final : Common::BaseCounter<UserPoolClient> {

        /**
         * ID
         */
        std::string id;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Client ID
         */
        std::string clientId;

        /**
         * Client name
         */
        std::string clientName;

        /**
         * Client secret
         */
        std::string clientSecret;

        /**
         * Access token validity
         */
        long accessTokenValidity{};

        /**
         * ID token validity
         */
        long idTokenValidity{};

        /**
         * Refresh token validity
         */
        long refreshTokenValidity{};

        /**
         * Token validity units
         */
        TokenValidityUnits tokenValidityUnits;

        /**
         * @brief Auth flows
         */
        std::vector<AuthFlowType> explicitAuthFlows = {AuthFlowType::USER_PASSWORD_AUTH, AuthFlowType::ADMIN_USER_PASSWORD_AUTH, AuthFlowType::USER_SRP_AUTH};

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

      private:

        friend UserPoolClient tag_invoke(boost::json::value_to_tag<UserPoolClient>, boost::json::value const &v) {
            UserPoolClient r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            r.clientName = Core::Json::GetStringValue(v, "ClientName");
            r.clientSecret = Core::Json::GetStringValue(v, "ClientSecret");
            r.accessTokenValidity = Core::Json::GetLongValue(v, "AccessTokenValidity");
            r.idTokenValidity = Core::Json::GetLongValue(v, "IdTokenValidity");
            r.refreshTokenValidity = Core::Json::GetLongValue(v, "RefreshTokenValidity");
            r.tokenValidityUnits = boost::json::value_to<TokenValidityUnits>(v.at("TokenValidityUnits"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            if (Core::Json::AttributeExists(v, "ExplicitAuthFlows")) {
                for (const auto &a: v.at("ExplicitAuthFlows").as_array()) {
                    if (!std::ranges::contains(r.explicitAuthFlows, AuthFlowTypeFromString(a.as_string().data()))) {
                        r.explicitAuthFlows.emplace_back(AuthFlowTypeFromString(a.as_string().data()));
                    }
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserPoolClient const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Id", obj.id},
                    {"UserPoolId", obj.userPoolId},
                    {"ClientId", obj.clientId},
                    {"ClientName", obj.clientName},
                    {"ClientSecret", obj.clientSecret},
                    {"AccessTokenValidity", obj.accessTokenValidity},
                    {"IdTokenValidity", obj.idTokenValidity},
                    {"RefreshTokenValidity", obj.refreshTokenValidity},
                    {"TokenValidityUnits", boost::json::value_from(obj.tokenValidityUnits)},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
            if (!obj.explicitAuthFlows.empty()) {
                boost::json::array authFlows;
                for (const auto &a: obj.explicitAuthFlows) {
                    authFlows.emplace_back(AuthFlowTypeToString(a));
                }
                jv.as_object()["ExplicitAuthFlows"] = authFlows;
            }
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H
