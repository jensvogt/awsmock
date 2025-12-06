//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H
#define AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H

// C++ includes
#include <string>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/SftpAuthenticationMethod.h>

namespace AwsMock::Dto::Transfer {

    struct IdentityProviderDetails final : Common::BaseCounter<IdentityProviderDetails> {

        /**
         * Directory ID
         */
        std::string directoryId;

        /**
         * Lambda function
         */
        std::string function;

        /**
         * IAM Invocation role
         */
        std::string invocationRole;

        /**
         * Identity provider type
         */
        SftpAuthenticationMethod sftpAuthenticationMethod = SftpAuthenticationMethod::UNKNOWN;

        /**
         * Url
         */
        std::string url;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the entity to a DTO representation.
         *
         * @param document BSON document
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {
                directoryId = Core::Bson::BsonUtils::GetStringValue(document, "DirectoryId");
                function = Core::Bson::BsonUtils::GetStringValue(document, "Function");
                url = Core::Bson::BsonUtils::GetStringValue(document, "Url");
                invocationRole = Core::Bson::BsonUtils::GetStringValue(document, "InvocationRole");
                sftpAuthenticationMethod = SftpAuthenticationMethodFromString(Core::Bson::BsonUtils::GetStringValue(document, "SftpAuthenticationMethod"));

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend IdentityProviderDetails tag_invoke(boost::json::value_to_tag<IdentityProviderDetails>, boost::json::value const &v) {
            IdentityProviderDetails r;
            r.directoryId = Core::Json::GetStringValue(v, "DirectoryId");
            r.function = Core::Json::GetStringValue(v, "Function");
            r.invocationRole = Core::Json::GetStringValue(v, "InvocationRole");
            r.sftpAuthenticationMethod = SftpAuthenticationMethodFromString(Core::Json::GetStringValue(v, "SftpAuthenticationMethod"));
            r.url = Core::Json::GetStringValue(v, "Url");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, IdentityProviderDetails const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"DirectoryId", obj.directoryId},
                    {"Function", obj.function},
                    {"InvocationRole", obj.invocationRole},
                    {"SftpAuthenticationMethod", SftpAuthenticationMethodToString(obj.sftpAuthenticationMethod)},
                    {"Url", obj.url},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H
