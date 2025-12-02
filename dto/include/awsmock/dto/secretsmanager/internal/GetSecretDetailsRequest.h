//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_REQUEST_H


// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    struct GetSecretDetailsRequest final : Common::BaseCounter<GetSecretDetailsRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

      private:

        friend GetSecretDetailsRequest tag_invoke(boost::json::value_to_tag<GetSecretDetailsRequest>, boost::json::value const &v) {
            GetSecretDetailsRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSecretDetailsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_REQUEST_H
