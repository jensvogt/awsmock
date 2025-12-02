//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SSM_PARAMETER_TOPIC_H
#define AWSMOCK_DB_ENTITY_SSM_PARAMETER_TOPIC_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SSM {

    using std::chrono::system_clock;

    /**
     * ssm parameter entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Parameter {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Parameter name
         */
        std::string parameterName;

        /**
         * Value
         *
         * @par
         * In case the KMS key ID is not null, this attribute contains the base64 encoded and encrypted paramezter value.
         */
        std::string parameterValue;

        /**
         * Description
         */
        std::string description;

        /**
         * Parameter type
         */
        std::string type;

        /**
         * Tier
         */
        std::string tier;

        /**
         * Version
         */
        int version;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * KMS key ARN
         *
         * @par
         * If this is not null, the parameter will be encrypted and base64 encoded.
         */
        std::string kmsKeyArn;

        /**
         * Topic tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
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
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Parameter &q);
    };

    typedef std::vector<Parameter> ParameterList;

}// namespace AwsMock::Database::Entity::SSM

#endif// AWSMOCK_DB_ENTITY_SSM_PARAMETER_TOPIC_H
