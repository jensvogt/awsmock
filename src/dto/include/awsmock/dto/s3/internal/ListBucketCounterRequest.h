//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct ListBucketCounterRequest final : Common::BaseCounter<ListBucketCounterRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize = 10;

        /**
         * Skip
         */
        long pageIndex = 0;

        /**
         * @brief List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListBucketCounterRequest tag_invoke(boost::json::value_to_tag<ListBucketCounterRequest>, boost::json::value const &v) {
            ListBucketCounterRequest r;
            r.region = Core::Json::GetStringValue(v, "region");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_LIST_BUCKET_REQUEST_H
