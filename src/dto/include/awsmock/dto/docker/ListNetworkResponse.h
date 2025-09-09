//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LIST_NETWORK_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_LIST_NETWORK_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/docker/model/Network.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List networks response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListNetworkResponse final : Common::BaseCounter<ListNetworkResponse> {

        /**
         * Network list
         */
        std::vector<Network> networks;

      private:

        friend ListNetworkResponse tag_invoke(boost::json::value_to_tag<ListNetworkResponse>, boost::json::value const &v) {
            ListNetworkResponse r;
            r.networks = boost::json::value_to<std::vector<Network>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListNetworkResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.networks)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_NETWORK_RESPONSE_H
