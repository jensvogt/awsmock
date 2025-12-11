//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_START_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_CONTAINER_START_CONTAINER_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Start a container request
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
    struct StartContainerRequest final : Common::BaseCounter<StartContainerRequest> {

        /**
         * Container ID
         */
        std::string containerId;

      private:

        friend StartContainerRequest tag_invoke(boost::json::value_to_tag<StartContainerRequest>, boost::json::value const &v) {
            StartContainerRequest r;
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            return r;
        }

        friend void
        tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StartContainerRequest const &obj) {
            jv = {
                    {"containerId", obj.containerId},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_START_CONTAINER_REQUEST_H
