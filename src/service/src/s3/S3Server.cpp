//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/s3/S3Server.h>

namespace AwsMock::Service {

    S3Server::S3Server(Core::PeriodicScheduler &scheduler) : AbstractServer("s3") {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _monitoringPeriod = configuration.GetValueInt("awsmock.modules.s3.monitoring.period");
        _syncPeriod = configuration.GetValueInt("awsmock.modules.s3.sync.object.period");
        _sizePeriod = configuration.GetValueInt("awsmock.modules.s3.sync.bucket.period");

        // Check module active
        if (!IsActive("s3")) {
            log_info << "S3 module inactive";
            return;
        }

        // Start S3 monitoring counters updates
        scheduler.AddTask("s3-monitoring-counters", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start synchronization of objects
        scheduler.AddTask("s3-sync-objects", [this] { SyncObjects(); }, _syncPeriod);

        // Start synchronization of buckets
        scheduler.AddTask("s3-sync-buckets", [this] { SyncBuckets(); }, _syncPeriod);

        log_debug << "S3 server initialized";
    }

    void S3Server::SyncObjects() const {

        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");
        const std::string s3DataDir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");

        Database::Entity::S3::BucketList buckets = _s3Database.ListBuckets();
        log_trace << "Object synchronization starting, bucketCount: " << buckets.size();

        if (buckets.empty()) {
            return;
        }

        // Loop over buckets and do some maintenance work
        int filesDeleted = 0, objectsDeleted = 0;
        for (auto &bucket: buckets) {
            // Get objects and delete object, where the file is not existing anymore
            for (std::vector objects = _s3Database.GetBucketObjectList(region, bucket.name, 1000); const auto &object: objects) {
                if (!Core::FileUtils::FileExists(s3DataDir + Core::FileUtils::separator() + object.internalName)) {
                    _s3Database.DeleteObject(object);
                    log_debug << "Object deleted, internalName: " << object.internalName;
                    objectsDeleted++;
                }
            }
        }

        // Loop over files and check database for internal name
        if (const path p(s3DataDir); is_directory(p)) {
            for (auto &entry: boost::make_iterator_range(directory_iterator(p), {})) {
                if (!_s3Database.ObjectExistsInternalName(Core::FileUtils::GetBasename(entry.path().string()))) {
                    Core::FileUtils::DeleteFile(entry.path().string());
                    log_debug << "File deleted, filename: " << entry.path().string();
                    filesDeleted++;
                }
            }
        }
        log_debug << "Object synchronized finished, bucketCount: " << buckets.size() << " fileDeleted: " << filesDeleted << " objectsDeleted: " << objectsDeleted;
    }

    void S3Server::SyncBuckets() const {
        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");

        Database::Entity::S3::BucketList buckets = _s3Database.ListBuckets();
        log_trace << "Bucket synchronization starting, bucketCount: " << buckets.size();

        if (buckets.empty()) {
            return;
        }

        // Loop over buckets and adjust bucket counters
        for (auto &bucket: buckets) {
            _s3Database.AdjustBucketCounters(region, bucket.name);
        }
        log_debug << "Bucket synchronized finished, bucketCount: " << buckets.size();
    }

    void S3Server::UpdateCounter() const {
        log_trace << "S3 Monitoring starting";

        const long buckets = _s3Database.BucketCount();
        const long objects = _s3Database.ObjectCount();
        _metricService.SetGauge(S3_BUCKET_COUNT, static_cast<double>(buckets));
        _metricService.SetGauge(S3_OBJECT_COUNT, static_cast<double>(objects));

        // Count resources per topic
        for (const auto &bucket: _s3Database.ListBuckets()) {
            std::string labelValue = bucket.name;
            Core::StringUtils::Replace(labelValue, "-", "_");
            const long objectsPerBuckets = _s3Database.ObjectCount(bucket.region, bucket.name);
            _metricService.SetGauge(S3_OBJECT_BY_BUCKET_COUNT, "bucket", labelValue, static_cast<double>(objectsPerBuckets));
        }
        log_trace << "S3 monitoring finished";
    }
}// namespace AwsMock::Service
