//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_SUBSCRIPTION_H
#define AWSMOCK_DB_ENTITY_SNS_SUBSCRIPTION_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Database::Entity::SNS {

    /**
     * SNS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Subscription {

        /**
         * Protocol
         */
        std::string protocol;

        /**
         * Endpoint
         */
        std::string endpoint;

        /**
         * Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(const std::optional<bsoncxx::document::view_or_value> &mResult);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @param os output stream
         * @param m subscription entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Subscription &m);
    };

    typedef std::vector<Subscription> SubscriptionList;

}// namespace AwsMock::Database::Entity::SNS
#endif// AWSMOCK_DB_ENTITY_SNS_SUBSCRIPTION_H
