//
// Created by vogje01 on 06/07/2025
//

#ifndef AWSMOCK_DTO_SET_LOG_LEVEL_REQUEST_H
#define AWSMOCK_DTO_SET_LOG_LEVEL_REQUEST_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Module {

    /**
     * @brief Set log level request
     *
     * @code{.json}
     * {
     *   "level":   "debug",
     *   "channel": "S3"
     * }
     * @endcode
     *
     * @par
     * When 'channel' is empty, the global log level is changed; otherwise only the
     * named channel is affected.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SetLogLevelRequest final : Common::BaseCounter<SetLogLevelRequest> {

        /**
         * Severity level string (e.g. "debug", "info", "warn", "error")
         */
        std::string level;

        /**
         * Optional channel name (e.g. "S3", "SQS"). Empty means global.
         */
        std::string channel;

      private:

        friend SetLogLevelRequest tag_invoke(boost::json::value_to_tag<SetLogLevelRequest>, boost::json::value const &v) {
            SetLogLevelRequest r;
            r.level = Core::Json::GetStringValue(v, "level");
            r.channel = Core::Json::GetStringValue(v, "channel");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SetLogLevelRequest const &obj) {
            jv = {
                    {"level", obj.level},
                    {"channel", obj.channel},
            };
        }
    };

}// namespace Awsmock::Dto::Module

#endif// AWSMOCK_DTO_SET_LOG_LEVEL_REQUEST_H
