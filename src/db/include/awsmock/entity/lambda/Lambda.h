//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_H
#define AWSMOCK_DB_ENTITY_LAMBDA_H

// C++ includes
#include <chrono>
#include <map>
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

// AwsMock includes
#include <awsmock/entity/lambda/Code.h>
#include <awsmock/entity/lambda/Environment.h>
#include <awsmock/entity/lambda/EphemeralStorage.h>
#include <awsmock/entity/lambda/EventSourceMapping.h>
#include <awsmock/entity/lambda/Instance.h>
#include <awsmock/repository/S3Database.h>

namespace AwsMock::Database::Entity::Lambda {

    using bsoncxx::to_json;
    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief Lambda entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum LambdaState {
        Pending,
        Active,
        Inactive,
        Failed
    };

    static std::map<LambdaState, std::string> LambdaStateNames{
            {Pending, "Pending"},
            {Active, "Active"},
            {Inactive, "Inactive"},
            {Failed, "Failed"},
    };

    [[maybe_unused]] static std::string LambdaStateToString(LambdaState lambdaState) {
        return LambdaStateNames[lambdaState];
    }

    [[maybe_unused]] static LambdaState LambdaStateFromString(const std::string &lambdaState) {
        for (auto &[fst, snd]: LambdaStateNames) {
            if (snd == lambdaState) {
                return fst;
            }
        }
        return LambdaState::Inactive;
    }

    enum LambdaStateReasonCode {
        Idle,
        Creating,
        Restoring,
        EniLimitExceeded,
        InsufficientRolePermissions,
        InvalidConfiguration,
        InternalError,
        SubnetOutOfIPAddresses,
        InvalidSubnet,
        InvalidSecurityGroup,
        ImageDeleted,
        ImageAccessDenied,
        InvalidImage,
        KMSKeyAccessDenied,
        KMSKeyNotFound,
        InvalidStateKMSKey,
        DisabledKMSKey,
        EFSIOError,
        EFSMountConnectivityError,
        EFSMountFailure,
        EFSMountTimeout,
        InvalidRuntime,
        InvalidZipFileException,
        FunctionError
    };

    static std::map<LambdaStateReasonCode, std::string> LambdaStateReasonCodeNames{
            {Idle, "Idle"},
            {Creating, "Creating"},
            {Restoring, "Restoring"},
            {EniLimitExceeded, "EniLimitExceeded"},
            {InsufficientRolePermissions, "InsufficientRolePermissions"},
            {InvalidConfiguration, "InvalidConfiguration"},
            {InternalError, "InternalError"},
            {SubnetOutOfIPAddresses, "SubnetOutOfIPAddresses"},
            {InvalidSubnet, "InvalidSubnet"},
            {InvalidSecurityGroup, "InvalidSecurityGroup"},
            {ImageDeleted, "ImageDeleted"},
            {ImageAccessDenied, "ImageAccessDenied"},
            {InvalidImage, "InvalidImage"},
            {KMSKeyAccessDenied, "KMSKeyAccessDenied"},
            {KMSKeyNotFound, "KMSKeyNotFound"},
            {InvalidStateKMSKey, "InvalidStateKMSKey"},
            {DisabledKMSKey, "DisabledKMSKey"},
            {EFSIOError, "EFSIOError"},
            {EFSMountConnectivityError, "EFSMountConnectivityError"},
            {EFSMountFailure, "EFSMountFailure"},
            {EFSMountTimeout, "EFSMountTimeout"},
            {InvalidRuntime, "InvalidRuntime"},
            {InvalidZipFileException, "InvalidZipFileException"},
            {FunctionError, "FunctionError"},
    };

    [[maybe_unused]] static std::string LambdaStateReasonCodeToString(LambdaStateReasonCode lambdaStateReasonCode) {
        return LambdaStateReasonCodeNames[lambdaStateReasonCode];
    }

    [[maybe_unused]] static LambdaStateReasonCode LambdaStateReasonCodeFromString(const std::string &lambdaStateReasonCode) {
        for (auto &[fst, snd]: LambdaStateReasonCodeNames) {
            if (snd == lambdaStateReasonCode) {
                return fst;
            }
        }
        return Idle;
    }

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
        long codeSize;

        /**
         * Image ID
         */
        std::string imageId;

        /**
         * @brief Image size, as reported by the docker daemon
         */
        long imageSize;

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
        long containerSize;

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
        int timeout;

        /**
         * Concurrency
         */
        int concurrency = 5;

        /**
         * Environment
         */
        Environment environment;

        /**
         * lambda state
         */
        LambdaState state = LambdaState::Pending;

        /**
         * State reason
         */
        std::string stateReason;

        /**
         * State reason code
         */
        LambdaStateReasonCode stateReasonCode = Creating;

        /**
         * Code SHA256
         */
        std::string codeSha256;

        /**
         * Code
         */
        Code code;

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
        long averageRuntime = 0;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Returns an instance by instance ID
         *
         * @param instanceId instance ID
         * @return lambda instance.
         */
        Instance GetInstance(const std::string &instanceId);

        /**
         * @brief Remove an instance from the instances list.
         *
         * @par
         * The instance is identified by the containerId.
         *
         * @param instance
         */
        void RemoveInstance(const Instance &instance);

        /**
         * @brief Checks whether a event source with the given ARN exists already.
         *
         * @param eventSourceArn event source ARN
         * @return true if a event source with the given ARN exists.
         */
        [[nodiscard]] bool HasEventSource(const std::string &eventSourceArn) const;

        /**
         * @brief Checks whether a tags with the given tags key exists.
         *
         * @param key key of the tags
         * @return true if tags with the given key exists.
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
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<bsoncxx::document::view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
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

}// namespace AwsMock::Database::Entity::Lambda

#endif//AWSMOCK_DB_ENTITY_LAMBDA_H
