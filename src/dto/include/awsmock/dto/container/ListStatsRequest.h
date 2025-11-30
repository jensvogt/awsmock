//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_LIST_STATS_RESPONSE_H
#define AWSMOCK_DTO_CONTAINER_LIST_STATS_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/JsonUtils.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List container statistics request
     *
     * @code{.json}
     * [ name
     *   ...
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListStatsRequest final : Common::BaseCounter<ListStatsRequest> {

        /**
         * Container name list
         */
        std::vector<std::string> names;

      private:

        friend ListStatsRequest tag_invoke(boost::json::value_to_tag<ListStatsRequest>, boost::json::value const &v) {
            ListStatsRequest r;
            r.names = boost::json::value_to<std::vector<std::string>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListStatsRequest const &obj) {
            jv = {
                    {boost::json::value_from(obj.names)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_LIST_STATS_RESPONSE_H
