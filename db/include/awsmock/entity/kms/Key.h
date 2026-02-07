//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_KMS_KEY_H
#define AWSMOCK_DB_ENTITY_KMS_KEY_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>


// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::KMS {

    /**
     * @brief KMS key entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Key {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Key usage
         */
        std::string keyUsage;

        /**
         * Key specification
         */
        std::string keySpec;

        /**
         * Key state
         */
        std::string keyState;

        /**
         * Key state
         */
        std::string origin;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Description
         */
        std::string description;

        /**
         * Key material AES 256 (hex-encoded)
         */
        std::string aes256Key;

        /**
         * IV (hex-encoded)
         */
        std::string aes256Iv;

        /**
         * RSA private key (hex-encoded)
         */
        std::string rsaPrivateKey;

        /**
         * RSA public key (hex-encoded)
         */
        std::string rsaPublicKey;

        /**
         * Key material HMAC 224 (hex-encoded)
         */
        std::string hmac224Key;

        /**
         * Key material HMAC 256 (hex-encoded)
         */
        std::string hmac256Key;

        /**
         * Key material HMAC 384 (hex-encoded)
         */
        std::string hmac384Key;

        /**
         * Key material HMAC 256 (hex-encoded)
         */
        std::string hmac512Key;

        /**
         * Topic tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Pending window in days
         */
        long pendingWindowInDays;

        /**
         * Scheduled deletion datetime
         */
        system_clock::time_point scheduledDeletion = system_clock::now();

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, const Key &k);
    };

    typedef std::vector<Key> KeyList;

}// namespace AwsMock::Database::Entity::KMS

#endif// AWSMOCK_DB_ENTITY_KMS_KEY_H
