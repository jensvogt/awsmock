//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H
#define AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Get object request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetObjectRequest final : Common::BaseCounter<GetObjectRequest> {

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

        /**
         * Range minimum
         */
        long min;

        /**
         * Range maximum
         */
        long max;

      private:

        friend GetObjectRequest tag_invoke(boost::json::value_to_tag<GetObjectRequest>, boost::json::value const &v) {
            GetObjectRequest r;
            r.bucket = v.at("bucket").as_string();
            r.key = v.at("key").as_string();
            r.versionId = v.at("versionId").as_string();
            r.min = v.at("min").as_int64();
            r.max = v.at("max").as_int64();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"versionId", obj.versionId},
                    {"min", obj.min},
                    {"max", obj.max},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H
