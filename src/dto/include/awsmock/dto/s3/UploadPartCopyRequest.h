//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H
#define AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct UploadPartCopyRequest final : Common::BaseCounter<UploadPartCopyRequest> {

        /**
         * Upload ID
         */
        std::string uploadId;

        /**
         * Part number
         */
        long partNumber;

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
         * Min byte from range
         */
        long min;

        /**
         * Max byte from range
         */
        long max;

      private:

        friend UploadPartCopyRequest tag_invoke(boost::json::value_to_tag<UploadPartCopyRequest>, boost::json::value const &v) {
            UploadPartCopyRequest r;
            r.uploadId = Core::Json::GetStringValue(v, "uploadId");
            r.partNumber = Core::Json::GetLongValue(v, "partNumber");
            r.sourceBucket = Core::Json::GetStringValue(v, "sourceBucket");
            r.sourceKey = Core::Json::GetStringValue(v, "sourceKey");
            r.targetBucket = Core::Json::GetStringValue(v, "targetBucket");
            r.targetKey = Core::Json::GetStringValue(v, "targetKey");
            r.min = Core::Json::GetLongValue(v, "min");
            r.max = Core::Json::GetLongValue(v, "max");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UploadPartCopyRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"uploadId", obj.uploadId},
                    {"partNumber", obj.partNumber},
                    {"sourceBucket", obj.sourceBucket},
                    {"sourceKey", obj.sourceKey},
                    {"targetBucket", obj.targetBucket},
                    {"targetKey", obj.targetKey},
                    {"min", obj.min},
                    {"max", obj.max},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H
