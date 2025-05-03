//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/core/SharedMemoryUtils.h"


#include <awsmock/service/s3/S3Server.h>

namespace AwsMock::Service {

    S3Server::S3Server(Core::PeriodicScheduler &scheduler) : AbstractServer("s3") {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _syncPeriod = configuration.GetValue<int>("awsmock.modules.s3.sync.period");
        _counterPeriod = configuration.GetValue<int>("awsmock.modules.s3.counter.period");

        // Check module active
        if (!IsActive("s3")) {
            log_info << "S3 module inactive";
            return;
        }

        // Initialize shared memory
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        _s3CounterMap = _segment.find<Database::S3CounterMapType>(Database::S3_COUNTER_MAP_NAME).first;

        // Start S3 monitoring counters updates
        scheduler.AddTask("s3-counter-updates", [this] { UpdateCounter(); }, _counterPeriod);

        // Start synchronization of objects
        scheduler.AddTask("s3-sync-objects", [this] { SyncObjects(); }, _syncPeriod);

        log_debug << "S3 server initialized";
    }

    void S3Server::SyncObjects() const {

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");

        const Database::Entity::S3::BucketList buckets = _s3Database.ListBuckets();
        log_trace << "Object synchronization starting, bucketCount: " << buckets.size();

        if (buckets.empty()) {
            return;
        }

        // Loop over buckets and do some maintenance work
        int filesDeleted = 0, objectsDeleted = 0;
        for (auto &bucket: buckets) {
            // Get objects and delete objects, where the file is not existing anymore, The files are identified by internal name.
            for (std::vector objects = _s3Database.GetBucketObjectList(region, bucket.name, 1000); const auto &object: objects) {
                if (!Core::FileUtils::FileExists(s3DataDir + "/" + object.internalName)) {
                    _s3Database.DeleteObject(object);
                    log_debug << "Object deleted, internalName: " << object.internalName;
                    objectsDeleted++;
                }
            }
        }

        // Loop over files and check the database for internal name
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

    void S3Server::UpdateCounter() {
        log_trace << "S3 Monitoring starting";

        if (_s3CounterMap) {
            long totalKeys = 0;
            long totalSize = 0;
            for (auto const &[key, val]: *_s3CounterMap) {

                std::string labelValue = key;
                Core::StringUtils::Replace(labelValue, "-", "_");

                _metricService.SetGauge(S3_OBJECT_BY_BUCKET_COUNT, "bucket", labelValue, static_cast<double>(val.keys));
                _metricService.SetGauge(S3_SIZE_BY_BUCKET_COUNT, "bucket", labelValue, static_cast<double>(val.size));

                totalKeys += val.keys;
                totalSize += val.size;
                _s3Database.UpdateBucketCounter(key, val.keys, val.size);
            }
            _metricService.SetGauge(S3_BUCKET_COUNT, static_cast<double>(_s3CounterMap->size()));
            _metricService.SetGauge(S3_OBJECT_COUNT, static_cast<double>(totalKeys));
        }
        log_debug << "S3 monitoring finished, freeShmSize: " << _segment.get_free_memory();
    }
}// namespace AwsMock::Service
