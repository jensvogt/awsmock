//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct ListObjectCounterRequest final : Common::BaseCounter<ListObjectCounterRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size, default: 10
         */
        long pageSize = 10;

        /**
         * Page index
         */
        long pageIndex = 0;

        /**
         * @brief List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListObjectCounterRequest tag_invoke(boost::json::value_to_tag<ListObjectCounterRequest>, boost::json::value const &v) {
            ListObjectCounterRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void
        tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_LIST_OBJECT_REQUEST_H
