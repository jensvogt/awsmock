//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Clean infrastructure request
     *
     * Example:
     * @code{.json}
     * {
     *   "modules": ["string", ...],
     *   "onlyObjects": bool,
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CleanInfrastructureRequest final : Common::BaseCounter<CleanInfrastructureRequest> {

        /**
         * Modules
         */
        std::vector<std::string> modules;

        /**
         * Only objects
         */
        bool onlyObjects{};

        /**
         * Pretty print
         */
        bool prettyPrint{};

      private:

        friend CleanInfrastructureRequest tag_invoke(boost::json::value_to_tag<CleanInfrastructureRequest>, boost::json::value const &v) {
            CleanInfrastructureRequest r;
            r.modules = boost::json::value_to<std::vector<std::string>>(v.at("modules"));
            r.onlyObjects = Core::Json::GetBoolValue(v, "onlyObjects");
            r.prettyPrint = Core::Json::GetBoolValue(v, "prettyPrint");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CleanInfrastructureRequest const &obj) {
            jv = {
                    {"modules", boost::json::value_from(obj.modules)},
                    {"onlyObjects", obj.onlyObjects},
                    {"prettyPrint", obj.prettyPrint},
            };
        }
    };

}// namespace AwsMock::Dto::Module

#endif//AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H
