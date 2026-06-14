//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// C++ includes
#include <map>
#include <memory>
#include <string>

// Boost includes
#include <boost/asio/io_context.hpp>

// Awsmock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

    /**
     * @brief API gateway module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         *
         * @param ioc shared Boost.Asio io_context used for all proxy listeners
         */
        explicit ApiGatewayServer(boost::asio::io_context &ioc);

        /**
         * @brief Shutdown the api-gateway server
         */
        void shutdown() override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * @brief Update counters
         */
        void updateCounter() const;

        /**
         * @brief Back up the api-gateway objects
         */
        static void backupApiGateway();

        /**
         * @brief Shared io_context from the manager, used by all proxy listeners
         */
        boost::asio::io_context &_ioc;

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();

        /**
         * @brief In-process proxy listeners, one per REST API
         */
        //std::map<std::string, std::shared_ptr<Agw::ProxyListener>> _listeners;
    };

}// namespace Awsmock::Service
