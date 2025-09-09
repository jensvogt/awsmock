//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/docker/model/Container.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List container request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListContainerResponse final : Common::BaseCounter<ListContainerResponse> {

        /**
         * Container list
         */
        std::vector<Container> containerList;

        /**
         * HTTP status
         */
        boost::beast::http::status status = boost::beast::http::status::unknown;

      private:

        friend ListContainerResponse tag_invoke(boost::json::value_to_tag<ListContainerResponse>, boost::json::value const &v) {
            ListContainerResponse r;
            r.containerList = boost::json::value_to<std::vector<Container>>(v);
            r.status = Core::HttpUtils::StatusCodeFromString(Core::Json::GetStringValue(v, "status"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListContainerResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.containerList)},
                    {"status", Core::HttpUtils::StatusCodeToString(obj.status)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H
