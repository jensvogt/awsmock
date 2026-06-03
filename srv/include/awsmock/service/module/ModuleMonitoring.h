//
// Created by vogje01 on 6/3/24.
//

#pragma once

// AwsMock includes
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace Awsmock::Service {

    /**
     * @brief Manager monitoring
     *
     * Reports up/down status of modules using the following correlations:
     *
     *  - Active && running == 0
     *  - Active && not running == 2
     *  - Not Active && not running == 0
     *  - Not Active && running == 1
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleMonitoring {

    public:
        /**
         * @brief Constructor
         */
        explicit ModuleMonitoring() = default;

        /**
         * Update the counter
         */
        void UpdateCounter() const;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Module"};

        /**
         * Module database connection
         */
        std::shared_ptr<Database::IModuleRepository> _moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
    };

} // namespace Awsmock::Service
