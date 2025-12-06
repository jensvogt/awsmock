//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H
#define AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct CopyObjectRequest final : Common::BaseCounter<CopyObjectRequest> {

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

        friend CopyObjectRequest tag_invoke(boost::json::value_to_tag<CopyObjectRequest>, boost::json::value const &v) {
            CopyObjectRequest r;
            r.sourceBucket = Core::Json::GetStringValue(v, "sourceBucket");
            r.sourceKey = Core::Json::GetStringValue(v, "sourceKey");
            r.targetBucket = Core::Json::GetStringValue(v, "targetBucket");
            r.targetKey = Core::Json::GetStringValue(v, "targetKey");
            r.sourceKey = Core::Json::GetStringValue(v, "sourceKey");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CopyObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"sourceBucket", obj.sourceBucket},
                    {"targetBucket", obj.targetBucket},
                    {"targetKey", obj.targetKey},
                    {"sourceKey", obj.sourceKey},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H
