//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CONTAINER_H
#define AWSMOCK_DTO_DOCKER_CONTAINER_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/docker/model/Port.h>

#define LAMBDA_INTERNAL_PORT 8080

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container port
     *
     * @par
     * Unfortunately, the different Docker REST Api's use different structures for the same object. For instance, the inspect-container returns
     * a totally different JSON structure, then the list-container command. Therefore, we have to use different JSON structures here.
     *
     * @code{json}
     * {
     *   "IP" : "0.0.0.0",
     *   "PrivatePort" : 8085,
     *   "PublicPort" : 8085,
     *   "Type" : "tcp"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ContainerPort final : Common::BaseCounter<ContainerPort> {

        /**
         * IP address
         */
        std::string ipAddress;

        /**
         * Private port
         */
        int privatePort{};

        /**
         * Public port
         */
        int publicPort{};

        /**
         * Type
         */
        std::string type;

      private:

        friend ContainerPort tag_invoke(boost::json::value_to_tag<ContainerPort>, boost::json::value const &v) {
            ContainerPort r;
            r.ipAddress = Core::Json::GetStringValue(v, "IP");
            r.privatePort = Core::Json::GetIntValue(v, "PrivatePort");
            r.publicPort = Core::Json::GetIntValue(v, "PublicPort");
            r.type = Core::Json::GetStringValue(v, "Type");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ContainerPort const &obj) {
            jv = {
                    {"IP", obj.ipAddress},
                    {"PrivatePort", obj.privatePort},
                    {"PublicPort", obj.publicPort},
                    {"Type", obj.type},
            };
        }
    };

    /**
     * @brief Docker container
     *
     * @par
     * Unfortunately, the different Docker REST Api's use different structures for the same object. For instance, the inspect-container returns
     * a totally different JSON structure, then the list-container command. Therefore, we have to use different JSON structures here.
     *
     * @code{json}
     * {
     *   "Id" : "7d4737b052b02043db8e92a1b550eec9df54ac917c6c582a421a1481e226efcd",
     *   "Names" : [ "/ftp-file-copy-23e4638f" ],
     *   "Image" : "ftp-file-copy:latest",
     *   "ImageID" : "sha256:d62efc48091d2dd5c241b9c4fbc3cbc4a894e65da1f1bf4c5e524ceb82e691ff",
     *   "Command" : "/lambda-entrypoint.sh org.springframework.cloud.function.adapter.aws.FunctionInvoker::handleRequest",
     *   "Created" : 1760866109,
     *   "Ports" : [ {
     *     "IP" : "0.0.0.0",
     *     "PrivatePort" : 8085,
     *     "PublicPort" : 8085,
     *     "Type" : "tcp"
     *   }, {
     *     "IP" : "::",
     *     "PrivatePort" : 8085,
     *     "PublicPort" : 8085,
     *     "Type" : "tcp"
     *   } ],
     *   "Labels" : {
     *     "com.amazonaws.lambda.platform.kernel" : "k510ga",
     *     "com.amazonaws.lambda.platform.version" : "sbxv2brave"
     *   },
     *   "State" : "created",
     *   "Status" : "Created",
     *   "HostConfig" : {
     *     "NetworkMode" : "local"
     *   },
     *   "NetworkSettings" : {
     *     "Networks" : {
     *       "local" : {
     *         "IPAMConfig" : null,
     *         "Links" : null,
     *         "Aliases" : null,
     *         "MacAddress" : "",
     *         "DriverOpts" : null,
     *         "GwPriority" : 0,
     *         "NetworkID" : "",
     *         "EndpointID" : "",
     *         "Gateway" : "",
     *         "IPAddress" : "",
     *         "IPPrefixLen" : 0,
     *         "IPv6Gateway" : "",
     *         "GlobalIPv6Address" : "",
     *         "GlobalIPv6PrefixLen" : 0,
     *         "DNSNames" : null
     *       }
     *     }
     *   },
     *   "Mounts" : [ ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Container final : Common::BaseCounter<Container> {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Container names
         */
        std::vector<std::string> names;

        /**
         * Image
         */
        std::string image;

        /**
         * Image ID
         */
        std::string imageId;

        /**
         * Command
         */
        std::string command;

        /**
         * The state of this container (e.g. Exited)
         */
        State state;

        /**
         * Additional human-readable status of this container (e.g. Exit 0)
         */
        std::string status;

        /**
         * The ports exposed by this container
         */
        std::vector<ContainerPort> ports{};

        /**
         * The size of files that have been created or changed by this container
         */
        long sizeRw = 0;

        /**
         * The total size of all the files in this container
         */
        long sizeRootFs = 0;

        /**
         * @brief Return the first name
         */
        [[nodiscard]] std::string GetContainerName() const {
            if (names.empty()) return "";
            return Core::StringUtils::StartsWith(names.front(), "/") ? names.front().substr(1) : names.front();
        }

        /**
         * @brief Return the private port, TCP version 4
         */
        [[nodiscard]] int GetPrivatePortV4() const {
            if (ports.empty()) return -1;
            for (const auto &port: ports) {
                if (Core::StringUtils::Contains(port.ipAddress, ".")) {
                    return port.privatePort;
                }
            }
            return -1;
        }

        /**
         * @brief Return the public port, TCP version 4
         */
        [[nodiscard]] int GetPublicPortV4() const {
            if (ports.empty()) return -1;
            for (const auto &port: ports) {
                if (Core::StringUtils::Contains(port.ipAddress, ".")) {
                    return port.publicPort;
                }
            }
            return -1;
        }

      private:

        friend Container tag_invoke(boost::json::value_to_tag<Container>, boost::json::value const &v) {
            Container r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.image = Core::Json::GetStringValue(v, "Image");
            r.imageId = Core::Json::GetStringValue(v, "ImageID");
            r.status = Core::Json::GetStringValue(v, "Status");
            r.command = Core::Json::GetStringValue(v, "Command");
            r.sizeRw = Core::Json::GetLongValue(v, "SizeRw");
            r.sizeRootFs = Core::Json::GetLongValue(v, "SizeRootFs");
            if (Core::Json::AttributeExists(v, "State") && v.at("State").is_object()) {
                r.state = boost::json::value_to<State>(v.at("State"));
            } else if (Core::Json::AttributeExists(v, "State") && v.at("State").is_string()) {
                if (const std::string stateString = Core::Json::GetStringValue(v, "State"); stateString == "running") {
                    r.state.running = true;
                }
            }
            if (Core::Json::AttributeExists(v, "Names")) {
                r.names = boost::json::value_to<std::vector<std::string>>(v.at("Names"));
            }
            if (Core::Json::AttributeExists(v, "Ports") && v.at("Ports").is_array()) {
                r.ports = boost::json::value_to<std::vector<ContainerPort>>(v.at("Ports"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Container const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Image", obj.imageId},
                    {"ImageID", obj.imageId},
                    {"Command", obj.command},
                    {"State", boost::json::value_from(obj.state)},
                    {"Status", obj.status},
                    {"SizeRw", obj.sizeRw},
                    {"SizeRootFs", obj.sizeRootFs},
                    {"Names", boost::json::value_from(obj.names)},
                    {"Ports", boost::json::value_from(obj.ports)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
