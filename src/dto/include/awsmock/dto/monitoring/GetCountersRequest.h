//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Monitoring {

    using std::chrono::system_clock;

    /**
     * @brief Get counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetCountersRequest final : Common::BaseCounter<GetCountersRequest> {

        /**
         * Name
         */
        std::string name;

        /**
         * Label name
         */
        std::string labelName;

        /**
         * Label name
         */
        std::string labelValue;

        /**
         * Stepping
         */
        long step{};

        /**
         * Limit to top x counters
         */
        long limit = 10;

        /**
         * Start time
         */
        system_clock::time_point start;

        /**
         * End time
         */
        system_clock::time_point end;

      private:

        friend GetCountersRequest tag_invoke(boost::json::value_to_tag<GetCountersRequest>, boost::json::value const &v) {
            GetCountersRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            if (Core::Json::AttributeExists(v, "labelName")) {
                r.labelName = Core::Json::GetStringValue(v, "labelName");
            }
            if (Core::Json::AttributeExists(v, "labelValue")) {
                r.labelValue = Core::Json::GetStringValue(v, "labelValue");
            }
            r.step = Core::Json::GetLongValue(v, "step");
            r.limit = Core::Json::GetLongValue(v, "limit");
#ifdef _WIN32
            r.start = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongLongValue(v, "start"));
            r.end = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongLongValue(v, "end"));
#else
            r.start = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(v, "start"));
            r.end = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(v, "end"));
#endif// _WIN32
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetCountersRequest const &obj) {
            jv = {
                    {"name", obj.name},
                    {"labelName", obj.labelName},
                    {"labelValue", obj.labelValue},
                    {"step", obj.step},
                    {"limit", obj.limit},
                    {"start", Core::DateTimeUtils::UnixTimestamp(obj.start)},
                    {"end", Core::DateTimeUtils::UnixTimestamp(obj.end)},
            };
        }
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H
