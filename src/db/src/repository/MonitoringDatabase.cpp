
//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/MonitoringDatabase.h>

namespace AwsMock::Database {

    typedef boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean>> Accumulator;

    MonitoringDatabase::MonitoringDatabase() : _databaseName(GetDatabaseName()), _monitoringCollectionName("monitoring"), _rollingMean(Core::Configuration::instance().GetValueBool("awsmock.monitoring.smooth")) {}

    void MonitoringDatabase::IncCounter(const std::string &name, double value, const std::string &labelName, const std::string &labelValue) const {
        log_trace << "Set counter value, name: " << name << " value: " << value << " labelName: " << labelName << " labelValue:" << labelValue;

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];
            auto session = client->start_session();

            try {

                document searchDocument;
                searchDocument.append(kvp("name", name));

                if (!labelName.empty()) {
                    searchDocument.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    searchDocument.append(kvp("labelValue", labelValue));
                }

                if (const auto mResult = _monitoringCollection.find_one(searchDocument.extract())) {
                    value += mResult.value()["value"].get_double().value;
                }

                session.start_transaction();

                document document;
                document.append(kvp("name", name));
                if (!labelName.empty()) {
                    document.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    document.append(kvp("labelValue", labelValue));
                }
                document.append(kvp("value", value));

                // As mongoDB uses UTC timestamps, we need to calculate everything in UTC
                document.append(kvp("created", bsoncxx::types::b_date(Core::DateTimeUtils::UtcDateTimeNow())));

                auto insert_one_result = _monitoringCollection.insert_one(document.extract());
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            log_trace << "Performance counter not available if you running the memory DB";
        }
    }

    void MonitoringDatabase::SetGauge(const std::string &name, double value, const std::string &labelName, const std::string &labelValue) const {
        log_trace << "Set gauge value, name: " << name << " value: " << value << " labelName: " << labelName << " labelValue:" << labelValue;

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];
            auto session = client->start_session();

            try {

                // As mongoDB uses UTC timestamps, we need to calculate everything in UTC
                const system_clock::time_point now = Core::DateTimeUtils::UtcDateTimeNow();
                const system_clock::time_point end = Core::ceilTimePoint(now, std::chrono::seconds(300));
                const system_clock::time_point middle = end - std::chrono::seconds(150);

                document document;
                document.append(kvp("name", name));
                document.append(kvp("created", bsoncxx::types::b_date(middle)));
                if (!labelName.empty()) {
                    document.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    document.append(kvp("labelValue", labelValue));
                }

                if (name.empty()) {
                    log_error << "Missing name";
                }
                session.start_transaction();
                if (const auto mResult = _monitoringCollection.find_one(document.extract())) {

                    double currentValue = mResult.value()["value"].get_double().value;
                    int currentCount = mResult.value()["count"].get_int32().value;
                    currentValue += value / static_cast<double>(++currentCount);
                    _monitoringCollection.update_one(make_document(kvp("_id", mResult.value()["_id"].get_oid())),
                                                     make_document(kvp("$set", make_document(kvp("value", currentValue), kvp("count", currentCount)))));

                } else {
                    if (name.empty()) {
                        log_error << "Missing name";
                    }
                    bsoncxx::builder::basic::document newDocument;
                    newDocument.append(kvp("value", value));
                    newDocument.append(kvp("count", 1));
                    newDocument.append(kvp("name", name));
                    newDocument.append(kvp("created", bsoncxx::types::b_date(middle)));
                    if (!labelName.empty()) {
                        newDocument.append(kvp("labelName", labelName));
                    }
                    if (!labelValue.empty()) {
                        newDocument.append(kvp("labelValue", labelValue));
                    }
                    _monitoringCollection.insert_one(newDocument.extract());
                }
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            log_trace << "Performance counter not available if you running the memory DB";
        }
    }

    std::vector<Entity::Monitoring::Counter> MonitoringDatabase::GetMonitoringValues(const std::string &name, const system_clock::time_point start, const system_clock::time_point end, const int step, const std::string &labelName, const std::string &labelValue) const {
        log_trace << "Get monitoring values, name: " << name << " start: " << start << " end: " << end << " step: " << step << " labelName: " << labelName << " labelValue:" << labelValue;

        const long offset = Core::DateTimeUtils::UtcOffset();

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];

            try {
                std::vector<Entity::Monitoring::Counter> result;

                mongocxx::options::find opts;
                opts.sort(make_document(kvp("created", 1)));

                // As mongoDB uses UTC timestamps, we need to convert everything to UTC
                auto startUtc = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::from_sys(start).time_since_epoch());
                auto endUtc = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::from_sys(end).time_since_epoch());

                document document;
                document.append(kvp("name", name));
                document.append(kvp("created", make_document(kvp("$gte", bsoncxx::types::b_date(startUtc)))), kvp("created", make_document(kvp("$lte", bsoncxx::types::b_date(endUtc)))));
                if (!labelName.empty()) {
                    document.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    document.append(kvp("labelValue", labelValue));
                }

                // Find and accumulate
                if (_rollingMean) {
                    Accumulator acc(boost::accumulators::tag::rolling_window::window_size = step);
                    for (auto cursor = _monitoringCollection.find(document.extract(), opts); auto it: cursor) {
                        acc(it["value"].get_double().value);
                        Entity::Monitoring::Counter counter = {.name = name, .performanceValue = boost::accumulators::mean(acc), .timestamp = bsoncxx::types::b_date(it["created"].get_date().value)};
                        result.emplace_back(counter);
                    }
                } else {
                    for (auto cursor = _monitoringCollection.find(document.extract(), opts); auto it: cursor) {
                        Entity::Monitoring::Counter counter = {.name = name, .performanceValue = it["value"].get_double().value, .timestamp = bsoncxx::types::b_date(it["created"].get_date().value)};
                        result.emplace_back(counter);
                    }
                }
                log_debug << "Counters, name: " << name << " count: " << result.size();
                return result;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        log_trace << "Performance counter not available if you running the memory DB";
        return {};
    }

    long MonitoringDatabase::DeleteOldMonitoringData(const int retentionPeriod) const {
        log_trace << "Deleting old monitoring data, retention:: " << retentionPeriod;

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];
            auto session = client->start_session();

            try {
                // Find and delete counters
                session.start_transaction();
                const auto retention = Core::DateTimeUtils::UtcDateTimeNow() - std::chrono::hours(retentionPeriod * 24);
                const auto mResult = _monitoringCollection.delete_many(make_document(kvp("created", make_document(kvp("$lte", bsoncxx::types::b_date(retention))))));
                log_debug << "Counters deleted, count: " << mResult.value().deleted_count();
                session.commit_transaction();
                return mResult.value().deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        log_trace << "Performance counter not available if you running the memory DB";
        return 0;
    }
}// namespace AwsMock::Database
