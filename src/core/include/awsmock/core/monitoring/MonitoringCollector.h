//
// Created by vogje01 on 12/19/24.
//

#ifndef AWS_MOCK_CORE_MONITORING_COLLECTOR_H
#define AWS_MOCK_CORE_MONITORING_COLLECTOR_H

// C++ includes
#include <map>
#include <sstream>
#include <string>
#include <utility>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>

#define MONITORING_SEGMENT_NAME "MonitoringShmSegment"
#define MONITORING_MAP_NAME "MonitoringCounter"
#define MONITORING_SHM_SIZE (1024 * 1024)

namespace AwsMock::Core {

    using std::chrono::system_clock;
    using SegmentManager = boost::interprocess::managed_shared_memory::segment_manager;

    template<class T>
    using ShmAllocator = boost::interprocess::allocator<T, SegmentManager>;
    using ShmString = boost::interprocess::basic_string<char, std::char_traits<char>, ShmAllocator<char>>;

    enum CounterType {
        COUNT_PER_SECOND,
        COUNT_ABSOLUTE,
        GAUGE,
        HISTOGRAM,
        UNKNOWN
    };

    struct MonitoringCounter {
        ShmString name;
        ShmString labelName;
        ShmString labelValue;
        double value{};
        long count{};
        CounterType type = UNKNOWN;
        system_clock::time_point timestamp;

        // The default constructor must accept an allocator for in-place construction
        explicit MonitoringCounter(const ShmAllocator<char> &alloc)
            : name(alloc), labelName(alloc), labelValue(alloc), timestamp(system_clock::now()) {}

        MonitoringCounter(ShmString name, ShmString labelName, ShmString labelValue, const double value, const CounterType type)
            : name(std::move(name)), labelName(std::move(labelName)), labelValue(std::move(labelValue)), value(value), count(1), type(type), timestamp(system_clock::now()) {}

        // Allow copying/moving (the string already knows its allocator)
        MonitoringCounter(const MonitoringCounter &) = default;
        MonitoringCounter &operator=(const MonitoringCounter &) = default;
    };

    // std::map with allocator rebound to the pair type. Note: std::map is allocator-aware.
    using MapValueType = std::pair<const ShmString, MonitoringCounter>;
    using MapAllocator = ShmAllocator<MapValueType>;
    using ShmMap = boost::container::map<ShmString, MonitoringCounter, std::less<>, MapAllocator>;

    /**
    * @brief Shared memory manager for the monitoring counters
    *
    * @author jens.vogt\@opitz-consulting.com
    */
    class MonitoringCollector {

      public:

        /**
         * @brief Constructor
         *
         * @par
         * Creates the shared memory segment and initializes the monitoring counter map.
         */
        explicit MonitoringCollector();

        /**
         * @brief Singleton instance
         */
        static MonitoringCollector &instance() {
            static MonitoringCollector shmUtils;
            return shmUtils;
        }

        /**
         * @brief Sets a gauge value
         *
         * @param name gauge name
         * @param value gauge value
         */
        void SetGauge(const std::string &name, double value) const;

        /**
         * @brief Sets a gauge value
         *
         * @param name gauge name
         * @param value gauge value
         */
        void SetGauge(const std::string &name, long value) const;

        /**
         * @brief Sets a gauge value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value gauge value
         */
        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, long value) const;

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
         * @brief Sets a gauge value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         */
        template<typename T>
        [[nodiscard]] T GetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Increment a counter per second map value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value increment value, default: 1.0
         */
        void IncCountPerSec(const std::string &name, const std::string &labelName, const std::string &labelValue, double value = 1.0) const;

        /**
         * @brief Increment an absolute counter map value
         *
         * @param name gauge name
         * @param value increment value, default: 1.0
         */
        void IncCountAbs(const std::string &name, double value = 1.0) const;

        /**
         * @brief Increment an absolute counter map value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value increment value, default: 1.0
         */
        void IncCountAbs(const std::string &name, const std::string &labelName, const std::string &labelValue, double value = 1.0) const;

        /**
         * @brief Returns the size of the shared memory segment
         *
         * @return size of the shared memory segment
         */
        [[nodiscard]] long Size() const;

        /**
         * @brief Return the map of counters
         *
         * @return counter map
         */
        [[nodiscard]] ShmMap *GetCounterMap() const;

        /**
         * @brief Dumps the content of the counter map to info logger
         */
        void DumpCounterMap() const;

        /**
         * @brief Returns the ID of a monitoring counter map key
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         */
        [[nodiscard]] static std::string GetId(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Clear the map of counters
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         */
        void Clear(const std::string &name, const std::string &labelName, const std::string &labelValue) const;

        /**
         * @brief Clear the map of counters
         *
         * @par
         * All counters starting with 'name' will be cleared
         *
         * @param name gauge name
         */
        void Clear(const std::string &name) const;

        /**
         * @brief Clear the map of counters
         */
        void Clear() const;

        /**
         * @brief Clear the map of counters
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const MonitoringCollector &s);

      private:

        /**
         * Shared memory segment
         */
        boost::interprocess::managed_shared_memory _segment;

        /**
         * Monitoring counter-map
         */
        ShmMap *_counterMap;

        /**
         * Mutex
         */
        static boost::mutex _monitoringMutex;

        /**
         * Monitoring period
         */
        long _period;
    };

    template<class T>
    T MonitoringCollector::GetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_monitoringMutex);
        const ShmAllocator<char> char_alloc(_segment.get_segment_manager());
        if (const ShmString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->contains(k)) {
            return static_cast<T>(_counterMap->at(k).value);
        }
        return static_cast<T>(0);
    }
}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_MONITORING_COLLECTOR_H
