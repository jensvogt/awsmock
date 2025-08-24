//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_MOVE_OBJECT_REQUEST_H
#define AWSMOCK_CORE_DTO_MOVE_OBJECT_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Move object request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MoveObjectRequest final : Common::BaseCounter<MoveObjectRequest> {

        /**
         * Source bucket
         */
        std::string sourceBucket;

        /**
         * Source key
         */
        std::string sourceKey;

        /**
         * Target bucket
         */
        std::string targetBucket;

        /**
         * Target key
         */
        std::string targetKey;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

      private:

        friend MoveObjectRequest tag_invoke(boost::json::value_to_tag<MoveObjectRequest>, boost::json::value const &v) {
            MoveObjectRequest r;
            r.sourceBucket = Core::Json::GetStringValue(v, "sourceBucket");
            r.sourceKey = Core::Json::GetStringValue(v, "sourceKey");
            r.targetBucket = Core::Json::GetStringValue(v, "targetBucket");
            r.targetKey = Core::Json::GetStringValue(v, "targetKey");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MoveObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"sourceBucket", obj.sourceBucket},
                    {"sourceKey", obj.sourceKey},
                    {"targetBucket", obj.targetBucket},
                    {"targetKey", obj.targetKey},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_MOVE_OBJECT_REQUEST_H
