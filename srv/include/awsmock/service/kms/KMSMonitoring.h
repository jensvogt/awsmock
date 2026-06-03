//
// Created by vogje01 on 4/21/24.
//

#pragma once

// AwsMock includes
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/repository/RepositoryFactory.h>

namespace Awsmock::Service {

    /**
     * @brief KMS monitoring thread
     *
     * @par
     * The KMS monitoring is 'self-starting', which means the constructor start itself as a background process.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSMonitoring {
    public:
        /**
         * @brief Constructor
         */
        explicit KMSMonitoring() = default;

        /**
         * @brief Update counters
         */
        void UpdateCounter();

    private:
        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::IKMSRepository> _kmsDatabase = Database::RepositoryFactory::instance().kmsRepository();
    };

} // namespace Awsmock::Service
