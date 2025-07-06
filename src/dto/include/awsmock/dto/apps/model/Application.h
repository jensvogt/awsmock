//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_APPLICATION_H
#define AWSMOCK_DTO_APPS_APPLICATION_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/apps/model/Runtime.h>
#include <awsmock/dto/apps/model/Status.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    using std::chrono::system_clock;

    /**
     * @brief Application DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Application final : Common::BaseCounter<Application> {

        /**
         * Application name
         */
        std::string name;

        /**
         * Application runtime
         */
        AppsRuntimeType runtime = AppsRuntimeType::UNKNOWN;

        /**
         * Application archive
         */
        std::string archive;

        /**
         * Application version
         */
        std::string version;

        /**
         * Application docker container ID
         */
        std::string containerId;

        /**
         * Application status
         */
        AppsStatusType status = AppsStatusType::UNKNOWN;

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();


      private:

        friend Application tag_invoke(boost::json::value_to_tag<Application>, boost::json::value const &v) {
            Application r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.runtime = AppsRuntimeTypeFromString(Core::Json::GetStringValue(v, "runtime"));
            r.archive = Core::Json::GetStringValue(v, "archive");
            r.version = Core::Json::GetStringValue(v, "version");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.status = AppsStatusTypeFromString(Core::Json::GetStringValue(v, "status"));
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Application const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"runtime", AppsRuntimeTypeToString(obj.runtime)},
                    {"archive", obj.archive},
                    {"version", obj.version},
                    {"containerId", obj.containerId},
                    {"status", AppsStatusTypeToString(obj.status)},
                    {"enabled", obj.enabled},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_APPLICATION_H
