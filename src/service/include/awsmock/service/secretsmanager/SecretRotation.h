//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/secretsmanager/LambdaInvocationRequest.h>
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
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
            {finishSecret, "finishSecret"},
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
        void operator()(Database::Entity::SecretsManager::Secret &secret, const std::string &clientRequestToken) const;

      private:

        /**
         * @brief Create a secret
         *
         * @param secret to create
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        static void CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Set a secret in the resource
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        static void SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Test the new secret
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        static void TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Finish secret rotation
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        static void FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Send a lambda invocation request
         *
         * @param lambda lambda entity
         * @param body message body
         */
        static void SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, std::string &body);

        /**
         * @brief Calculates the next rotation date
         *
         * @param secret secret entity
         * @return next rotation datetime
         */
        static system_clock::time_point GetNextRotationDate(const Database::Entity::SecretsManager::Secret &secret);
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_ROTATION_H
