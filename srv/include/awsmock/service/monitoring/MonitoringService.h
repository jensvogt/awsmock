//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_MONITORING_SERVICE_H
#define AWSMOCK_SERVICE_MONITORING_SERVICE_H

// AwsMock includes
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/monitoring/GetCountersRequest.h>
#include <awsmock/dto/monitoring/GetCountersResponse.h>
#include <awsmock/dto/monitoring/GetMultiCountersResponse.h>
#include <awsmock/dto/monitoring/mapper/Mapper.h>
#include <awsmock/repository/MonitoringDatabase.h>

namespace AwsMock::Service {

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
        explicit MonitoringService() : _database(Database::MonitoringDatabase::instance()) {};

        /**
         * @brief Get counters request
         *
         * @param request get counters request
         * @return GetCountersResponse
         * @see GetCountersResponse
         */
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
         * Database connection
         */
        Database::MonitoringDatabase &_database;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MONITORING_SERVICE_H
