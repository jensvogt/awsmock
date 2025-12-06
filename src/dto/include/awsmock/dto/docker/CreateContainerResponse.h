//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Create container response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateContainerResponse final : Common::BaseCounter<CreateContainerResponse> {

        /**
         * ID
         */
        std::string id;

        /**
         * Warnings
         */
        std::vector<std::string> warnings;

        /**
         * Assigned random port
         */
        int hostPort{};

      private:

        friend CreateContainerResponse tag_invoke(boost::json::value_to_tag<CreateContainerResponse>, boost::json::value const &v) {
            CreateContainerResponse r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.hostPort = Core::Json::GetIntValue(v, "hostPort");
            if (Core::Json::AttributeExists(v, "Warnings")) {
                r.warnings = boost::json::value_to<std::vector<std::string>>(v.at("Warnings"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateContainerResponse const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"HostPort", obj.hostPort},
                    {"Warnings", boost::json::value_from(obj.warnings)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H
