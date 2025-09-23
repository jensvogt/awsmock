//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_LOG_CONFIG_H
#define AWSMOCK_DTO_DOCKER_LOG_CONFIG_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/docker/model/PortBinding.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker log config object
     *
     * @par
     * Currently not used.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Config final : Common::BaseCounter<Config> {

      private:

        friend Config tag_invoke(boost::json::value_to_tag<Config>, boost::json::value const &v) {
            Config r;
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Config const &obj) {
            jv = {};
        }
    };

    /**
     * @brief Docker log config object
     *
     * @code{.jso}
     * "LogConfig" : {
     *   "Type" : "json-file",
     *   "Config" : { }
     * }
     * @endCode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LogConfig final : Common::BaseCounter<LogConfig> {

        /**
         * Logging type
         */
        std::string type = "json-file";

        /**
         * Logging configuration
         */
        Config config;

        /**
         * Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                type = Core::Bson::BsonUtils::GetStringValue(document, "Type");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend LogConfig tag_invoke(boost::json::value_to_tag<LogConfig>, boost::json::value const &v) {
            LogConfig r;
            r.type = Core::Json::GetStringValue(v, "Type");
            if (Core::Json::AttributeExists(v, "Config")) {
                r.config = boost::json::value_to<Config>(v.at("Config"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LogConfig const &obj) {
            jv = {
                    {"Type", obj.type},
                    {"Config", boost::json::value_from(obj.config)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_HOST_CONFIG_H
