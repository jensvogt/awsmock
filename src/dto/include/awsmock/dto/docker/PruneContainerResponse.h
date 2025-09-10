//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_PRUNE_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_PRUNE_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Prune docker response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PruneContainerResponse final : Common::BaseCounter<PruneContainerResponse> {

        /**
         * Image ID
         */
        std::vector<std::string> containersDeleted;

        /**
         * Space reclaimed
         */
        long spaceReclaimed{};

      private:

        friend PruneContainerResponse tag_invoke(boost::json::value_to_tag<PruneContainerResponse>, boost::json::value const &v) {
            PruneContainerResponse r;
            if (Core::Json::AttributeExists(v, "ContainersDeleted")) {
                r.containersDeleted = boost::json::value_to<std::vector<std::string>>(v.at("ContainersDeleted"));
            }
            r.spaceReclaimed = Core::Json::GetLongValue(v, "SpaceReclaimed");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PruneContainerResponse const &obj) {
            jv = {
                    {"ContainersDeleted", boost::json::value_from(obj.containersDeleted)},
                    {"SpaceReclaimed", obj.spaceReclaimed},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_PRUNE_CONTAINER_RESPONSE_H
