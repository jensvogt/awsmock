//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_STOP_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_CONTAINER_STOP_CONTAINER_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Stop a container request
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
    struct StopContainerRequest final : Common::BaseCounter<StopContainerRequest> {

        /**
         * Container ID
         */
        std::string containerId;

      private:

        friend StopContainerRequest tag_invoke(boost::json::value_to_tag<StopContainerRequest>, boost::json::value const &v) {
            StopContainerRequest r;
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopContainerRequest const &obj) {
            jv = {
                    {"containerId", obj.containerId},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_STOP_CONTAINER_REQUEST_H
