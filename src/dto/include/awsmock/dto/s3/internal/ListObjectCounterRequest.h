//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct ListObjectCounterRequest final : Common::BaseCounter<ListObjectCounterRequest> {

        /**
         * Region
         */
        std::string region;

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
        int pageSize = 10;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * @brief List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListObjectCounterRequest tag_invoke(boost::json::value_to_tag<ListObjectCounterRequest>, boost::json::value const &v) {
            ListObjectCounterRequest r;
            r.region = v.at("region").as_string();
            r.prefix = v.at("prefix").as_string();
            r.pageSize = v.at("pageSize").as_int64();
            r.pageIndex = v.at("pageIndex").as_int64();

            // Sort columns
            for (const auto &sc: v.at("sortColumns").as_array()) {
                Common::SortColumn sortColumn;
                sortColumn.column = sc.at("column").as_string();
                sortColumn.sortDirection = sc.at("sortDirection").as_int64();
                r.sortColumns.emplace_back(sortColumn);
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectCounterRequest const &obj) {
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

#endif//AWSMOCK_DTO_S3_LIST_OBJECT_REQUEST_H
