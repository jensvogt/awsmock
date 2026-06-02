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

    /**
     * @brief Get log level request
     *
     * @code{.json}
     * {
     *   "names": ["s3", "sqs" ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetLogLevelRequest final : Common::BaseCounter<GetLogLevelRequest> {

        std::vector<std::string> names;

      private:

        friend GetLogLevelRequest tag_invoke(boost::json::value_to_tag<GetLogLevelRequest>, boost::json::value const &v) {
            GetLogLevelRequest r;
            if (Core::Json::AttributeExists(v, "names")) {
                for (const auto &item: v.at("names").as_array()) {
                    r.names.push_back(std::string(item.get_string()));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLogLevelRequest const &obj) {
            boost::json::array arr;
            for (const auto &ll: obj.names) {
                arr.emplace_back(boost::json::value_from(ll));
            }
            jv = {{"names", arr}};
        }
    };

}// namespace Awsmock::Dto::Module
