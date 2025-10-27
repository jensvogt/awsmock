//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_CREATOR_H
#define AWSMOCK_SERVICE_LAMBDA_CREATOR_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/ZipUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Lambda async creator
     *
     * @par
     * First it writes a base64 encoded file to the lambda directory (<i>/home/awsmock/data/lambda</i>). If the lambda code comes from the AWS CLI it is already a base64 encoded
     * zip-file, so we only write the base64 encoded code to the lambda directory. In case the lambda code comes from S3 bucket/key we need to encode the file and write it
     * to the lambda directory. This is needed, as the lambda server runs through all lambda entity in the database and starts the lambdas from the lambda directory.
     *
     * @par
     * Once the lambdas are written to the lambda directory, the creator decodes the lambda and check for a existing image in the docker registry. If it find a image, it will
     * create a docker container and start it. Otherwise, a docker image is create using the AWS runtime (Java, Python, nodes.js, etc.) and creates a docker image from the
     * Dockerfile for that runtime. Then a container is created and started.
     *
     * @par
     * To see the running container, issue a 'docker ps'. The container has a name of 'lambda-function-name:version. The docker tag is taken from the lambda function
     * tags. If a 'version' or 'dockerTag' exists in the lambda function, this is taken as the docker tag. Otherwise, 'latest' is used. If it is a function, which is loaded
     * from a versioned S3 bucket/key, the version tag of the S3 object is taken.
     *
     * @par
     * The docker container port is random and maps internally to the AWS runtime port (8080). The port is mapped to the host and saved in the database. The port will be
     * between 32768 and 65536. You can see the chosen port in the MongoDB or in the in-memory database. The outside port (<i><&lt;randomPort&gt;0</i>) is used for the invocation
     * request.
     *
     * @par
     * The docker creator runs asynchronously. Therefore, the lambda function is in 'pending' state as long as the asynchronous thread runs. Afterwards state status will be
     * set to 'active'. This means you need to wait until the state is 'active' before you can invoke the lambda function.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaCreator {

      public:

        /**
         * @brief Constructor.
         */
        explicit LambdaCreator() = default;

        /**
         * @brief Create a new lambda function
         *
         * @param lambda lambda entity
         * @param instanceId instanceId
         * @return updated lambda entity
         */
        Database::Entity::Lambda::Lambda CreateLambda(Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId) const;

        /**
         * @brief Create a new version of an existing lambda function
         *
         * @param lambda lambda instance
         * @param functionCode new function code
         * @param newVersion new version
         * @return
         */
        void UpdateLambda(Database::Entity::Lambda::Lambda &lambda, const std::string &functionCode, const std::string &newVersion) const;

      private:

        /**
         * @brief Create a lambda function instance
         *
         * @param instanceId name of the instance, lambda function name + '-' + 8 random hex digits
         * @param lambda lambda entity
         * @param functionCode function code
         * @return container ID
         */
        std::string CreateInstance(const std::string &instanceId, Database::Entity::Lambda::Lambda &lambda, const std::string &functionCode) const;

        /**
         * @brief Save the ZIP file and unpack it in a temporary folder
         *
         * @param zipFile Base64 encoded ZIP file
         * @param lambdaEntity lambda entity
         * @param dockerTag docker tag to use
         */
        static void CreateDockerImage(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag);

        /**
         * @brief Creates a new docker container, in case the container does not exists inside the docker daemon.
         *
         * @param lambda lambda entity.
         * @param instanceId lambda instance ID.
         * @param hostPort host port
         * @param dockerTag docker tag.
         * @see Database::Entity::Lambda::Lambda
         */
        static void CreateDockerContainer(const Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId, int hostPort, const std::string &dockerTag);

        /**
         * @brief Converts the lambda environment to a vector of string, which is needed by the docker API
         *
         * @par
         * Additionally, the AWS_LAMBDA_FUNCTION_TIMEOUT variable will be added. This restricts the AWS Lambda RIE for the given timeout.
         *
         * @param lambda lambda entity
         * @return vector of strings containing the runtime environment as key=value pairs
         * @see Database::Entity::Lambda::Lambda
         */
        static std::vector<std::string> GetEnvironment(const Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Unpack the provided ZIP file.
         *
         * <p>Needed only when the lambda function is provided as zipped request body.</p>
         *
         * @param codeDir temporary data directory
         * @param functionCode Base64 encoded zip file.
         * @param runtime AWS lambda runtime name
         * @return code directory
         */
        static std::string UnpackZipFile(const std::string &codeDir, const std::string &functionCode, const std::string &runtime);

        /**
         * @brief Returns the docker tag.
         *
         * <p>The method returns the docker tags in that order:
         * <ul>
         * <li>version: if the lambda entity has a version tag, the version tag is used.</li>
         * <li>dockerTag: if the lambda entity has a dockerTag, the dockerTag is used.</li>
         * <li>tag: if the lambda entity has a tag named tag, this tag is used.</li>
         * <li>latest: default return value.</li>
         * </ul>
         * </p>
         *
         * @param lambda lambda database entity
         * @return random port between 32768 and 65536
         * @see Database::Entity::Lambda::Lambda
         */
        static std::string GetDockerTag(const Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Write Base64 encoded file to lambda dir
         *
         * @param zipFile base64 encoded code from AWS CLI
         * @param lambda lambda entity
         * @param dockerTag docker tag to use
         * @return base64 string
         * @see Database::Entity::Lambda::Lambda
         */
        static std::string WriteBase64File(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambda, const std::string &dockerTag);

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();

        /**
         * Dockerized flag
         */
        bool _dockerized = Core::Configuration::instance().GetValue<bool>("awsmock.dockerized");
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_CREATOR_H
