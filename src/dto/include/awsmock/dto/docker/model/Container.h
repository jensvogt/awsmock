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
     * @brief Docker container
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
        std::vector<Port> ports;

        /**
         * The size of files that have been created or changed by this container
         */
        long sizeRw = 0;

        /**
         * The total size of all the files in this container
         */
        long sizeRootFs = 0;

      private:

        friend Container tag_invoke(boost::json::value_to_tag<Container>, boost::json::value const &v) {
            Container r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.image = Core::Json::GetStringValue(v, "Image");
            r.imageId = Core::Json::GetStringValue(v, "ImageId");
            r.status = Core::Json::GetStringValue(v, "Status");
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
            if (Core::Json::AttributeExists(v, "Ports")) {
                r.ports = boost::json::value_to<std::vector<Port>>(v.at("Ports"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Container const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Image", obj.imageId},
                    {"ImageId", obj.imageId},
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
