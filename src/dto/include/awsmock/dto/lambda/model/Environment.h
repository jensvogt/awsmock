//
// Created by vogje01 on 21/06/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H
#define AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/Error.h>

namespace AwsMock::Dto::Lambda {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    struct EnvironmentVariables {

        /**
         * @brief Environment variables
         */
        std::map<std::string, std::string> variables;

        /**
         * Error
         */
        Error error;

        /**
         * @brief Convert to a JSON object
         *
         * @return DTO as BSON document
         */
        view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EnvironmentVariables &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H
