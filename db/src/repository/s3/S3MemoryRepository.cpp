//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/s3/S3MemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex S3MemoryRepository::_bucketMutex;
    boost::mutex S3MemoryRepository::_objectMutex;

    bool S3MemoryRepository::BucketExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_buckets,
                                    [region, name](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                        return bucket.second.region == region && bucket.second.name == name;
                                    }) != _buckets.end();
    }

    bool S3MemoryRepository::BucketExists(const Entity::S3::Bucket &bucket) const {
        return BucketExists(bucket.region, bucket.name);
    }

    bool S3MemoryRepository::BucketExists(const std::string &bucketArn) const {

        return std::ranges::find_if(_buckets,
                                    [bucketArn](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                        return bucket.second.arn == bucketArn;
                                    }) != _buckets.end();
    }

    Entity::S3::Bucket S3MemoryRepository::GetBucketById(const std::string &oid) const {

        const auto it = std::ranges::find_if(_buckets, [oid](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
            return bucket.first == oid;
        });

        if (it != _buckets.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::S3::Bucket S3MemoryRepository::GetBucketById(const bsoncxx::oid &oid) const {
        return GetBucketById(oid.to_string());
    }

    Entity::S3::Bucket S3MemoryRepository::GetBucketByRegionName(const std::string &region, const std::string &name) const {

        const auto it = std::ranges::find_if(_buckets,
                                             [region, name](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                                 return bucket.second.region == region && bucket.second.name == name;
                                             });
        if (it != _buckets.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::S3::Bucket S3MemoryRepository::GetBucketByArn(const std::string &bucketArn) const {

        const auto it = std::ranges::find_if(_buckets,
                                             [bucketArn](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                                 return bucket.second.arn == bucketArn;
                                             });

        if (it != _buckets.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::S3::Bucket S3MemoryRepository::CreateBucket(Entity::S3::Bucket &bucket) const {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _buckets[oid] = bucket;
        log_trace << "Bucket created, oid: " << oid;
        return GetBucketById(oid);
    }

    Entity::S3::BucketList S3MemoryRepository::ListBuckets(const std::string &region, const std::string &prefix, long maxResults, long skip, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_bucketMutex);

        auto q = Core::from(_buckets | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Bucket &b) { return b.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::S3::Bucket &b) { return Core::StringUtils::StartsWith(b.name, prefix); });
        }
        for (const auto &col: sortColumns) {
            q = q.order_by([col](const Entity::S3::Bucket &a, const Entity::S3::Bucket &b) {
                if (col.column == "name") {
                    return col.sortDirection == 1 ? a.name < b.name : b.name < a.name;
                }
                if (col.column == "size") {
                    return col.sortDirection == 1 ? a.size < b.size : b.size < a.size;
                }
                if (col.column == "keys") {
                    return col.sortDirection == 1 ? a.keys < b.keys : b.keys < a.keys;
                }
                return false;
            });
        }
        auto result = q.to_vector();
        if (skip > 0) {
            result = result | std::views::drop(skip) | std::ranges::to<std::vector>();
        }
        if (maxResults > 0) {
            result = result | std::views::take(maxResults) | std::ranges::to<std::vector>();
        }
        return result;
    }

    Entity::S3::BucketList S3MemoryRepository::ExportBuckets(const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_buckets | std::views::values | std::ranges::to<std::vector>());
        for (const auto &col: sortColumns) {
            q = q.order_by([col](const Entity::S3::Bucket &key1, const Entity::S3::Bucket &key2) {
                if (col.column == "name") {
                    return col.sortDirection == 1 ? key1.name < key2.name : key2.name < key1.name;
                }
                if (col.column == "size") {
                    return col.sortDirection == 1 ? key1.size < key2.size : key2.size < key1.size;
                }
                if (col.column == "keys") {
                    return col.sortDirection == 1 ? key1.keys < key2.keys : key2.keys < key1.keys;
                }
                return false;
            });
        }
        return q.to_vector();
    }

    bool S3MemoryRepository::HasObjects(const Entity::S3::Bucket &bucket) const {
        return std::ranges::count_if(_objects, [bucket](const auto &object) {
                   return object.second.region == bucket.region && object.second.bucket == bucket.name;
               }) > 0;
    }

    std::vector<Entity::S3::Object> S3MemoryRepository::GetBucketObjectList(const std::string &region, const std::string &bucket, const long maxKeys) const {

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Object &object) { return object.region == region; });
        }
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &object) { return object.bucket == bucket; });
        }
        if (maxKeys > 0) {
            return q | std::views::take(maxKeys) | std::ranges::to<std::vector>();
        }
        return q.to_vector();
    }

    long S3MemoryRepository::GetBucketObjectCount(const std::string &region, const std::string &bucket) const {

        return std::ranges::count_if(_objects, [region, bucket](const auto &object) {
            return object.second.region == region && object.second.bucket == bucket;
        });
    }

    long S3MemoryRepository::GetBucketSize(const std::string &region, const std::string &bucket) const {
        return std::accumulate(_objects.begin(), _objects.end(), 0L,
                               [region, bucket](long sum, const auto &item) {
                                   return item.second.region == region && item.second.bucket == bucket ? sum + item.second.size : sum;
                               });
    }

    long S3MemoryRepository::BucketCount(const std::string &region, const std::string &prefix) const {

        return static_cast<long>(_buckets.size());
    }

    std::vector<Entity::S3::Object> S3MemoryRepository::ListBucket(const std::string &bucket, const std::string &prefix) const {

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &item) { return item.bucket == bucket; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::S3::Object &item) { return Core::StringUtils::StartsWith(item.key, prefix); });
        }

        log_trace << "Got object list, size: " << q.count();
        return q.to_vector();
    }

    long S3MemoryRepository::PurgeBucket(Entity::S3::Bucket &bucket) const {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto count = std::erase_if(_objects, [bucket](const auto &item) {
            return item.second.region == bucket.region && item.second.bucket == bucket.name;
        });
        return static_cast<long>(count);
    }

    Entity::S3::Bucket S3MemoryRepository::UpdateBucket(Entity::S3::Bucket &bucket) const {
        boost::mutex::scoped_lock lock(_bucketMutex);
        const auto it = std::ranges::find_if(_buckets,
                                             [bucket](const std::pair<std::string, Entity::S3::Bucket> &b) {
                                                 return b.second.region == bucket.region && b.second.name == bucket.name;
                                             });
        _buckets[it->first] = bucket;
        return _buckets[it->first];
    }

    void S3MemoryRepository::UpdateBucketCounter(const std::string &bucketArn, const long keys, const long size) const {

        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto it = std::ranges::find_if(_buckets,
                                             [bucketArn](const std::pair<std::string, Entity::S3::Bucket> &b) {
                                                 return b.second.arn == bucketArn;
                                             });
        if (it != _buckets.end()) {
            it->second.keys = keys;
            it->second.size = size;
        }
    }

    void S3MemoryRepository::DeleteBucket(const Entity::S3::Bucket &bucket) const {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto count = std::erase_if(_buckets, [bucket](const auto &item) {
            return item.second.region == bucket.region && item.second.name == bucket.name;
        });
        log_debug << "Bucket deleted, count: " << count;
    }

    long S3MemoryRepository::DeleteAllBuckets() const {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const long count = static_cast<long>(_buckets.size());
        log_debug << "All buckets deleted, count: " << _buckets.size();
        _buckets.clear();
        return count;
    }

    bool S3MemoryRepository::ObjectExists(const Entity::S3::Object &object) const {

        return std::ranges::find_if(_objects,
                                    [object](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.region == object.region && o.second.bucket == object.bucket && o.second.key == object.key;
                                    }) != _objects.end();
    }

    bool S3MemoryRepository::ObjectExists(const std::string &oid) const {

        return std::ranges::find_if(_objects,
                                    [oid](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.oid == oid;
                                    }) != _objects.end();
    }

    bool S3MemoryRepository::ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const {

        return std::ranges::find_if(_objects,
                                    [region, bucket, key](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.region == region && o.second.bucket == bucket && o.second.key == key;
                                    }) != _objects.end();
    }

    bool S3MemoryRepository::ObjectExistsInternalName(const std::string &filename) const {

        return std::ranges::find_if(_objects,
                                    [filename](const std::pair<std::string, Entity::S3::Object> &object) {
                                        return object.second.internalName == filename;
                                    }) != _objects.end();
    }

    Entity::S3::Object S3MemoryRepository::CreateObject(Entity::S3::Object &object) const {
        boost::mutex::scoped_lock lock(_objectMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _objects[oid] = object;
        log_trace << "Object created, oid: " << oid;
        return GetObjectById(oid);
    }

    Entity::S3::Object S3MemoryRepository::UpdateObject(Entity::S3::Object &object) const {
        boost::mutex::scoped_lock lock(_objectMutex);

        const auto it = std::ranges::find_if(_objects,
                                             [object](const std::pair<std::string, Entity::S3::Object> &o) {
                                                 return o.second.bucket == object.bucket && o.second.key == object.key;
                                             });
        if (it != _objects.end()) {
            _objects[it->first] = object;
            return _objects[it->first];
        }
        return {};
    }

    Entity::S3::Object S3MemoryRepository::GetObjectById(const std::string &oid) const {

        const auto it = std::ranges::find_if(_objects, [oid](const std::pair<std::string, Entity::S3::Object> &object) {
            return object.first == oid;
        });

        if (it != _objects.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::S3::Object S3MemoryRepository::GetObject(const std::string &region, const std::string &bucket, const std::string &key) const {

        const auto it = std::ranges::find_if(_objects,
                                             [region, bucket, key](const std::pair<std::string, Entity::S3::Object> &object) {
                                                 return object.second.region == region && object.second.bucket == bucket && object.second.key == key;
                                             });

        if (it != _objects.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::S3::Object S3MemoryRepository::GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) const {

        const auto it = std::ranges::find_if(_objects,
                                             [region, bucket, key, md5sum](const std::pair<std::string, Entity::S3::Object> &object) {
                                                 return object.second.region == region && object.second.bucket == bucket && object.second.key == key && object.second.md5sum == md5sum;
                                             });

        if (it != _objects.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    long S3MemoryRepository::ObjectCount(const std::string &region, const std::string &prefix, const std::string &bucket) const {
        boost::mutex::scoped_lock lock(_objectMutex);

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Object &object) { return object.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::S3::Object &object) { return Core::StringUtils::StartsWith(object.key, prefix); });
        }
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &object) { return object.bucket == bucket; });
        }
        log_debug << "Object count, region: " << region << ", bucket: " << bucket << ", count: " << q.count();
        return static_cast<long>(q.count());
    }

    std::vector<Entity::S3::Object> S3MemoryRepository::ListObjects(const std::string &region, const std::string &prefix, const std::string &bucket, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Object &object) { return object.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::S3::Object &object) { return Core::StringUtils::StartsWith(object.key, prefix); });
        }
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &object) { return object.bucket == bucket; });
        }
        if (!sortColumns.empty()) {
            std::ranges::sort(q.to_vector(), [sortColumns](const Entity::S3::Object &a, const Entity::S3::Object &b) {
                for (const auto &sc: sortColumns) {
                    if (sc.column == "bucket") {
                        return sc.sortDirection == 1 ? a.bucket < b.bucket : b.bucket < a.bucket;
                    }
                    if (sc.column == "size") {
                        return sc.sortDirection == 1 ? a.size < b.size : b.size < a.size;
                    }
                    if (sc.column == "created") {
                        return sc.sortDirection == 1 ? a.created < b.created : b.created < a.created;
                    }
                    if (sc.column == "modified") {
                        return sc.sortDirection == 1 ? a.modified < b.modified : b.modified < a.modified;
                    }
                }
                return false;
            });
        }
        log_trace << "Objects list, prefix: " << prefix << ", count: " << q.count();
        return Core::PageVector(q.to_vector(), pageSize, pageIndex);
    }

    std::vector<Entity::S3::Object> S3MemoryRepository::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Object &object) { return object.region == region; });
        }
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &object) { return object.bucket == bucket; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::S3::Object &object) { return Core::StringUtils::StartsWith(object.key, prefix); });
        }
        log_trace << "Versioned object list, region: " << region << ", bucket: " << bucket << ", size: " << q.count();
        return q.to_vector();
    }

    long S3MemoryRepository::DeleteObject(const Entity::S3::Object &object) const {
        boost::mutex::scoped_lock lock(_objectMutex);

        const auto count = std::erase_if(_objects, [object](const auto &item) {
            return item.second.bucket == object.bucket && item.second.key == object.key;
        });
        log_debug << "Object deleted, count: " << count;
        return count;
    }

    long S3MemoryRepository::DeleteObjects(const std::string &region, const std::string &bucket, const std::vector<std::string> &keys) const {
        const std::unordered_set keySet(keys.begin(), keys.end());
        long count = 0;
        {
            // Minimal Lock Zone
            boost::mutex::scoped_lock lock(_objectMutex);

            count = static_cast<long>(std::erase_if(_objects, [&](const auto &item) {
                return item.second.region == region && item.second.bucket == bucket && keySet.contains(item.second.key);
            }));
        }
        log_debug << "Objects deleted, count: " << count;
        return count;
    }

    long S3MemoryRepository::DeleteAllObjects() const {
        boost::mutex::scoped_lock lock(_objectMutex);

        const long count = static_cast<long>(_objects.size());
        log_debug << "Deleting objects, size: " << _objects.size();
        _objects.clear();
        return count;
    }

    Entity::S3::Object S3MemoryRepository::GetObjectById(bsoncxx::oid oid) const {
        return GetObjectById(oid.to_string());
    }

    Entity::S3::Bucket S3MemoryRepository::CreateOrUpdateBucket(Entity::S3::Bucket &bucket) const {
        if (BucketExists(bucket)) {
            return UpdateBucket(bucket);
        }
        return CreateBucket(bucket);
    }

    Entity::S3::Object S3MemoryRepository::CreateOrUpdateObject(Entity::S3::Object &object) const {
        if (ObjectExists(object)) {
            return UpdateObject(object);
        }
        return CreateObject(object);
    }

    Entity::S3::Object S3MemoryRepository::GetObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version) const {

        const auto it = std::ranges::find_if(_objects,
                                             [region, bucket, key, version](const std::pair<std::string, Entity::S3::Object> &object) {
                                                 return object.second.region == region && object.second.bucket == bucket && object.second.key == key && object.second.versionId == version;
                                             });
        if (it != _objects.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::S3::Bucket S3MemoryRepository::CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {
        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);
        log_debug << "Bucket notification added, notification: " << bucketNotification.ToString();
        return UpdateBucket(internBucket);
    }

    Entity::S3::Bucket S3MemoryRepository::DeleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {
        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);
        log_trace << "Bucket notification deleted, notification: " << bucketNotification.ToString();
        return UpdateBucket(internBucket);
    }

    void S3MemoryRepository::AdjustObjectCounters() const {
        for (const auto &bucket: _buckets | std::views::values) {
            _buckets[bucket.oid].keys = ObjectCount(bucket.region, {}, bucket.name);
        }
        log_debug << "All object counters updated, count: " << _buckets.size();
    }
}// namespace Awsmock::Database
