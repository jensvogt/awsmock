//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_LAMBDA_H
#define AWSMOCK_DB_ENTITY_LAMBDA_H

// C++ includes
#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/stdx.hpp>

// Poco includes
#include <Poco/JSON/Object.h>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/entity/lambda/Code.h>
#include <awsmock/entity/lambda/Environment.h>
#include <awsmock/entity/lambda/EphemeralStorage.h>
#include <awsmock/entity/lambda/EventSourceMapping.h>
#include <awsmock/entity/lambda/Instance.h>
#include <awsmock/entity/lambda/Tags.h>
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
            {LambdaState::Pending, "Pending"},
            {LambdaState::Active, "Active"},
            {LambdaState::Inactive, "Inactive"},
            {LambdaState::Failed, "Failed"},
    };

    [[maybe_unused]] static std::string LambdaStateToString(LambdaState lambdaState) {
        return LambdaStateNames[lambdaState];
    }

    [[maybe_unused]] static LambdaState LambdaStateFromString(const std::string &lambdaState) {
        for (auto &it: LambdaStateNames) {
            if (it.second == lambdaState) {
                return it.first;
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
            {LambdaStateReasonCode::Idle, "Idle"},
            {LambdaStateReasonCode::Creating, "Creating"},
            {LambdaStateReasonCode::Restoring, "Restoring"},
            {LambdaStateReasonCode::EniLimitExceeded, "EniLimitExceeded"},
            {LambdaStateReasonCode::InsufficientRolePermissions, "InsufficientRolePermissions"},
            {LambdaStateReasonCode::InvalidConfiguration, "InvalidConfiguration"},
            {LambdaStateReasonCode::InternalError, "InternalError"},
            {LambdaStateReasonCode::SubnetOutOfIPAddresses, "SubnetOutOfIPAddresses"},
            {LambdaStateReasonCode::InvalidSubnet, "InvalidSubnet"},
            {LambdaStateReasonCode::InvalidSecurityGroup, "InvalidSecurityGroup"},
            {LambdaStateReasonCode::ImageDeleted, "ImageDeleted"},
            {LambdaStateReasonCode::ImageAccessDenied, "ImageAccessDenied"},
            {LambdaStateReasonCode::InvalidImage, "InvalidImage"},
            {LambdaStateReasonCode::KMSKeyAccessDenied, "KMSKeyAccessDenied"},
            {LambdaStateReasonCode::KMSKeyNotFound, "KMSKeyNotFound"},
            {LambdaStateReasonCode::InvalidStateKMSKey, "InvalidStateKMSKey"},
            {LambdaStateReasonCode::DisabledKMSKey, "DisabledKMSKey"},
            {LambdaStateReasonCode::EFSIOError, "EFSIOError"},
            {LambdaStateReasonCode::EFSMountConnectivityError, "EFSMountConnectivityError"},
            {LambdaStateReasonCode::EFSMountFailure, "EFSMountFailure"},
            {LambdaStateReasonCode::EFSMountTimeout, "EFSMountTimeout"},
            {LambdaStateReasonCode::InvalidRuntime, "InvalidRuntime"},
            {LambdaStateReasonCode::InvalidZipFileException, "InvalidZipFileException"},
            {LambdaStateReasonCode::FunctionError, "FunctionError"},
    };

    [[maybe_unused]] static std::string LambdaStateReasonCodeToString(LambdaStateReasonCode lambdaStateReasonCode) {
        return LambdaStateReasonCodeNames[lambdaStateReasonCode];
    }

    [[maybe_unused]] static LambdaStateReasonCode LambdaStateReasonCodeFromString(const std::string &lambdaStateReasonCode) {
        for (auto &it: LambdaStateReasonCodeNames) {
            if (it.second == lambdaStateReasonCode) {
                return it.first;
            }
        }
        return LambdaStateReasonCode::Idle;
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
         * Size of the code in bytes
         */
        long codeSize;

        /**
         * Image ID
         */
        std::string imageId;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Random host port
         */
        int hostPort;

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
        LambdaStateReasonCode stateReasonCode = LambdaStateReasonCode::Creating;

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
        void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult);

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

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
