//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_NETWORK_H
#define AWSMOCK_DTO_DOCKER_NETWORK_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker network object
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Network final : Common::BaseCounter<Network> {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Name
         */
        std::string name;

        /**
         * Driver
         */
        std::string driver;

        /**
         * Scope
         */
        std::string scope;

        /**
         * IPv4 enabled
         */
        bool enableIPv4{};

        /**
         * IPv6 enabled
         */
        bool enableIPv6{};

        /**
         * Created date time
         */
        system_clock::time_point created;

      private:

        friend Network tag_invoke(boost::json::value_to_tag<Network>, boost::json::value const &v) {
            Network r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.name = Core::Json::GetStringValue(v, "Name");
            r.driver = Core::Json::GetStringValue(v, "Driver");
            r.scope = Core::Json::GetStringValue(v, "Scope");
            r.enableIPv4 = Core::Json::GetBoolValue(v, "EnableIPv4");
            r.enableIPv6 = Core::Json::GetBoolValue(v, "EnableIPv6");
            r.created = Core::Json::GetDatetimeValue(v, "Created");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Network const &obj) {
            jv = {
                    {"id", obj.id},
                    {"name", obj.name},
                    {"driver", obj.driver},
                    {"scope", obj.scope},
                    {"EnableIPv4", obj.enableIPv4},
                    {"EnableIPv6", obj.enableIPv6},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_NETWORK_H
