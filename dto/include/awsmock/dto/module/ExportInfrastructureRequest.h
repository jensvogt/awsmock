//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

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
         * Include objects, default: false
         */
        bool includeObjects = false;

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

      private:

        friend ExportInfrastructureRequest tag_invoke(boost::json::value_to_tag<ExportInfrastructureRequest>, boost::json::value const &v) {
            ExportInfrastructureRequest r;
            r.includeObjects = Core::Json::GetBoolValue(v, "includeObjects");
            r.prettyPrint = Core::Json::GetBoolValue(v, "prettyPrint");
            r.cleanFirst = Core::Json::GetBoolValue(v, "cleanFirst");
            if (Core::Json::AttributeExists(v,"modules")) {
                r.modules = boost::json::value_to<std::vector<std::string>>(v.at("modules"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExportInfrastructureRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"includeObjects", obj.includeObjects},
                    {"prettyPrint", obj.prettyPrint},
                    {"cleanFirst", obj.cleanFirst},
                    {"modules", boost::json::value_from(obj.modules)},
            };
        }
    };

}// namespace AwsMock::Dto::Module

#endif//AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
