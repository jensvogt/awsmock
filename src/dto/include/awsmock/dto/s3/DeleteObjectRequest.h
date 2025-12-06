//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_OBJECT_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_OBJECT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct DeleteObjectRequest final : Common::BaseCounter<DeleteObjectRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Version ID
         */
        std::string versionId;

      private:

        friend DeleteObjectRequest tag_invoke(boost::json::value_to_tag<DeleteObjectRequest>, boost::json::value const &v) {
            DeleteObjectRequest r;
            r.bucket = Core::Json::GetStringValue(v, "Bucket");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.key = Core::Json::GetStringValue(v, "Key");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteObjectRequest const &obj) {
            jv = {
                    {"Bucket", obj.bucket},
                    {"VersionId", obj.versionId},
                    {"Key", obj.key},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_DELETE_OBJECT_REQUEST_H
