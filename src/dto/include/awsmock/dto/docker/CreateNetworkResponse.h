//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_NETWORK_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_CREATE_NETWORK_RESPONSE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Create network response.
     *
     * @par
     * Adds the DNS entries for S3 host-style requests. This needs a DNS server which is able to resolve the hostnames. Usually on Linux this can be done using 'dnsmasq'. You
     * need to set up the hosts in dnsmasq. The host names must conform to the AWS S3 specification, i.e.: &lt;bucketname&gt;.s3.&lt;region&gt;.&lt;domainname&gt;
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateNetworkResponse final : Common::BaseCounter<CreateNetworkResponse> {

        /**
         * Network ID
         */
        std::string id;

        /**
         * Warning
         */
        std::string warning;

      private:

        friend CreateNetworkResponse tag_invoke(boost::json::value_to_tag<CreateNetworkResponse>, boost::json::value const &v) {
            CreateNetworkResponse r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.warning = Core::Json::GetStringValue(v, "Warning");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateNetworkResponse const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Warning", obj.warning},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_NETWORK_RESPONSE_H
