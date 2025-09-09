//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_PORT_H
#define AWSMOCK_DTO_DOCKER_PORT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker port
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Port final : Common::BaseCounter<Port> {

        /**
         * Host IP address
         */
        std::string hostIp;

        /**
         * Public port means port inside the container
         */
        long hostPort{};

        /**
         * @brief Convert to a JSON string
         *
         * @param object JSON object
         */
        void FromDocument(const view_or_value<view, value> &object) {

            try {

                hostIp = Core::Bson::BsonUtils::GetStringValue(object, "HostIp");
                hostPort = Core::Bson::BsonUtils::GetIntValue(object, "HostPort");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Port tag_invoke(boost::json::value_to_tag<Port>, boost::json::value const &v) {
            Port r;
            r.hostIp = Core::Json::GetStringValue(v, "HostIp");
            r.hostPort = Core::Json::GetLongValue(v, "HostPort");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Port const &obj) {
            jv = {
                    {"HostIp", obj.hostIp},
                    {"HostPort", obj.hostPort},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_PORT_H
