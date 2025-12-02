//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_TRANSFER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_CREATE_TRANSFER_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>
#include <awsmock/dto/transfer/model/Protocol.h>


namespace AwsMock::Dto::Transfer {

    struct CreateServerRequest final : Common::BaseCounter<CreateServerRequest> {

        /**
         * Domain
         */
        std::string domain;

        /**
         * Protocols
         */
        std::vector<ProtocolType> protocols;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Identity provider details
         */
        IdentityProviderDetails identityProviderDetails;

      private:

        friend CreateServerRequest tag_invoke(boost::json::value_to_tag<CreateServerRequest>, boost::json::value const &v) {
            CreateServerRequest r;
            r.domain = Core::Json::GetStringValue(v, "Domain");
            if (Core::Json::AttributeExists(v, "Tags") && !v.at("Tags").is_null()) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            if (Core::Json::AttributeExists(v, "IdentityProviderDetails") && !v.at("IdentityProviderDetails").is_null()) {
                r.identityProviderDetails = boost::json::value_to<IdentityProviderDetails>(v.at("IdentityProviderDetails"));
            }
            if (Core::Json::AttributeExists(v, "Protocols")) {
                for (const auto &p: v.at("Protocols").as_array()) {
                    r.protocols.emplace_back(ProtocolTypeFromString(p.as_string().data()));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateServerRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Domain", obj.domain},
                    //                    {"Protocols", boost::json::value_from(obj.protocols)},
                    {"IdentityProviderDetails", boost::json::value_from(obj.identityProviderDetails)}};
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif