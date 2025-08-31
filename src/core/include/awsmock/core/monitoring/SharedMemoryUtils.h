//
// Created by vogje01 on 12/19/24.
//

#ifndef AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H
#define AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H

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
#define MONITORING_SHM_SIZE 1024 * 1024

namespace AwsMock::Core {

    using std::chrono::system_clock;
    using SegmentManager = boost::interprocess::managed_shared_memory::segment_manager;

    template<class T>
    using ShmemAllocator = boost::interprocess::allocator<T, SegmentManager>;
    using ShmemString = boost::interprocess::basic_string<char, std::char_traits<char>, ShmemAllocator<char>>;

    enum CounterType {
        COUNTER,
        GAUGE,
        HISTOGRAM,
        UNKNOWN
    };

    struct MonitoringCounter {
        ShmemString name;
        ShmemString labelName;
        ShmemString labelValue;
        double value{};
        long count{};
        CounterType type = UNKNOWN;
        system_clock::time_point timestamp;

        // The default constructor must accept an allocator for in-place construction
        explicit MonitoringCounter(const ShmemAllocator<char> &alloc)
            : name(alloc), labelName(alloc), labelValue(alloc), timestamp(system_clock::now()) {}

        MonitoringCounter(ShmemString name, const ShmemString &labelName, const ShmemString &labelValue, const double value, const CounterType type)
            : name(std::move(name)), labelName(labelName), labelValue(labelValue), value(value), count(1), type(type), timestamp(system_clock::now()) {}

        // Allow copying/moving (the string already knows its allocator)
        MonitoringCounter(const MonitoringCounter &) = default;
        MonitoringCounter &operator=(const MonitoringCounter &) = default;
    };

    // std::map with allocator rebound to the pair type. Note: std::map is allocator-aware.
    using MapValueType = std::pair<const ShmemString, MonitoringCounter>;
    using MapAllocator = ShmemAllocator<MapValueType>;
    using ShmemMap = boost::container::map<ShmemString, MonitoringCounter, std::less<>, MapAllocator>;

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
         * @brief Increment a gauge value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value gauge value
         */
        void IncGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, long value = 1) const;

        /**
         * @brief Increment a gauge value
         *
         * @param name gauge name
         * @param labelName label name
         * @param labelValue value of the label
         * @param value gauge value
         */
        void IncGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, double value = 1.0) const;

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

        /**
         * @brief Return the counter map
         *
         * @return counter map
         */
        [[nodiscard]] ShmemMap *GetCounterMap() const;

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
        friend std::ostream &operator<<(std::ostream &os, const SharedMemoryUtils &s);

      private:

        /**
         * Shared memory segment
         */
        boost::interprocess::managed_shared_memory _segment;

        /**
         * Monitoring counter-map
         */
        ShmemMap *_counterMap;

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
    T SharedMemoryUtils::GetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_monitoringMutex);
        const ShmemAllocator<char> char_alloc(_segment.get_segment_manager());
        if (const ShmemString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->contains(k)) {
            return static_cast<T>(_counterMap->at(k).value);
        }
        return static_cast<T>(0);
    }
}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_SHARED_MEMORY_UTILS_H
