//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief List object version request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListObjectVersionsRequest final : Common::BaseCounter<ListObjectVersionsRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Delimiter
         */
        std::string delimiter;

        /**
         * Encoding type
         */
        std::string encodingType;

        /**
         * Page size
         */
        long pageSize{};

        /**
         * Maximal number of keys
         */
        long maxKeys{};

        /**
         * Version ID marker
         */
        std::string versionIdMarker;

      private:

        friend ListObjectVersionsRequest tag_invoke(boost::json::value_to_tag<ListObjectVersionsRequest>, boost::json::value const &v) {
            ListObjectVersionsRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.delimiter = Core::Json::GetStringValue(v, "delimiter");
            r.encodingType = Core::Json::GetStringValue(v, "encodingType");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.maxKeys = Core::Json::GetLongValue(v, "maxKeys");
            r.versionIdMarker = Core::Json::GetStringValue(v, "versionIdMarker");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectVersionsRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"prefix", obj.prefix},
                    {"delimiter", obj.delimiter},
                    {"encodingType", obj.encodingType},
                    {"pageSize", obj.pageSize},
                    {"maxKeys", obj.maxKeys},
                    {"versionIdMarker", obj.versionIdMarker},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H
