//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_CODE_H
#define AWSMOCK_DB_ENTITY_LAMBDA_CODE_H

// C++ includes
#include <string>

// MongoDB includes
#include <awsmock/core/BsonUtils.h>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>


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
         * @brief Name of the ZIP file.
         *
         * @par
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
         * @brief S3 bucket for the lambda code
         */
        std::string s3Bucket;

        /**
         * @brief S3 key for the lambda code
         */
        std::string s3Key;

        /**
         * @brief S3 version of the lambda code
         */
        std::string s3ObjectVersion;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<bsoncxx::document::view> &mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
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
