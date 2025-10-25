//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/docker/model/HostConfig.h>
#include <awsmock/dto/docker/model/State.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Inspect container response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct InspectContainerResponse final : Common::BaseCounter<InspectContainerResponse> {

        /**
         * ID
         */
        std::string id;

        /**
         * Name
         */
        std::string name;

        /**
         * Path
         */
        std::string path;

        /**
         * Image
         */
        std::string image;

        /**
         * State
         */
        State state;

        /**
         * Host config
         */
        HostConfig hostConfig;

        /**
         * The size of files that have been created or changed by this container.
         */
        long sizeRw{};

        /**
         * The total size of all files in the read-only layers from the image that the container uses. These layers can be shared between containers.
         */
        long sizeRootFs{};

        /**
         * Status of the HTTP call
         */
        boost::beast::http::status status = boost::beast::http::status::unknown;

        /**
         * Return container name
         */
        std::string GetContainerName() {
            if (!name.empty() && Core::StringUtils::StartsWith(name, "/")) {
                return name.substr(1);
            }
            return name;
        }

      private:

        friend InspectContainerResponse tag_invoke(boost::json::value_to_tag<InspectContainerResponse>, boost::json::value const &v) {
            InspectContainerResponse r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.name = Core::Json::GetStringValue(v, "Name");
            r.path = Core::Json::GetStringValue(v, "Path");
            r.image = Core::Json::GetStringValue(v, "Image");
            r.state = boost::json::value_to<State>(v.at("State"));
            r.status = Core::HttpUtils::StatusCodeFromString(Core::Json::GetStringValue(v, "Status"));
            r.hostConfig = boost::json::value_to<HostConfig>(v.at("HostConfig"));
            r.sizeRw = Core::Json::GetLongValue(v, "SizeRw");
            r.sizeRootFs = Core::Json::GetLongValue(v, "SizeRootFs");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InspectContainerResponse const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Name", obj.name},
                    {"Path", obj.path},
                    {"Image", obj.image},
                    {"State", boost::json::value_from(obj.state)},
                    {"Status", Core::HttpUtils::StatusCodeToString(obj.status)},
                    {"HostConfig", boost::json::value_from(obj.hostConfig)},
                    {"SizeRw", obj.sizeRw},
                    {"SizeRootFs", obj.sizeRootFs},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H
