//
// Created by vogje01 on 03/09/2023.
//

#pragma once

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMOck includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/LambdaRuntimeStatus.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/lambda/RuntimeStatus.h>

namespace Awsmock::Database::Entity::Lambda {

    /**
     * @brief Lambda instance entity
     *
     * @par
     * An instance is an invocation of the lambda function. Each invocation (until max. concurrency) will start a new instance of the lambda function. The status will be
     * set 'RUNNING'. Default status is 'IDLE'. Each lambda runs in its own docker container having a distinguished name like 'lambda-function-s7654d'. Private ports are
     * always the same (usually 8080), whereas the public port will be a random integer between 32.768 and 65.535. The public port is chosen between this two numbers taken
     * into account that the port must be free on the local machine. Otherwise, a new random number is selected in the given range.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Instance final : Common::BaseEntity<Instance> {

        /**
         * Instance ID, will be appended to the container name, in case of multiple instances.
         */
        std::string instanceId{};

        /**
         * Container ID
         */
        std::string containerId{};

        /**
         * Container name
         */
        std::string containerName{};

        /**
         * Container public
         */
        int publicPort{};

        /**
         * Container public
         */
        int privatePort = 8080;

        /**
         * Host name
         */
        std::string hostName{};

        /**
         * Status
         */
        RuntimeStatus status = unknown;

        /**
         * @brief Last invocation timestamp
         */
        long invocations{};

        /**
         * @brief Average invocation duration
         */
        double avgDuration{};

        /**
         * @brief Last started timestamp
         */
        system_clock::time_point lastStart{};

        /**
         * @brief Last invocation timestamp
         */
        system_clock::time_point lastInvocation{};

        /**
         * @brief Last stopped timestamp
         */
        system_clock::time_point lastStop{};

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]]
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as a MongoDB document.
         */
        [[nodiscard]]
        view_or_value<view, value> ToDocument() const override;
    };

}// namespace Awsmock::Database::Entity::Lambda
