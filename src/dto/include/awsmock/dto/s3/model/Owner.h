//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_OWNER_H
#define AWSMOCK_DTO_S3_OWNER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 object owner DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Owner final : Common::BaseCounter<Owner> {

        /**
         * ID
         */
        std::string id;

        /**
         * Display name
         */
        std::string displayName;

      private:

        friend Owner tag_invoke(boost::json::value_to_tag<Owner>, boost::json::value const &v) {

            Owner r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.displayName = Core::Json::GetStringValue(v, "displayName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Owner const &obj) {
            jv = {
                    {"id", obj.id},
                    {"displayName", boost::json::value_from(obj.displayName)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OWNER_H
