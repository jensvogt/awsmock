//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_CONTAINER_LIST_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_CONTAINER_LIST_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List container request
     *
     * @code{.json}
     * [ name
     *   ...
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListContainerRequest final : Common::BaseCounter<ListContainerRequest> {

        /**
         * Container list
         */
        std::vector<std::string> names;

        /**
         * Prefix
         */
        std::string prefix;

      private:

        friend ListContainerRequest tag_invoke(boost::json::value_to_tag<ListContainerRequest>, boost::json::value const &v) {
            ListContainerRequest r;
            if (v.is_array()) {
                r.names = boost::json::value_to<std::vector<std::string>>(v);
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListContainerRequest const &obj) {
            jv = {
                    {boost::json::value_from(obj.names)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_CONTAINER_LIST_CONTAINER_RESPONSE_H
