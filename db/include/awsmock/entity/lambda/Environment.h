//
// Created by vogje01 on 03/09/2023.
//

#pragma once

// C++ includes
#include <map>
#include <string>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace Awsmock::Database::Entity::Lambda {

    /**
     * @brief Lambda environment entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Environment {

        /**
         * Variables
         */
        std::map<std::string, std::string> variables;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult database view.
         */
        void FromDocument(const view_or_value<view, value> &mResult);

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Lambda"};
    };

}// namespace Awsmock::Database::Entity::Lambda
