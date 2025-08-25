//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_VERSION_H
#define AWSMOCK_DTO_S3_OBJECT_VERSION_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/s3/model/Owner.h>
#include <awsmock/dto/s3/model/RestoreStatus.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief S3 object versioning DTO
     *
     * @par
     * Only used in case the bucket is versioned.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ObjectVersion final : Common::BaseCounter<ObjectVersion> {

        /**
         * Key
         */
        std::string key;

        /**
         * ETag
         */
        std::string eTag;

        /**
         * VersionId
         */
        std::string versionId;

        /**
         * Storage class
         */
        std::string storageClass;

        /**
         * Checksum algorithm
         */
        std::vector<std::string> checksumAlgorithms;

        /**
         * Is latest
         */
        bool isLatest{};

        /**
         * Size
         */
        long size{};

        /**
         * Owner
         */
        Owner owner;

        /**
         * Restore status
         */
        RestoreStatus restoreStatus;

        /**
         * Is latest
         */
        system_clock::time_point lastModified;

      private:

        friend ObjectVersion tag_invoke(boost::json::value_to_tag<ObjectVersion>, boost::json::value const &v) {
            ObjectVersion r;
            r.key = Core::Json::GetStringValue(v, "Key");
            r.eTag = Core::Json::GetStringValue(v, "ETag");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.storageClass = Core::Json::GetStringValue(v, "StorageClass");
            r.isLatest = Core::Json::GetLongValue(v, "IsLatest");
            r.size = Core::Json::GetLongValue(v, "Size");
            r.lastModified = Core::Json::GetDatetimeValue(v, "LastModified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ObjectVersion const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", obj.key},
                    {"ETag", obj.eTag},
                    {"VersionId", obj.versionId},
                    {"StorageClass", obj.storageClass},
                    {"IsLatest", obj.isLatest},
                    {"Size", obj.size},
                    {"LastModified", Core::DateTimeUtils::ToISO8601(obj.lastModified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_VERSION_H
