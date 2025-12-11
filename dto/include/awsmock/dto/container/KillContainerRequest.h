//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_KILL_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_CONTAINER_KILL_CONTAINER_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Kill a container request
     *
     * @code{.json}
     * [
     *  "containerId": string
     *   ...
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KillContainerRequest final : Common::BaseCounter<KillContainerRequest> {

        /**
         * Container ID
         */
        std::string containerId;

      private:

        friend KillContainerRequest tag_invoke(boost::json::value_to_tag<KillContainerRequest>, boost::json::value const &v) {
            KillContainerRequest r;
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, KillContainerRequest const &obj) {
            jv = {
                    {"containerId", obj.containerId},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_KILL_CONTAINER_REQUEST_H
