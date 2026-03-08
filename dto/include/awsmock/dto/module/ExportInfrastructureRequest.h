//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/module/model/ExportType.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Export infrastructure request
     *
     * Example:
     * @code{.json}
     * {
     *   "modules": ["string", ...],
     *   "onlyObjects": bool,
     *   "prettyPrint": bool,
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExportInfrastructureRequest final : Common::BaseCounter<ExportInfrastructureRequest> {

        /**
         * Pretty print, default: true
         */
        bool prettyPrint = true;

        /**
         * Clean infrastructure first, default: false
         */
        bool cleanFirst = false;

        /**
         * Modules
         */
        std::vector<std::string> modules;

        /**
         * Export type
         */
        ExportType exportType = BOTH;

        bool IsInfrastructure() const {
            return exportType == INFRA_STRUCTURE || exportType == BOTH;
        }

        bool IsObjects() const {
            return exportType == OBJECTS || exportType == BOTH;
        }

      private:

        friend ExportInfrastructureRequest tag_invoke(boost::json::value_to_tag<ExportInfrastructureRequest>, boost::json::value const &v) {
            ExportInfrastructureRequest r{};
            r.prettyPrint = Core::Json::GetBoolValue(v, "prettyPrint");
            r.cleanFirst = Core::Json::GetBoolValue(v, "cleanFirst");
            r.exportType = ExportTypeFromString(Core::Json::GetStringValue(v, "exportType"));
            if (Core::Json::AttributeExists(v, "modules")) {
                r.modules = boost::json::value_to<std::vector<std::string>>(v.at("modules"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExportInfrastructureRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"prettyPrint", obj.prettyPrint},
                    {"cleanFirst", obj.cleanFirst},
                    {"exportType", ExportTypeToString(obj.exportType)},
                    {"modules", boost::json::value_from(obj.modules)},
            };
        }
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
