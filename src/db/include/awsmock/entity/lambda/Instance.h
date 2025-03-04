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

namespace AwsMock::Database::Entity::Lambda {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief Lambda instance status enum
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum LambdaInstanceStatus {
        InstanceIdle,
        InstanceRunning,
        InstanceFailed,
        InstanceUnknown
    };

    static std::map<LambdaInstanceStatus, std::string> LambdaInstanceStatusNames{
            {InstanceIdle, "Idle"},
            {InstanceRunning, "Running"},
            {InstanceFailed, "Failed"},
            {InstanceUnknown, "Unknown"},
    };

    [[maybe_unused]] static std::string LambdaInstanceStatusToString(LambdaInstanceStatus lambdaInstanceStatus) {
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
    struct Instance {

        /**
         * Instance ID, will be appended to the container name, in case of multiple instances.
         */
        std::string instanceId;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Container name
         */
        std::string containerName;

        /**
         * Host port
         */
        int hostPort;

        /**
         * Status
         */
        LambdaInstanceStatus status;

        /**
         * Created timestamp
         */
        system_clock::time_point created = system_clock::now();

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
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument();

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
        friend std::ostream &operator<<(std::ostream &os, const Instance &tag);
    };

}// namespace AwsMock::Database::Entity::Lambda

#endif// AWSMOCK_DB_ENTITY_LAMBDA_INSTANCE_H
