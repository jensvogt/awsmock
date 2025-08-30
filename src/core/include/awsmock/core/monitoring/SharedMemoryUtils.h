//
// Created by vogje01 on 12/19/24.
//

#ifndef AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H
#define AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H

// C++ includes
#include <map>
#include <string>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>

#define MONITORING_SEGMENT_NAME "MonitoringShmSegment"
#define MONITORING_MAP_NAME "MonitoringCounter"

namespace AwsMock::Core {

    using std::chrono::system_clock;

    enum CounterType {
        COUNTER,
        GAUGE,
        HISTOGRAM,
        UNKNOWN
    };

    struct MonitoringCounter {
        std::string name;
        std::string labelName;
        std::string labelValue;
        double value{};
        long count{};
        CounterType type = UNKNOWN;
        system_clock::time_point timestamp;
    };

    class SharedMemoryUtils {

      public:

        using ShmAllocator = boost::interprocess::allocator<std::pair<const std::string, MonitoringCounter>, boost::interprocess::managed_shared_memory::segment_manager>;
        using CounterMapType = boost::container::map<std::string, MonitoringCounter, std::less<std::string>, ShmAllocator>;

        /**
         * @brief Constructor
         *
         * @par
         * Creates the shared memory segment and initializes the monitoring counter map.
         */
        explicit SharedMemoryUtils();

        /**
         * @brief Singleton instance
         */
        static SharedMemoryUtils &instance() {
            static SharedMemoryUtils shmUtils;
            return shmUtils;
        }

        /**
         * @brief Sets a gauge value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value gauge value
         */
        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, double value) const;

        /**
         * @brief Increment a counter map value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value increment value, default: 1.0
         */
        void IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, double value = 1.0) const;

        /**
         * @brief Returns the size of the shared memory segment
         *
         * @return size of the shared memory segment
         */
        [[nodiscard]] long Size() const;

        [[nodiscard]] CounterMapType *GetCounterMap() const;

        /**
         * @brief Returns the ID of an monitoring counter map key
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         */
        static std::string GetId(const std::string &name, const std::string &labelName, const std::string &labelValue);

      private:

        /**
         * Shared memory segment
         */
        boost::interprocess::managed_shared_memory _segment;

        /**
         * Monitoring counter-map
         */
        CounterMapType *_counterMap;

        /**
         * Mutex
         */
        static boost::mutex _monitoringMutex;
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H
