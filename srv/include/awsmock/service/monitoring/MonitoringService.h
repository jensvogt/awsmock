//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/monitoring/GetCountersRequest.h>
#include <awsmock/dto/monitoring/GetCountersResponse.h>
#include <awsmock/dto/monitoring/GetMultiCountersResponse.h>
#include <awsmock/dto/monitoring/mapper/Mapper.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/monitoring/IMonitoringRepository.h>

namespace Awsmock::Service {

    /**
     * @brief Monitoring service.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringService {

    public:
        /**
         * @brief Constructor
         */
        explicit MonitoringService() = default;

        /**
         * @brief Get counters request
         *
         * @param request get counters request
         * @return GetCountersResponse
         * @see GetCountersResponse
         */
        [[nodiscard]]
        Dto::Monitoring::GetCountersResponse GetCounters(const Dto::Monitoring::GetCountersRequest &request) const;

        /**
         * @brief Returns several series of counters in a map
         *
         * @param request monitoring counters request
         * @return map of counter-objects
         */
        Dto::Monitoring::GetMultiCountersResponse GetMultiCounters(const Dto::Monitoring::GetCountersRequest &request) const;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Monitoring"};

        /**
         * Database connection
         */
        std::shared_ptr<Database::IMonitoringRepository> _database = Database::RepositoryFactory::instance().monitoringRepository();
    };

} // namespace Awsmock::Service

