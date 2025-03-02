//
// Created by vogje01 on 6/3/24.
//

#ifndef AWSMOCK_MANAGER_MONITORING_H
#define AWSMOCK_MANAGER_MONITORING_H

// AwsMock includes
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Manager {

    /**
     * @brief Manager monitoring
     *
     * @par
     * Reports up/down status of modules using the following correlations:
     *
     *  - Active && running == 0
     *  - Active && not running == 2
     *  - Not Active && not running == 0
     *  - Not Active && running == 1
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ManagerMonitoring {

      public:

        /**
         * @brief Constructor
         */
        ManagerMonitoring() {};

      private:

        /**
         * @brief Update the counter
         */
        void UpdateCounter() const;

        /**
         * @brief Module database connection
         */
        Database::ModuleDatabase &_moduleDatabase = Database::ModuleDatabase::instance();
    };

}// namespace AwsMock::Manager
#endif// AWSMOCK_MANAGER_M