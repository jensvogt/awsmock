//
// Created by vogje01 on 8/2/26.
//

#pragma once

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::S3 {

    /**
     * @brief AWS S3 object version counter DTO.
     *
     * @par
     * Used only internally by the frontend, to show all stored versions of a single object key.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ObjectVersionCounter final : Common::BaseObject<ObjectVersionCounter> {

        /**
         * Database OID
         */
        std::string oid;

        /**
         * Object key
         */
        std::string key;

        /**
         * Object version ID
         */
        std::string versionId;

        /**
         * S3 storage class
         */
        std::string storageClass;

        /**
         * Object size in bytes
         */
        long size{};

        /**
         * True, in case this is the most recently modified version of the key
         */
        bool isLatest{};

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend ObjectVersionCounter tag_invoke(boost::json::value_to_tag<ObjectVersionCounter>, boost::json::value const &v) {
            ObjectVersionCounter r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.key = Core::Json::GetStringValue(v, "key");
            r.versionId = Core::Json::GetStringValue(v, "versionId");
            r.storageClass = Core::Json::GetStringValue(v, "storageClass");
            r.size = Core::Json::GetLongValue(v, "size");
            r.isLatest = v.at("isLatest").as_bool();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ObjectVersionCounter const &obj) {
            jv = {
                    {"oid", obj.oid},
                    {"key", obj.key},
                    {"versionId", obj.versionId},
                    {"storageClass", obj.storageClass},
                    {"size", obj.size},
                    {"isLatest", obj.isLatest},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };
}// namespace Awsmock::Dto::S3
