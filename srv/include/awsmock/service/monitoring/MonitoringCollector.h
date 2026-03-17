//
// Created by jensv on 17/03/2026.
//

#ifndef AWSMOCK_MONITORING_COLLECTOR_H
#define AWSMOCK_MONITORING_COLLECTOR_H

// C++ includes
#include <mutex>

// Boost includes
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

namespace AwsMock::Monitoring {

    class MonitoringCollector {
    public:
        /**
         * @brief Constructor
         *
         * @param io boost IO context
         */
        MonitoringCollector(boost::asio::io_context &io) : timer_(io) {
            _period = Core::Configuration::instance().GetValue<long>("awsmock.monitoring.average-period");
            StartReportingCycle();
        }

        // This is the "Push" entry point called by the signal
        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) {
            std::lock_guard lock(_monitoringMutex);
            _collectorMap[GetId(name, labelName, labelValue)](value);
        }

    private:
        std::map<std::string, boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean> > > _collectorMap;
        boost::asio::steady_timer timer_;
        std::mutex _monitoringMutex;

        void StartReportingCycle() {
            timer_.expires_after(boost::asio::chrono::seconds(_period));
            timer_.async_wait([this](const boost::system::error_code &ec) {
                if (!ec) {
                    ProcessAndSend();
                    StartReportingCycle();
                }
            });
        }

        void ProcessAndSend() {
            std::map<std::string, double> values;
            for (auto &[fst, snd]: _collectorMap) {
                std::lock_guard lock(_monitoringMutex);
                if (boost::accumulators::count(snd) > 0) {
                    values[fst] = boost::accumulators::mean(snd);
                    snd = {};
                }
            }
            Core::EventBus::instance().sigCollector(values);
        }

        /**
         * @brief Creates a metric ID
         *
         * @param name metric name
         * @param labelName label name
         * @param labelValue  label value
         * @return metric ID
         */
        static std::string GetId(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Average time in seconds
         */
        long _period{};
    };

    inline std::string MonitoringCollector::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }

} // namespace AwsMock::Monitoring

#endif // AWSMOCK_MONITORING_COLLECTOR_H
