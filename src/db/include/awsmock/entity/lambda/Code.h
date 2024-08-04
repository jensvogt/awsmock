//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_CODE_H
#define AWSMOCK_DB_ENTITY_LAMBDA_CODE_H

// C++ includes
#include <sstream>
#include <string>
#include <vector>

// Poco includes
#include <Poco/JSON/Object.h>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/stdx.hpp>

namespace AwsMock::Database::Entity::Lambda {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;

    /**
     * @brief Lambda code entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Code {

        /**
         * Filename of the code. This is the filename of the base64 encoded ZIP file, which is saved in the
         * lambda directory, usually /home/awsmock/data/lambda. This file is loaded by the server during
         * startup. All lambdas, which are saved in the database and found in the lambda directory get
         * automatically started.
         *
         * In case the lambda does not exist and the lambda is created by the AWS CLI, zipFile contains the
         * base64 encoded lambda code.
         */
        std::string zipFile;

        /**
         * S3 bucket
         */
        std::string s3Bucket;

        /**
         * S3 key
         */
        std::string s3Key;

        /**
         * S3 version
         */
        std::string s3ObjectVersion;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param tag tag entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Code &tag);
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif// AWSMOCK_DB_ENTITY_LAMBDA_CODE_H
