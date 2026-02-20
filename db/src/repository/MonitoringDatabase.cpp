
//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/MonitoringDatabase.h>

namespace AwsMock::Database {

    typedef boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean> > Accumulator;

    MonitoringDatabase::MonitoringDatabase() : _databaseName(GetDatabaseName()), _monitoringCollectionName("monitoring"), _rollingMean(Core::Configuration::instance().GetValue<bool>("awsmock.monitoring.smooth")) {
    }

    std::vector<std::string> MonitoringDatabase::GetDistinctLabelValues(const std::string &name, const std::string &labelName, const long limit) const {
        log_trace << "Get distinct label values, labelName: " << labelName;

        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];

            try {
                std::vector<std::string> labels;

                document query;
                query.append(kvp("name", name));
                query.append(kvp("labelName", labelName));

                if (limit > 0) {
                    mongocxx::pipeline p{};
                    p.match(make_document(kvp("name", name), kvp("labelName", labelName)));
                    p.group(make_document(kvp("_id", "$labelValue"), kvp("totalSize", make_document(kvp("$sum", "$value")))));
                    p.sort(make_document(kvp("totalSize", -1)));
                    p.limit(static_cast<std::int32_t>(limit));
                    p.project(make_document(kvp("_id", "$_id"), kvp("value", "$labelValue"), kvp("total", "$totalSize")));
                    for (auto t = _monitoringCollection.aggregate(p); const auto s: t) {
                        labels.emplace_back(s["_id"].get_string().value);
                    }
                } else {

                    for (auto cursor = _monitoringCollection.distinct("labelValue", query.extract()); view doc: cursor) {
                        for (const view eventsView = doc["values"].get_array().value; const bsoncxx::document::element &element: eventsView) {
                            labels.emplace_back(element.get_string().value);
                        }
                    }
                }
                return labels;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        log_trace << "Performance counter not available if you running the memory DB";
        return {};
    }

    void MonitoringDatabase::IncCounter(const std::string &name, double value, const std::string &labelName, const std::string &labelValue) const {
        log_trace << "Set counter value, name: " << name << ", value: " << value << ", labelName: " << labelName << ", labelValue:" << labelValue;

        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];
            auto session = client->start_session();

            try {
                if (name.empty()) {
                    log_error << "Missing name";
                    return;
                }

                document newDocument;
                newDocument.append(kvp("value", value));
                newDocument.append(kvp("count", 1));
                newDocument.append(kvp("name", name));
                if (!labelName.empty()) {
                    newDocument.append(kvp("labelName", labelName));
                }
                if (!labelName.empty()) {
                    newDocument.append(kvp("labelValue", labelValue));
                }
                newDocument.append(kvp("created", bsoncxx::types::b_date(system_clock::now())));

                session.start_transaction();
                _monitoringCollection.insert_one(newDocument.extract());
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
                if (name.empty()) {
                    log_error << "Missing name";
                    return;
                }

                document newDocument;
                newDocument.append(kvp("value", value));
                newDocument.append(kvp("count", 1));
                newDocument.append(kvp("name", name));
                newDocument.append(kvp("created", bsoncxx::types::b_date(Core::DateTimeUtils::UtcDateTimeNow())));
                if (!labelName.empty()) {
                    newDocument.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    newDocument.append(kvp("labelValue", labelValue));
                }

                session.start_transaction();
                _monitoringCollection.insert_one(newDocument.extract());
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

    std::vector<Entity::Monitoring::Counter> MonitoringDatabase::GetMonitoringValues(const std::string &name, const system_clock::time_point start, const system_clock::time_point end, const long step, const std::string &labelName,
                                                                                     const std::string &labelValue, const long limit) const {
        log_trace << "Get monitoring values, name: " << name << ", start: " << start << ", end: " << end << ", step: " << step << ", labelName: " << labelName << ", labelValue: " << labelValue;

        using namespace std::literals;
        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _monitoringCollection = (*client)[_databaseName][_monitoringCollectionName];

            try {
                // As mongoDB uses UTC timestamps, we need to convert everything to UTC
#ifdef __APPLE__
                const std::chrono::time_point startTime = std::chrono::time_point_cast<std::chrono::microseconds>(start);
                auto startUtc = bsoncxx::types::b_date(system_clock::time_point(startTime.time_since_epoch()));
                const std::chrono::time_point endTime = std::chrono::time_point_cast<std::chrono::microseconds>(end);
                auto endUtc = bsoncxx::types::b_date(system_clock::time_point(endTime.time_since_epoch()));
#else
                auto startUtc = bsoncxx::types::b_date(Core::DateTimeUtils::ConvertToUtc(start));
                auto endUtc = bsoncxx::types::b_date(Core::DateTimeUtils::ConvertToUtc(end));
#endif
                document document;
                document.append(kvp("name", name));
                document.append(kvp("created", make_document(kvp("$gte", startUtc))), kvp("created", make_document(kvp("$lte", endUtc))));
                if (!labelName.empty()) {
                    document.append(kvp("labelName", labelName));
                }
                if (!labelValue.empty()) {
                    document.append(kvp("labelValue", labelValue));
                }

                // Find and accumulate
                std::vector<Entity::Monitoring::Counter> result;
                if (_rollingMean) {
                    mongocxx::options::find opts;
                    opts.sort(make_document(kvp("created", 1)));
                    Accumulator acc(boost::accumulators::tag::rolling_window::window_size = step);
                    for (auto cursor = _monitoringCollection.find(document.extract(), opts); auto it: cursor) {
                        acc(it["value"].get_double().value);
                        Entity::Monitoring::Counter counter = {.name = name, .performanceValue = boost::accumulators::mean(acc), .timestamp = bsoncxx::types::b_date(it["created"].get_date().value)};
                        result.emplace_back(counter);
                    }
                } else {
                    for (auto cursor = _monitoringCollection.find(document.extract()); auto it: cursor) {
                        Entity::Monitoring::Counter counter = {.name = name, .performanceValue = it["value"].get_double().value, .timestamp = bsoncxx::types::b_date(it["created"].get_date().value)};
                        result.emplace_back(counter);
                    }
                }
                log_debug << "Counters, name: " << name << ", count: " << result.size() << ", start:" << startUtc << ", end: " << endUtc;
                return result;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        log_trace << "Performance counter not available if you running the memory DB";
        return {};
    }

    void MonitoringDatabase::UpdateMonitoringCounters() {

        if (HasDatabase()) {

            // Get the map of counters
            Core::ShmMap *counterMap = Core::MonitoringCollector::instance().GetCounterMap();

            const auto client = ConnectionPool::instance().GetConnection();
            auto monitoringCollection = client->database(_databaseName)[_monitoringCollectionName];
            auto session = client->start_session();

            try {

                std::vector<value> documents;
                for (auto &val: *counterMap | std::views::values) {

                    double databaseValue = 0.0;
                    if (val.type == Core::CounterType::COUNT_PER_SECOND || val.type == Core::CounterType::GAUGE) {
                        // Average over number of counters or measuring period
                        databaseValue = val.count > 0 ? val.value / static_cast<double>(val.count) : 0.0;
                    } else if (val.type == Core::CounterType::COUNT_ABSOLUTE) {
                        // Absolute value; will be kept
                        databaseValue = val.value;
                    }

                    // Prepare insert query
                    document insertQuery;
                    insertQuery.append(kvp("name", bsoncxx::types::b_string(val.name)));
                    insertQuery.append(kvp("labelName", bsoncxx::types::b_string(val.labelName)));
                    insertQuery.append(kvp("labelValue", bsoncxx::types::b_string(val.labelValue)));
                    insertQuery.append(kvp("value", databaseValue));
                    insertQuery.append(kvp("created", bsoncxx::types::b_date(val.timestamp)));
                    documents.emplace_back(insertQuery.extract());
                    val.count = 0;
                    val.value = 0;
                }

                // Execute bulk update
                long count{};
                if (!documents.empty()) {
                    session.start_transaction();
                    count += monitoringCollection.insert_many(documents)->inserted_count();
                    session.commit_transaction();
                }
                log_debug << "Imported monitoring values: " << count;

            } catch (mongocxx::exception &e) {
                log_error << "Collection transaction exception: " << e.what();
                session.abort_transaction();
                throw Core::DatabaseException(e.what());
            }
        }
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
                const auto retention = Core::DateTimeUtils::UtcDateTimeNow() - std::chrono::days(retentionPeriod);
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

} // namespace AwsMock::Database
