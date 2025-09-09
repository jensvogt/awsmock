//
// Created by vogje01 on 13/08/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_VERSION_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_VERSION_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/docker/model/Platform.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker details
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Details final : Common::BaseCounter<Details> {

        /**
         * API version
         */
        std::string apiVersion;

        /**
         * Architecture
         */
        std::string architecture;

        /**
         * Build time
         */
        system_clock::time_point buildTime;

        /**
         * Experimental
         */
        bool experimental = false;

        /**
         * Git commit userPoolId
         */
        std::string gitCommit;

      private:

        friend Details tag_invoke(boost::json::value_to_tag<Details>, boost::json::value const &v) {
            Details r;
            r.apiVersion = Core::Json::GetStringValue(v, "ApiVersion");
            r.architecture = Core::Json::GetStringValue(v, "Architecture");
            r.gitCommit = Core::Json::GetStringValue(v, "GitCommit");
            r.buildTime = Core::Json::GetDatetimeValue(v, "BuildTime");
            r.experimental = Core::Json::GetBoolValue(v, "Experimental");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Details const &obj) {
            jv = {
                    {"ApiVersion", obj.apiVersion},
                    {"Architecture", obj.architecture},
                    {"GitCommit", obj.gitCommit},
                    {"BuildTime", Core::DateTimeUtils::ToISO8601(obj.buildTime)},
                    {"Experimental", obj.experimental},
            };
        }
    };

    /**
     * @brief Docker component
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Component final : Common::BaseCounter<Component> {

        /**
         * Platform name
         */
        std::string name;

        /**
         * Version
         */
        std::string version;

        /**
         * Details
         */
        Details details;

      private:

        friend Component tag_invoke(boost::json::value_to_tag<Component>, boost::json::value const &v) {
            Component r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.version = Core::Json::GetStringValue(v, "Version");
            r.details = boost::json::value_to<Details>(v.at("Details"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Component const &obj) {
            jv = {
                    {"Name", obj.name},
                    {"Version", obj.version},
                    {"Details", boost::json::value_from(obj.details)},
            };
        }
    };

    /**
     * @brief Docker version
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DockerVersion final : Common::BaseCounter<DockerVersion> {

        /**
         * Platform object
         */
        Platform platform;

        /**
         * Components
         */
        std::vector<Component> components;

      private:

        friend DockerVersion tag_invoke(boost::json::value_to_tag<DockerVersion>, boost::json::value const &v) {
            DockerVersion r = {};
            if (Core::Json::AttributeExists(v, "Platform")) {
                r.platform = boost::json::value_to<Platform>(v.at("Platform"));
            }
            if (Core::Json::AttributeExists(v, "Components")) {
                r.components = boost::json::value_to<std::vector<Component>>(v.at("Components"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DockerVersion const &obj) {
            jv = {
                    {"Platform", boost::json::value_from(obj.platform)},
                    {"Components", boost::json::value_from(obj.components)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_VERSION_RESPONSE_H
