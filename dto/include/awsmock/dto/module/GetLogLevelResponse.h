//
// Created by vogje01 on 06/07/2025
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Module {

    struct LogLevel {
        /**
         * @brief Name of the logger
         */
        std::string name;

        /**
         * @brief Log level of the logger.
         */
        std::string level;

      private:

        friend LogLevel tag_invoke(boost::json::value_to_tag<LogLevel>, boost::json::value const &v) {
            LogLevel r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.level = Core::Json::GetStringValue(v, "level");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LogLevel const &obj) {
            jv = {
                    {"name", obj.name},
                    {"level", obj.level},
            };
        }
    };

    /**
     * @brief Get log level response
     *
     * @code{.json}
     * {
     *   "logLevels": [
     *     { "name": "S3",  "level": "debug" },
     *     { "name": "SQS", "level": "info"  }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetLogLevelResponse final : Common::BaseCounter<GetLogLevelResponse> {

        std::vector<LogLevel> logLevels;

      private:

        friend GetLogLevelResponse tag_invoke(boost::json::value_to_tag<GetLogLevelResponse>, boost::json::value const &v) {
            GetLogLevelResponse r;
            if (Core::Json::AttributeExists(v, "logLevels")) {
                for (const auto &item: v.at("logLevels").as_array()) {
                    r.logLevels.push_back(boost::json::value_to<LogLevel>(item));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLogLevelResponse const &obj) {
            boost::json::array arr;
            for (const auto &ll: obj.logLevels) {
                arr.emplace_back(boost::json::value_from(ll));
            }
            jv = {{"logLevels", arr}};
        }
    };

}// namespace Awsmock::Dto::Module
