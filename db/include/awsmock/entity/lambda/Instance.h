//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_INSTANCE_H
#define AWSMOCK_DB_ENTITY_LAMBDA_INSTANCE_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMOck includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::Lambda {

    /**
     * @brief Lambda instance status enum
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum LambdaInstanceStatus {
        InstanceIdle,
        InstanceRunning,
        InstanceSuccess,
        InstanceFailed,
        InstanceUnknown
    };

    static std::map<LambdaInstanceStatus, std::string> LambdaInstanceStatusNames{
            {InstanceIdle, "Idle"},
            {InstanceRunning, "Running"},
            {InstanceSuccess, "Success"},
            {InstanceFailed, "Failed"},
            {InstanceUnknown, "Unknown"},
    };

    [[maybe_unused]] static std::string LambdaInstanceStatusToString(const LambdaInstanceStatus &lambdaInstanceStatus) {
        return LambdaInstanceStatusNames[lambdaInstanceStatus];
    }

    [[maybe_unused]] static LambdaInstanceStatus LambdaInstanceStatusFromString(const std::string &lambdaInstanceStatus) {
        for (auto &[fst, snd]: LambdaInstanceStatusNames) {
            if (snd == lambdaInstanceStatus) {
                return fst;
            }
        }
        return InstanceUnknown;
    }

    /**
     * @brief Lambda instance entity
     *
     * @par
     * A instance is a invocation of the lambda function. Each invocation (until max. concurrency) will start a new instance of the lambda function. The status will be
     * set 'RUNNING'. Default status is 'IDLE'.
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
        LambdaInstanceStatus status = InstanceUnknown;

        /**
         * Last invocation timestamp
         */
        system_clock::time_point lastInvocation;

        /**
         * Created timestamp
         */
        system_clock::time_point created = system_clock::now();

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif// AWSMOCK_DB_ENTITY_LAMBDA_INSTANCE_H
