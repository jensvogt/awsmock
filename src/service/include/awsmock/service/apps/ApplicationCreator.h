//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_CREATOR_H
#define AWSMOCK_SERVICE_APPLICATION_CREATOR_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/ZipUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apps/model/Status.h>
#include <awsmock/entity/apps/Application.h>
#include <awsmock/repository/ApplicationDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Application async creator
     *
     * @par
     * First, it writes a base64 encoded file to the application directory (<i>/home/awsmock/data/application</i>). If the application code comes from the AWS CLI it is already a base64 encoded
     * zip-file, so we only write the base64 encoded code to the application directory. In case the application code comes from S3 bucket/key we need to encode the file and write it
     * to the application directory. This is needed, as the application server runs through all application entity in the database and starts the applications from the application directory.
     *
     * @par
     * Once the applications are written to the application directory, the creator decodes the application and check for a existing image in the docker registry. If it find a image, it will
     * create a docker container and start it. Otherwise, a docker image is create using the AWS runtime (Java, Python, nodes.js, etc.) and creates a docker image from the
     * Dockerfile for that runtime. Then a container is created and started.
     *
     * @par
     * To see the running container, issue a 'docker ps'. The container has a name of 'application-function-name:version. The docker tag is taken from the application
     * tags. If a 'version' or 'dockerTag' exists in the application, this is taken as the docker tag. Otherwise, 'latest' is used. If it is a function, which is loaded
     * from a versioned S3 bucket/key, the version tag of the S3 object is taken.
     *
     * @par
     * The docker container port is random and maps internally to the AWS runtime port (8080). The port is mapped to the host and saved in the database. The port will be
     * between 32768 and 65536. You can see the chosen port in the MongoDB or in the in-memory database. The outside port (<i><&lt;randomPort&gt;0</i>) is used for the invocation
     * request.
     *
     * @par
     * The docker creator runs asynchronously. Therefore, the application is in 'pending' state as long as the asynchronous thread runs. Afterwards state status will be
     * set to 'active'. This means you need to wait until the state is 'active' before you can invoke the application.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationCreator {

      public:

        /**
         * @brief Constructor.
         */
        explicit ApplicationCreator() = default;

        /**
         * @brief Create a new application
         *
         * @param applicationCodeFile full path to a zipped and BASE64 encoded function code
         * @param region application region
         * @param name application name
         * @param instanceId instanceId
         */
        void operator()(const std::string &applicationCodeFile, const std::string &region, const std::string &name, const std::string &instanceId) const;

      private:

        /**
         * @brief Create an application instance
         *
         * @param instanceId name of the instance, application name + '-' + 8 random hex digits
         * @param applicationEntity application entity
         * @param applicationCodeFile function code
         */
        static void CreateInstance(const std::string &instanceId, Database::Entity::Apps::Application &applicationEntity, const std::string &applicationCodeFile);

        /**
         * @brief Save the ZIP file and unpack it in a temporary folder
         *
         * @param applicationCodeFile Base64 encoded application code file
         * @param applicationEntity application entity
         * @param dockerTag docker tag to use
         */
        static void CreateDockerImage(const std::string &applicationCodeFile, Database::Entity::Apps::Application &applicationEntity, const std::string &dockerTag);

        /**
         * @brief Creates a new docker container, in case the container does not exist inside the docker daemon.
         *
         * @par
         * If the public port < 0, a random free port will be used, otherwise the application.publicPort will be used.
         *
         * @param applicationEntity application entity.
         * @param instanceId application instance ID.
         * @param dockerTag docker tag.
         * @see Database::Entity::Application::Application
         */
        static void CreateDockerContainer(const Database::Entity::Apps::Application &applicationEntity, const std::string &instanceId, const std::string &dockerTag);

        /**
         * @brief Converts the application environment to a vector of string, which is needed by the docker API
         *
         * @param application application entity
         * @return vector of strings containing the runtime environment as key=value pairs
         * @see Database::Entity::Apps::Application
         */
        static std::vector<std::string> GetEnvironment(const Database::Entity::Apps::Application &application);

        /**
         * @brief Unpack the provided ZIP file.
         *
         * <p>Needed only when the application is provided as a zipped request body.</p>
         *
         * @param codeDir temporary data directory
         * @param applicationCode Base64 encoded a zip-file.
         * @param runtime AWS application runtime name
         * @return code directory
         */
        static std::string UnpackZipFile(const std::string &codeDir, const std::string &applicationCode, const std::string &runtime);

        /**
         * @brief Returns the docker tag.
         *
         * <p>The method returns the docker tags in that order:
         * <ul>
         * <li>version: if the application entity has a version tag, the version tag is used.</li>
         * <li>dockerTag: if the application entity has a dockerTag, the dockerTag is used.</li>
         * <li>tag: if the application entity has a tag named tag, this tag is used.</li>
         * <li>latest: default return value.</li>
         * </ul>
         * </p>
         *
         * @param application application database entity
         * @return random port between 32768 and 65536
         * @see Database::Entity::Application::Application
         */
        static std::string GetDockerTag(const Database::Entity::Apps::Application &application);
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_CREATOR_H
