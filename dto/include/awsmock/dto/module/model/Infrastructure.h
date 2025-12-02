//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_DTO_COMMON_INFRASTRUCTURE_H
#define AWSMOCK_DTO_COMMON_INFRASTRUCTURE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apps/Application.h>
#include <awsmock/entity/cognito/Group.h>
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>
#include <awsmock/entity/dynamodb/Item.h>
#include <awsmock/entity/dynamodb/Table.h>
#include <awsmock/entity/kms/Key.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/entity/transfer/Transfer.h>

namespace AwsMock::Dto::Module {

    using bsoncxx::builder::basic::array;
    using bsoncxx::builder::basic::document;
    using bsoncxx::builder::basic::kvp;

    /**
     * @brief Infrastructure
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Infrastructure {

        /**
         * @brief S3 buckets
         */
        Database::Entity::S3::BucketList s3Buckets;

        /**
         * @brief S3 objects
         */
        std::vector<Database::Entity::S3::Object> s3Objects;

        /**
         * @brief SQS queues
         */
        Database::Entity::SQS::QueueList sqsQueues;

        /**
         * @brief SQS resources
         */
        Database::Entity::SQS::MessageList sqsMessages;

        /**
         * @brief SNS topics
         */
        Database::Entity::SNS::TopicList snsTopics;

        /**
         * @brief SQS resources
         */
        Database::Entity::SNS::MessageList snsMessages;

        /**
         * @brief Lambda functions
         */
        Database::Entity::Lambda::LambdaList lambdas;

        /**
         * @brief Transfer servers
         */
        Database::Entity::Transfer::TransferList transferServers;

        /**
         * @brief Cognito user pools
         */
        Database::Entity::Cognito::UserPoolList cognitoUserPools;

        /**
         * @brief Cognito users
         */
        Database::Entity::Cognito::UserList cognitoUsers;

        /**
         * Cognito user groups
         */
        Database::Entity::Cognito::GroupList cognitoUserGroups;

        /**
         * DynamoDb tables
         */
        Database::Entity::DynamoDb::TableList dynamoDbTables;

        /**
         * @brief DynamoDb items
         */
        Database::Entity::DynamoDb::ItemList dynamoDbItems;

        /**
         * @brief Secrets manager secrets
         */
        Database::Entity::SecretsManager::SecretList secrets;

        /**
         * @brief Key management keys
         */
        Database::Entity::KMS::KeyList kmsKeys;

        /**
         * @brief System manager parameters
         */
        Database::Entity::SSM::ParameterList ssmParameters;

        /**
         * @brief Applications
         */
        std::vector<Database::Entity::Apps::Application> applications;

        /**
         * @brief API gateway keys
         */
        std::vector<Database::Entity::ApiGateway::ApiKey> apiKeys;

        /**
         * @brief Convert to a BSON object
         *
         * @return bson document
         */
        [[nodiscard]] document ToDocument() const;

        /**
         * @brief Convert from BSON document
         *
         * @param document BSON document
         */
        void FromDocument(const view &document);

        /**
         * @brief From JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_COMMON_INFRASTRUCTURE_H
