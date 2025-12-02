//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/s3/S3Server.h>

namespace AwsMock::Service {

    S3Server::S3Server(Core::Scheduler &scheduler) : AbstractServer("s3"), _monitoringCollector(Core::MonitoringCollector::instance()) {

        // Get HTTP configuration values
        _syncPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.s3.sync-period");
        _counterPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.s3.counter-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.s3.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.backup.cron");

        // Check module active
        if (!IsActive("s3")) {
            log_info << "S3 module inactive";
            return;
        }

        // Start S3 monitoring counters updates
        scheduler.AddTask("s3-monitoring", [this] { UpdateCounter(); }, _counterPeriod, _counterPeriod);

        // Start synchronization of objects
        scheduler.AddTask("s3-sync-objects", [this] { SyncObjects(); }, _syncPeriod, _syncPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("s3-backup", [] { BackupS3(); }, _backupCron);
        }

        // Set running
        SetRunning();

        log_debug << "S3 server initialized";
    }

    void S3Server::SyncObjects() const {

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");

        // Get the bucket list
        const Database::Entity::S3::BucketList buckets = _s3Database.ListBuckets();
        log_trace << "Object synchronization starting, bucketCount: " << buckets.size();

        if (buckets.empty()) {
            return;
        }

        // Loop over buckets and do some maintenance work
        int filesDeleted = 0, objectsDeleted = 0;
        for (auto &bucket: buckets) {
            // Get objects and delete objects, where the file is not existing anymore, The files are identified by internal name.
            std::vector objects = _s3Database.GetBucketObjectList(region, bucket.name, 1000);
            while (!objects.empty()) {
                for ( const auto &object: objects) {
                    if (!Core::FileUtils::FileExists(s3DataDir + Core::FileUtils::separator() + object.internalName)) {
                        _s3Database.DeleteObject(object);
                        log_debug << "Object deleted, internalName: " << object.internalName;
                        objectsDeleted++;
                    }
                }
                objects = _s3Database.GetBucketObjectList(region, bucket.name, 1000);
            }
        }

        // Loop over files and check the database for internal name
        if (const path p(s3DataDir); is_directory(p)) {
            for (auto &entry: boost::make_iterator_range(directory_iterator(p), {})) {
                if (!_s3Database.ObjectExistsInternalName(Core::FileUtils::GetBasename(entry.path().string()))) {
                    Core::FileUtils::RemoveFile(entry.path().string());
                    log_debug << "File deleted, filename: " << entry.path().string();
                    filesDeleted++;
                }
            }
        }
        log_debug << "Object synchronized finished, bucketCount: " << buckets.size() << " fileDeleted: " << filesDeleted << " objectsDeleted: " << objectsDeleted;
    }

    void S3Server::UpdateCounter() const {

        log_trace << "S3 Monitoring starting";

        long totalKeys = 0;
        long totalSize = 0;

        // Reload the counters first
        _s3Database.AdjustObjectCounters();

        const Database::Entity::S3::BucketList buckets = _s3Database.ListBuckets();
        for (const auto &bucket: buckets) {

            _monitoringCollector.SetGauge(S3_OBJECT_BY_BUCKET_COUNT, "bucket", bucket.name, static_cast<double>(bucket.keys));
            _monitoringCollector.SetGauge(S3_SIZE_BY_BUCKET_COUNT, "bucket", bucket.name, static_cast<double>(bucket.size));
            totalKeys += bucket.keys;
            totalSize += bucket.size;
        }
        _monitoringCollector.SetGauge(S3_BUCKET_COUNT, {}, {}, static_cast<double>(buckets.size()));
        _monitoringCollector.SetGauge(S3_BUCKET_SIZE_COUNT, {}, {}, static_cast<double>(totalSize));
        _monitoringCollector.SetGauge(S3_OBJECT_COUNT, {}, {}, static_cast<double>(totalKeys));
        log_debug << "S3 monitoring finished";
    }

    void S3Server::BackupS3()  {

        // Backup S3 buckets and objects
        ModuleService::BackupModule("s3", true);

    }

}// namespace AwsMock::Service
