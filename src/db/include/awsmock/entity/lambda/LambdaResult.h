//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_RESULT_H
#define AWSMOCK_DB_ENTITY_LAMBDA_RESULT_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/lambda/Instance.h>

namespace AwsMock::Database::Entity::Lambda {

    using std::chrono::system_clock;

    struct LambdaResult final : Common::BaseEntity<LambdaResult> {

        /**
         * Lambda name
         */
        std::string lambdaName;

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Request body
         */
        std::string requestBody;

        /**
         * Response body
         */
        std::string responseBody;

        /**
         * Log messages
         */
        std::string logMessages;

        /**
         * Runtime
         */
        std::string runtime;

        /**
         * Instance ID
         */
        std::string instanceId;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Status code
         */
        std::string httpStatusCode;

        /**
         * Status code
         */
        boost::beast::http::status status;

        /**
         * Lambda status
         */
        LambdaInstanceStatus lambdaStatus = InstanceUnknown;

        /**
         * Duration in milliseconds
         */
        long duration;

        /**
         *
         * Last function StartServer
         */
        system_clock::time_point timestamp = system_clock::now();

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
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif//AWSMOCK_DB_ENTITY_LAMBDA_H
