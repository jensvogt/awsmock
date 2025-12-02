//
// Created by vogje01 on 9/30/24.
//

#ifndef AWSMOCK_DTO_COMMON_SORT_COLUMN_H
#define AWSMOCK_DTO_COMMON_SORT_COLUMN_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Common {

    struct SortColumn final : BaseCounter<SortColumn> {

        /**
         * Columns
         */
        std::string column{};

        /**
         * Direction, MongoDB style, 1: ascending, -1 descending
         */
        int sortDirection = 1;

      private:

        friend SortColumn tag_invoke(boost::json::value_to_tag<SortColumn>, boost::json::value const &v) {
            SortColumn r;
            r.column = v.at("column").as_string();
            r.sortDirection = static_cast<int>(v.at("sortDirection").as_int64());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SortColumn const &obj) {
            jv = {
                    {"column", obj.column},
                    {"sortDirection", obj.sortDirection},
            };
        }
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SORT_COLUMN_H
