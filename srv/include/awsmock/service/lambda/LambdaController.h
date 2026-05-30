//
// Created by vogje01 on 28/05/2026.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_CONTROLLER_H
#define AWSMOCK_SERVICE_LAMBDA_CONTROLLER_H

// C++ standard includes
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/lambda/internal/StartLambdaRequest.h>
#include <awsmock/dto/lambda/internal/StopLambdaRequest.h>
#include <awsmock/dto/lambda/model/InvocationType.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/lambda/LambdaService.h>

namespace AwsMock::Service {

    /**
     * @brief Lambda controller
     *
     * @par
     * Reacts to Boost EventBus signals for lambda lifecycle commands (start, stop, start-all,
     * stop-all, invoke, check). Each signal handler delegates to the LambdaService and/or
     * ContainerService, then emits monitoring metrics.
     *
     * @par
     * Additionally schedules a periodic health check task that inspects running Docker
     * containers and removes dead instances from the database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaController final : public AbstractServer {

    public:
        /**
         * @brief Constructor
         *
         * Connects all EventBus lambda signals and registers the periodic health-check task.
         *
         * @param scheduler async task scheduler
         */
        explicit LambdaController(Core::Scheduler &scheduler);

        /**
         * @brief Shutdown controller and deregister scheduled tasks
         */
        void Shutdown() override;

    private:
        /**
         * @brief Handle sigLambdaStart — start a single lambda by ARN
         *
         * @param functionArn lambda function ARN
         * @param region AWS region
         */
        void OnStartLambda(const std::string &functionArn, const std::string &region) const;

        /**
         * @brief Handle sigLambdaStop — stop all running containers of a single lambda
         *
         * @param functionArn lambda function ARN
         * @param region AWS region
         */
        void OnStopLambda(const std::string &functionArn, const std::string &region) const;

        /**
         * @brief Handle sigLambdaStartAll — start every enabled lambda in the given region
         *
         * @param region AWS region
         */
        void OnStartAllLambdas(const std::string &region) const;

        /**
         * @brief Handle sigLambdaStopAll — stop all running lambda containers in the given region
         *
         * @param region AWS region
         */
        void OnStopAllLambdas(const std::string &region) const;

        /**
         * @brief Handle sigLambdaInvoke — invoke a lambda function
         *
         * @param region AWS region
         * @param functionName lambda function name
         * @param payload JSON payload
         * @param invocationType "RequestResponse" or "Event"
         * @param promise result channel; nullptr for fire-and-forget (EVENT invocations)
         */
        void OnInvokeLambda(const std::string &region, const std::string &functionName, const std::string &payload,
                            const std::string &invocationType,
                            const std::shared_ptr<std::promise<std::pair<int, std::string> > > &promise) const;

        /**
         * @brief Handle sigLambdaCheck — inspect the Docker daemon state for a single lambda
         *
         * @param functionArn lambda function ARN
         */
        void OnCheckLambda(const std::string &functionArn) const;

        /**
         * @brief Periodic health-check task
         *
         * @par
         * Iterates all active lambdas, inspects each Docker container, and removes
         * instances whose containers are no longer running.
         */
        void CheckContainerHealth() const;

        /**
         * @brief Returns (or creates) the per-function mutex used to serialise instance selection.
         *
         * @param functionName lambda function name
         * @return shared_ptr to the function's mutex
         */
        std::shared_ptr<boost::mutex> GetOrCreateMutex(const std::string &functionName) const;

        /**
         * @brief Lambda database
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * @brief Lambda service
         */
        LambdaService _lambdaService;

        /**
         * @brief Container (Docker/Podman) service
         */
        ContainerService &_containerService;

        /**
         * @brief Async task scheduler
         */
        Core::Scheduler &_scheduler;

        /**
         * @brief AWS region read from configuration
         */
        std::string _region;

        /**
         * @brief Health-check interval in seconds
         */
        int _healthCheckPeriod{};

        /**
         * @brief Per-function mutex map — serialises instance selection so that two concurrent
         * invocations of the same function never claim the same idle instance.
         */
        mutable std::map<std::string, std::shared_ptr<boost::mutex> > _instanceMutex;

        /**
         * @brief Guards _instanceMutex map insertions.
         */
        mutable std::mutex _mutexMapLock;

        mutable logger_t _logger{boost::log::keywords::channel = "Lambda"};
    };

} // namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_CONTROLLER_H
