//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_APPLICATION_H
#define AWSMOCK_DTO_APPS_APPLICATION_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/apps/model/RunType.h>
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
         * Application run type
         */
        AppsRunType runType = AppsRunType::UNKNOWN;

        /**
         * Application private port
         */
        long privatePort{};

        /**
         * Application public port
         */
        long publicPort{};

        /**
         * Application archive
         */
        std::string archive;

        /**
         * Application version
         */
        std::string version;

        /**
         * Application docker image ID
         */
        std::string imageId;

        /**
         * Application docker container ID
         */
        std::string containerId;

        /**
         * Application docker container name
         */
        std::string containerName;

        /**
         * Application status
         */
        AppsStatusType status = AppsStatusType::UNKNOWN;

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * Application description
         */
        std::string description;

        /**
         * Application docker file
         */
        std::string dockerFile;

        /**
         * Environment
         */
        std::map<std::string, std::string> environment;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Dependencies
         */
        std::vector<std::string> dependencies;

        /**
         * Creation date
         */
        system_clock::time_point lastStarted;

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
            r.runType = AppsRunTypeFromString(Core::Json::GetStringValue(v, "runType"));
            r.privatePort = Core::Json::GetLongValue(v, "privatePort");
            r.publicPort = Core::Json::GetLongValue(v, "publicPort");
            r.archive = Core::Json::GetStringValue(v, "archive");
            r.version = Core::Json::GetStringValue(v, "version");
            r.imageId = Core::Json::GetStringValue(v, "imageId");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.containerName = Core::Json::GetStringValue(v, "containerName");
            r.status = AppsStatusTypeFromString(Core::Json::GetStringValue(v, "status"));
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.description = Core::Json::GetStringValue(v, "description");
            r.dockerFile = Core::Json::GetStringValue(v, "dockerFile");
            r.environment = Core::Json::GetMapFromObject<std::string, std::string>(v, "environment");
            r.tags = Core::Json::GetMapFromObject<std::string, std::string>(v, "tags");
            r.lastStarted = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastStarted"));
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            if (Core::Json::AttributeExists(v, "dependencies")) {
                r.dependencies = boost::json::value_to<std::vector<std::string>>(v.at("dependencies"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Application const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"runtime", AppsRuntimeTypeToString(obj.runtime)},
                    {"runType", AppsRunTypeToString(obj.runType)},
                    {"privatePort", obj.privatePort},
                    {"publicPort", obj.publicPort},
                    {"archive", obj.archive},
                    {"version", obj.version},
                    {"imageId", obj.imageId},
                    {"containerId", obj.containerId},
                    {"containerName", obj.containerName},
                    {"status", AppsStatusTypeToString(obj.status)},
                    {"enabled", obj.enabled},
                    {"description", obj.description},
                    {"dockerFile", obj.dockerFile},
                    {"lastStarted", Core::DateTimeUtils::ToISO8601(obj.lastStarted)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"environment", boost::json::value_from(obj.environment)},
                    {"dependencies", boost::json::value_from(obj.dependencies)},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_APPLICATION_H
