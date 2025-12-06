//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_NETWORK_REQUEST_H
#define AWSMOCK_DTO_DOCKER_CREATE_NETWORK_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Create network request.
     *
     * @par
     * Adds the DNS entries for S3 host-style requests. This needs a DNS server which is able to resolve the hostnames. Usually on Linux this can be done using 'dnsmasq'. You
     * need to setup the hosts in dnsmasq. The host names must conform to the AWS S3 specification, i.e.: &lt;bucketname&gt;.s3.&lt;region&gt;.&lt;domainname&gt;.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateNetworkRequest final : Common::BaseCounter<CreateNetworkRequest> {

        /**
         * Network name
         */
        std::string name;

        /**
         * Driver name
         */
        std::string driver = "bridge";

      private:

        friend CreateNetworkRequest tag_invoke(boost::json::value_to_tag<CreateNetworkRequest>, boost::json::value const &v) {
            CreateNetworkRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.driver = Core::Json::GetStringValue(v, "Driver");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateNetworkRequest const &obj) {
            jv = {
                    {"Name", obj.name},
                    {"Driver", obj.driver},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
