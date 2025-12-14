//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_DELETE_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_CONTAINER_DELETE_CONTAINER_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {
    /**
     * @brief Delete a container request
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
    struct DeleteContainerRequest final : Common::BaseCounter<DeleteContainerRequest> {
        /**
         * Container ID
         */
        std::string containerId;

    private:
        friend DeleteContainerRequest tag_invoke(boost::json::value_to_tag<DeleteContainerRequest>, boost::json::value const &v) {
            DeleteContainerRequest r;
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteContainerRequest const &obj) {
            jv = {
                {"containerId", obj.containerId},
            };
        }
    };
} // namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_DELETE_CONTAINER_REQUEST_H
