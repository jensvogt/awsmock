//
// Created by vogje01 on 07/06/2023.
//

#pragma once

// C++ includes
#include <chrono>
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/lambda/Code.h>
#include <awsmock/entity/lambda/Environment.h>
#include <awsmock/entity/lambda/EphemeralStorage.h>
#include <awsmock/entity/lambda/EventSourceMapping.h>
#include <awsmock/entity/lambda/Instance.h>

namespace Awsmock::Database::Entity::Lambda {

    struct Lambda {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region name
         */
        std::string region;

        /**
         * User
         */
        std::string user;

        /**
         * Function
         */
        std::string function;

        /**
         * Runtime
         */
        std::string runtime;

        /**
         * Role
         */
        std::string role;

        /**
         * Handler
         */
        std::string handler;

        /**
         * Memory size in MB, Default: 128, Range: 128 - 10240 MB
         */
        long memorySize = 128;

        /**
         * Temporary dask space in MB, Default: 512, Range: 512 - 10240 MB
         */
        EphemeralStorage ephemeralStorage;

        /**
         * Size of the code in bytes, usually the size of the ZIP or JAR file.
         */
        long codeSize = 0;

        /**
         * Image ID
         */
        std::string imageId;

        /**
         * @brief Image size, as reported by the docker daemon
         */
        long imageSize = 0;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * @brief Container size in bytes, as reported by the docker daemon.
         *
         * @par
         * Usually this id the size of the root file system of the container, as reported by the docker daemon. The docker container
         * reports this size as 'SizeRootFs' in container JSON.
         */
        long containerSize = 0;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Timeout in seconds
         */
        int timeout = 3600;

        /**
         * Concurrency
         */
        long concurrency = 5;

        /**
         * Environment
         */
        Environment environment;

        /**
         * Code SHA256
         */
        std::string codeSha256;

        /**
         * Code
         */
        Code code;

        /**
         * Docker tag
         */
        std::string dockerTag;

        /**
         * Last function StartServer
         */
        system_clock::time_point lastStarted;

        /**
         * Last function invocation
         */
        system_clock::time_point lastInvocation;

        /**
         * Running instances
         */
        std::vector<Instance> instances;

        /**
         * Event sources
         */
        std::vector<EventSourceMapping> eventSources;

        /**
         * Number of invocations
         */
        long invocations = 0;

        /**
         * Average runtime in milliseconds
         */
        long avgDuration = 0;

        /**
         * Enabled flag
         */
        bool enabled = false;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Returns the existence of a lambda instance by ID
         *
         * @param instanceId instance ID
         * @return true if the lambda instance exists, otherwise false.
         */
        bool HasInstance(const std::string &instanceId);

        /**
         * @brief Returns an instance by instance ID
         *
         * @param instanceId instance ID
         * @return lambda instance.
         */
        Instance GetInstance(const std::string &instanceId);

        /**
         * @brief Remove an instance from the instance list.
         *
         * @par
         * The instance is identified by the instanceId.
         *
         * @param instance lambda instance
         */
        void RemoveInstance(const Instance &instance);

        /**
         * @brief Remove an instance from the instance list.
         *
         * @par
         * The instance is identified by the instanceId.
         *
         * @param instanceId ID of the instance
         */
        void RemoveInstance(const std::string &instanceId);

        /**
         * @brief Remove an instance from the instance list.
         *
         * @par
         * The instance is identified by the containerId.
         *
         * @param containerID ID of the instance container
         */
        void RemoveInstanceByContainerId(const std::string &containerID);

        /**
         * @brief Checks for any lambda instance with the state 'inactive'.
         *
         * @return true if any idle instance has been found.
         */
        bool HasIdleInstance();

        /**
         * @brief Returns an idle instance
         *
         * @return idle instance
         */
        Instance GetIdleInstance();

        /**
         * @brief Returns the count of idle instances
         *
         * @return number of running instances
         */
        long countIdleInstances();

        /**
         * @brief Returns the count of running instances
         *
         * @return number of running instances
         */
        long countRunningInstances();

        /**
         * @brief Returns the total number of invocations for all instances
         *
         * @return total number of invocations for all instances
         */
        long getTotalInvocations();

        /**
         * @brief Returns the average duration across all instances (weighted by invocation count).
         *
         * @return average duration in milliseconds, or 0 if there are no invocations
         */
        long getAvgDuration();

        /**
         * @brief Checks whether an event source with the given ARN exists already.
         *
         * @param eventSourceArn event source ARN
         * @return true if an event source with the given ARN exists.
         */
        [[nodiscard]] bool HasEventSource(const std::string &eventSourceArn) const;

        /**
         * @brief Returns an event source with the given ARN.
         *
         * @param eventSourceArn event source ARN
         * @return true if an event source with the given ARN exists.
         */
        [[nodiscard]] EventSourceMapping GetEventSource(const std::string &eventSourceArn) const;

        /**
         * @brief Checks whether tags with the given tags key exist.
         *
         * @param key key of the tags
         * @return true if tags with the given key exist.
         */
        [[nodiscard]] bool HasTag(const std::string &key) const;

        /**
         * @brief Returns a given tags value by key
         *
         * @param key name of the tag
         * @return found notification or notifications.end().
         */
        [[nodiscard]] std::string GetTagValue(const std::string &key) const;

        /**
         * @brief Set the last invocation time for an instance
         *
         * @param instanceId instance ID
         * @param host current host name
         * @param port current port
         */
        void SetInstanceHostPort(const std::string &instanceId, const std::string &host, int port);

        /**
         * @brief Set the last invocation time for an instance
         *
         * @param instanceId instance ID
         */
        void SetInstanceLastInvocation(const std::string &instanceId);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as a MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
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
         * @param os output stream
         * @param lambda lambda entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Lambda &lambda);
    };

    typedef std::vector<Lambda> LambdaList;

    inline bool Lambda::HasIdleInstance() {
        return std::ranges::find_if(instances, [](const Instance &i) {
                   return i.status == idle;
               }) != instances.end();
    }

    inline Instance Lambda::GetIdleInstance() {
        const auto it = std::ranges::find_if(instances, [](const Instance &i) {
            return i.status == idle;
        });
        if (it != instances.end()) {
            return *it;
        }
        return {};
    }

    inline long Lambda::countIdleInstances() {
        return std::ranges::count_if(instances, [](const Instance &i) {
            return i.status == idle;
        });
    }

    inline long Lambda::countRunningInstances() {
        return std::ranges::count_if(instances, [](const Instance &i) {
            return i.status == running;
        });
    }

    inline long Lambda::getTotalInvocations() {
        return std::ranges::fold_left(instances, 0L, [](const long acc, const Instance &i) {
            return acc + i.invocations;
        });
    }

    inline long Lambda::getAvgDuration() {
        const long totalInvocations = getTotalInvocations();
        if (totalInvocations == 0) return 0;
        const double weightedSum = std::ranges::fold_left(instances, 0.0, [](const double acc, const Instance &i) {
            return acc + i.avgDuration * static_cast<double>(i.invocations);
        });
        return static_cast<long>(weightedSum / static_cast<double>(totalInvocations));
    }

}// namespace Awsmock::Database::Entity::Lambda
