//
// Created by vogje01 on 9/30/24.
//

#ifndef AWSMOCK_CORE_SORT_COLUMN_H
#define AWSMOCK_CORE_SORT_COLUMN_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Core {

    struct SortColumn {

        /**
         * Columns
         */
        std::string column;

        /**
         * Direction, MongoDB style, 1: ascending, -1 descending
         */
        int sortDirection;

        /**
         * @brief Convert from BSON object
         *
         * @param document BSON document
         */
        void FromDocument(const std::optional<view> &document);

        /**
         * @brief Convert to JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SORT_COLUMN_H
