//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_NETWORK_REQUEST_H
#define AWSMOCK_DTO_DOCKER_CREATE_NETWORK_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

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
    struct CreateNetworkRequest {

        /**
         * Network name
         */
        std::string name;

        /**
         * Driver name
         */
        std::string driver = "bridge";

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CreateNetworkRequest &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
