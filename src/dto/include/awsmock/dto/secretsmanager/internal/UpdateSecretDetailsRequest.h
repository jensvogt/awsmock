//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_DETAILS_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_DETAILS_REQUEST_H


// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretCounter.h>

namespace AwsMock::Dto::SecretsManager {

    struct UpdateSecretDetailsRequest final : Common::BaseCounter<UpdateSecretDetailsRequest> {

        /**
         * Secret details
         */
        SecretCounter secretDetails;

      private:

        friend UpdateSecretDetailsRequest tag_invoke(boost::json::value_to_tag<UpdateSecretDetailsRequest>, boost::json::value const &v) {
            UpdateSecretDetailsRequest r;
            if (Core::Json::AttributeExists(v, "secretDetails")) {
                r.secretDetails = boost::json::value_to<SecretCounter>(v.at("secretDetails"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretDetailsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"secretDetails", boost::json::value_from(obj.secretDetails)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_DETAILS_REQUEST_H
