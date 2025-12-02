//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretCounter.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SecretsManager {

    struct UpdateSecretDetailsResponse final : Common::BaseCounter<UpdateSecretDetailsResponse> {

        /**
         * Secret details
         */
        SecretCounter secretDetails;

      private:

        friend UpdateSecretDetailsResponse tag_invoke(boost::json::value_to_tag<UpdateSecretDetailsResponse>, boost::json::value const &v) {
            UpdateSecretDetailsResponse r;
            if (Core::Json::AttributeExists(v, "secretDetails")) {
                r.secretDetails = boost::json::value_to<SecretCounter>(v.at("secretDetails"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretDetailsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"secretDetails", boost::json::value_from(obj.secretDetails)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_COUNTERS_RESPONSE_H
