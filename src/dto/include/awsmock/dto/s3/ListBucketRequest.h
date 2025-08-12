//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 list bucket request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListBucketRequest final : Common::BaseCounter<ListBucketRequest> {

        /**
         * Name
         */
        std::string name;

        /**
         * List type
         */
        long listType{};

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

      private:

        friend ListBucketRequest tag_invoke(boost::json::value_to_tag<ListBucketRequest>, boost::json::value const &v) {
            ListBucketRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.listType = Core::Json::GetLongValue(v, "listType");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.delimiter = Core::Json::GetStringValue(v, "delimiter");
            r.encodingType = Core::Json::GetStringValue(v, "encodingType");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"listType", obj.listType},
                    {"prefix", obj.prefix},
                    {"delimiter", obj.delimiter},
                    {"encodingType", obj.encodingType},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_LIST_BUCKET_REQUEST_H
