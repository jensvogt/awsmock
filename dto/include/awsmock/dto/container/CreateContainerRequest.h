//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
#define AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/container/model/HostConfig.h>

namespace Awsmock::Dto::Docker {

    /**
     * @brief Exposed port
     *
     * @par
     * Defines the port which will be connected to the outside world.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExposedPort final : Common::BaseObject<ExposedPort> {

      private:

        friend ExposedPort tag_invoke(boost::json::value_to_tag<ExposedPort>, boost::json::value const &v) {
            ExposedPort r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExposedPort const &obj) {
            jv = {};
        }
    };

    /**
     * @brief Docker container healthcheck configuration.
     */
    struct Healthcheck final : Common::BaseObject<Healthcheck> {

        /**
         * Test command: ["CMD-SHELL", "<shell-cmd>"] or ["NONE"] to disable.
         */
        std::vector<std::string> test;

        /**
         * Interval between checks in nanoseconds (default 5s).
         */
        long long interval = 5'000'000'000LL;

        /**
         * Per-check timeout in nanoseconds (default 2s).
         */
        long long timeout = 2'000'000'000LL;

        /**
         * Number of consecutive failures before marking unhealthy.
         */
        int retries = 3;

        /**
         * Grace period before health checks start in nanoseconds (default 30s).
         */
        long long startPeriod = 30'000'000'000LL;

      private:

        friend Healthcheck tag_invoke(boost::json::value_to_tag<Healthcheck>, boost::json::value const &) {
            return {};
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Healthcheck const &obj) {
            jv = {
                    {"Test", boost::json::value_from(obj.test)},
                    {"Interval", obj.interval},
                    {"Timeout", obj.timeout},
                    {"Retries", obj.retries},
                    {"StartPeriod", obj.startPeriod},
            };
        }
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
    struct CreateContainerRequest final : Common::BaseCounter<CreateContainerRequest> {

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
         * Container vcpkg-ports
         */
        std::string containerPort;

        /**
         * Host vcpkg-ports
         */
        std::string hostPort;

        /**
         * Host config
         */
        HostConfig hostConfig;

        /**
         * Exposed vcpkg-ports
         */
        std::map<std::string, ExposedPort> exposedPorts;

        /**
         * TTy settings
         */
        bool tty = false;

        /**
         * Environment
         */
        std::vector<std::string> environment;

        /**
         * Container healthcheck configuration.
         */
        Healthcheck healthcheck;

      private:

        friend CreateContainerRequest tag_invoke(boost::json::value_to_tag<CreateContainerRequest>, boost::json::value const &v) {
            CreateContainerRequest r;
            r.hostName = Core::Json::GetStringValue(v, "HostName");
            r.domainName = Core::Json::GetStringValue(v, "DomainName");
            r.user = Core::Json::GetStringValue(v, "User");
            r.image = Core::Json::GetStringValue(v, "Image");
            r.containerPort = Core::Json::GetStringValue(v, "ContainerPort");
            r.hostPort = Core::Json::GetStringValue(v, "HostPort");
            r.tty = Core::Json::GetBoolValue(v, "Tty");
            if (Core::Json::AttributeExists(v, "HostConfig")) {
                r.hostConfig = boost::json::value_to<HostConfig>(v.at("HostConfig"));
            }
            if (Core::Json::AttributeExists(v, "ExposedPorts")) {
                r.exposedPorts = boost::json::value_to<std::map<std::string, ExposedPort>>(v.at("ExposedPorts"));
            }
            if (Core::Json::AttributeExists(v, "Env")) {
                r.environment = boost::json::value_to<std::vector<std::string>>(v.at("Env"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateContainerRequest const &obj) {
            jv = {
                    {"HostName", obj.hostName},
                    {"DomainName", obj.domainName},
                    {"User", obj.user},
                    {"Image", obj.image},
                    {"ContainerPort", obj.containerPort},
                    {"HostPort", obj.hostPort},
                    {"Tty", obj.tty},
                    {"HostConfig", boost::json::value_from(obj.hostConfig)},
                    {"ExposedPorts", boost::json::value_from(obj.exposedPorts)},
                    {"Env", boost::json::value_from(obj.environment)},
                    {"Healthcheck", boost::json::value_from(obj.healthcheck)},
            };
        }
    };

}// namespace Awsmock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_REQUEST_H
