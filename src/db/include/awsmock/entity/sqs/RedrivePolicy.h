//
// Created by vogje01 on 12/10/23.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_REDRIVE_POLICY_H
#define AWSMOCK_DB_ENTITY_SQS_REDRIVE_POLICY_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::SQS {

    /**
     * @brief SQS queue re-drive policy entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RedrivePolicy final : Common::BaseEntity<RedrivePolicy> {

        /**
         * Dead letter queue target ARN
         */
        std::string deadLetterTargetArn;

        /**
         * Maximal number of retries before the message will be sent to the DQL
         */
        long maxReceiveCount = 0;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

        void FromJson(const std::string &json) {
            value jsonObject = bsoncxx::from_json(json);
            FromDocument(jsonObject);
        };
    };

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_REDRIVE_POLICY_H
