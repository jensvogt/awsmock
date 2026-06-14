//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_MODEL_PLATFORM_H
#define AWSMOCK_DTO_DOCKER_MODEL_PLATFORM_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Docker {

    /**
     * @brief Docker platform
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Platform final : Common::BaseObject<Platform> {

        /**
         * Platform name
         */
        std::string name;

      private:

        friend Platform tag_invoke(boost::json::value_to_tag<Platform>, boost::json::value const &v) {
            Platform r;
            r.name = Core::Json::GetStringValue(v, "Name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Platform const &obj) {
            jv = {
                    {"Name", obj.name},
            };
        }
    };

}// namespace Awsmock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_MODEL_PLATFORM_H
