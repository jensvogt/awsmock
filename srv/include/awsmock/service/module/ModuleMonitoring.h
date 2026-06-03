//
// Created by vogje01 on 6/3/24.
//

#ifndef AWSMOCK_SERVICE_MODULE_MONITORING_H
#define AWSMOCK_SERVICE_MODULE_MONITORING_H

// AwsMock includes
#include <../../../../../db/include/awsmock/repository/module/ModuleMongoRepository.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
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

        mutable logger_t _logger{boost::log::keywords::channel = "Module"};

        /**
         * Module database connection
         */
        Database::ModuleMongoRepository &_moduleDatabase = Database::ModuleMongoRepository::instance();
    };

}// namespace Awsmock::Service
#endif// AWSMOCK_SERVICE_MODULE_MONITORING_H
