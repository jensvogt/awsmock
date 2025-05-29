//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Exposed port
     *
     * @par
     * Defines the port which will be connected to the outside world.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExposedPort {

        /**
         * Internal port
         */
        std::string internalPort;

        /**
         * Internal protocol
         */
        std::string protocol;
    };

    /**
     * @brief Create container request.
     *
     * @par
     * Adds the DNS entries for S3 host-style requests. This needs a DNS server which is able to resolve the hostnames. Usually on Linux this can be done using 'dnsmasq'. You
     * need to set up the hosts in dnsmasq. The host names must conform to the AWS S3 specification, i.e.: &lt;bucketname&gt;.s3.&lt;region&gt;.&lt;domainname&gt;.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateContainerRequest {

        /**
         * Hostname
         */
        std::string hostName;

        /**
         * Domain name
         */
        std::string domainName;

        /**
         * User
         */
        std::string user;

        /**
         * Image
         */
        std::string image;

        /**
         * Network mode
         */
        std::string networkMode = "bridge";

        /**
         * Environment
         */
        std::vector<std::string> environment;

        /**
         * Container ports
         */
        std::string containerPort;

        /**
         * Host ports
         */
        std::string hostPort;

        /**
         * TTy settings
         */
        bool tty = true;

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
        friend std::ostream &operator<<(std::ostream &os, const CreateContainerRequest &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
