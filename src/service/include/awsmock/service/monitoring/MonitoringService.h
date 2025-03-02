//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_MONITORING_SERVICE_H
#define AWSMOCK_SERVICE_MONITORING_SERVICE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/iostreams/copy.hpp>
#include <boost/thread/thread.hpp>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MemoryMappedFile.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/monitoring/GetCountersRequest.h>
#include <awsmock/dto/monitoring/GetCountersResponse.h>
#include <awsmock/dto/monitoring/GetMultiCountersResponse.h>
#include <awsmock/repository/MonitoringDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief S3 service.
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
         * @brief Returns deveral series of counters in a map
         *
         * @param request monitoring counter request
         * @return map of counter objects
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
