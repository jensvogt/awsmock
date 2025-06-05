//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/lambda/LambdaService.h>

namespace AwsMock::Service {

    enum TaskType {
        createSecret,
        setSecret,
        testSecret,
        finishSecret,
        unknown
    };

    static std::map<TaskType, std::string> TaskTypeNames{
            {createSecret, "createSecret"},
            {setSecret, "setSecret"},
            {testSecret, "testSecret"},
            {unknown, "unknown"},
    };

    [[maybe_unused]] static std::string TaskTypeToString(const TaskType taskType) {
        return TaskTypeNames[taskType];
    }

    [[maybe_unused]] static TaskType TaskTypeFromString(const std::string &taskType) {
        for (auto &[fst, snd]: TaskTypeNames) {
            if (snd == taskType) {
                return fst;
            }
        }
        return unknown;
    }

    /**
     * @brief Secrets manager rotation
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretRotation {

      public:

        /**
         * @brief Constructor.
         */
        explicit SecretRotation() = default;

        /**
         * @brief Rotate the secret
         *
         * @param secret to create
         * @param clientRequestToken client request token
         */
        void operator()(const Database::Entity::SecretsManager::Secret &secret, const std::string &clientRequestToken) const;

      private:

        /**
         * @brief Create a secret
         *
         * @param secret to create
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const;

        /**
         * @brief Set a secret in the resource
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const;

        /**
         * @brief Test the new secret
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const;

        /**
         * @brief Finish secret rotation
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const;

        /**
         * @brief Send a lambda invocation request
         *
         * @param lambda lambda entity
         * @param body message body
         */
        void SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const std::string &body) const;

        /**
         * Lambda service
         */
        LambdaService _lambdaService;

        /**
         * Lambda service
         */
        Database::LambdaDatabase _lambdaDatabase = std::move(Database::LambdaDatabase::instance());
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H
