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
     * @code{.json}
     * [ {
     *     "Id" : "7d4737b052b02043db8e92a1b550eec9df54ac917c6c582a421a1481e226efcd",
     *     "Names" : [ "/ftp-file-copy-23e4638f" ],
     *     "Image" : "ftp-file-copy:latest",
     *     "ImageID" : "sha256:d62efc48091d2dd5c241b9c4fbc3cbc4a894e65da1f1bf4c5e524ceb82e691ff",
     *     "Command" : "/lambda-entrypoint.sh org.springframework.cloud.function.adapter.aws.FunctionInvoker::handleRequest",
     *     "Created" : 1760866109,
     *     "Ports" : [ {
     *       "IP" : "0.0.0.0",
     *       "PrivatePort" : 8085,
     *       "PublicPort" : 8085,
     *       "Type" : "tcp"
     *     }, {
     *       "IP" : "::",
     *       "PrivatePort" : 8085,
     *       "PublicPort" : 8085,
     *       "Type" : "tcp"
     *     } ],
     *     "Labels" : {
     *       "com.amazonaws.lambda.platform.kernel" : "k510ga",
     *       "com.amazonaws.lambda.platform.version" : "sbxv2brave"
     *     },
     *     "State" : "created",
     *     "Status" : "Created",
     *     "HostConfig" : {
     *       "NetworkMode" : "local"
     *     },
     *     "NetworkSettings" : {
     *       "Networks" : {
     *         "local" : {
     *           "IPAMConfig" : null,
     *           "Links" : null,
     *           "Aliases" : null,
     *           "MacAddress" : "",
     *           "DriverOpts" : null,
     *           "GwPriority" : 0,
     *           "NetworkID" : "",
     *           "EndpointID" : "",
     *           "Gateway" : "",
     *           "IPAddress" : "",
     *           "IPPrefixLen" : 0,
     *           "IPv6Gateway" : "",
     *           "GlobalIPv6Address" : "",
     *           "GlobalIPv6PrefixLen" : 0,
     *           "DNSNames" : null
     *         }
     *       }
     *     },
     *     "Mounts" : [ ]
     *   }
     *   ...
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListContainerResponse final : Common::BaseCounter<ListContainerResponse> {

        /**
         * Container list
         */
        std::vector<Container> containerList;

      private:

        friend ListContainerResponse tag_invoke(boost::json::value_to_tag<ListContainerResponse>, boost::json::value const &v) {
            ListContainerResponse r;
            r.containerList = boost::json::value_to<std::vector<Container>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListContainerResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.containerList)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H
