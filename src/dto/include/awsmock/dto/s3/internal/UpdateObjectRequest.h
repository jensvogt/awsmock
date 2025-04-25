//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H
#define AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct UpdateObjectRequest final : Common::BaseCounter<UpdateObjectRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

      private:

        friend UpdateObjectRequest tag_invoke(boost::json::value_to_tag<UpdateObjectRequest>, boost::json::value const &v) {

            UpdateObjectRequest r;
            r.region = v.at("region").as_string();
            r.bucket = v.at("bucket").as_string();
            r.key = v.at("key").as_string();
            r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateObjectRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_UPDATE_OBJECT_REQUEST_H
