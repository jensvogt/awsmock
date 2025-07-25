//
// Created by vogje01 on 9/30/24.
//

#ifndef AWSMOCK_DATABASE_COMMON_SORT_COLUMN_H
#define AWSMOCK_DATABASE_COMMON_SORT_COLUMN_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Database {

    struct SortColumn /*final : Entity::Common::BaseEntity<SortColumn>*/ {

        /**
         * Columns
         */
        std::string column;

        /**
         * Direction, MongoDB style, 1: ascending, -1 descending
         */
        int sortDirection;

        /**
         * @brief Convert from a BSON object
         *
         * @param document BSON document
         */
        void FromDocument(const std::optional<view> &document) {

            column = Core::Bson::BsonUtils::GetStringValue(document, "column");
            sortDirection = Core::Bson::BsonUtils::GetIntValue(document, "sortDirection");
        }

        /**
         * @brief Convert to JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const {

            try {
                bsoncxx::builder::basic::document document;
                Core::Bson::BsonUtils::SetStringValue(document, "column", column);
                Core::Bson::BsonUtils::SetIntValue(document, "sortDirection", sortDirection);

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_DATABASE_COMMON_SORT_COLUMN_H
