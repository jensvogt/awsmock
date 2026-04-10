//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/S3MemoryDb.h>

namespace AwsMock::Database {

    boost::mutex S3MemoryDb::_bucketMutex;
    boost::mutex S3MemoryDb::_objectMutex;

    bool S3MemoryDb::BucketExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_buckets,
                                    [region, name](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                        return bucket.second.region == region && bucket.second.name == name;
                                    }) != _buckets.end();
    }

    bool S3MemoryDb::BucketExists(const Entity::S3::Bucket &bucket) {
        return BucketExists(bucket.region, bucket.name);
    }

    bool S3MemoryDb::BucketExists(const std::string &bucketArn) const {

        return std::ranges::find_if(_buckets,
                                    [bucketArn](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
                                        return bucket.second.arn == bucketArn;
                                    }) != _buckets.end();
    }

    Entity::S3::Bucket S3MemoryDb::GetBucketById(const std::string &oid) {

        const auto it = std::ranges::find_if(_buckets, [oid](const std::pair<std::string, Entity::S3::Bucket> &bucket) {
            return bucket.first == oid;
        });

        if (it != _buckets.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::S3::Bucket S3MemoryDb::GetBucketByRegionName(const std::string &region, const std::string &name) {

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

    Entity::S3::Bucket S3MemoryDb::GetBucketByArn(const std::string &bucketArn) {

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

    Entity::S3::Bucket S3MemoryDb::CreateBucket(const Entity::S3::Bucket &bucket) {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _buckets[oid] = bucket;
        log_trace << "Bucket created, oid: " << oid;
        return GetBucketById(oid);
    }

    Entity::S3::BucketList S3MemoryDb::ListBuckets() {
        boost::mutex::scoped_lock lock(_bucketMutex);
        return Core::NumberUtils::toVector(_buckets);
    }

    Entity::S3::BucketList S3MemoryDb::ExportBuckets(const std::vector<SortColumn> &sortColumns) const {

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

    bool S3MemoryDb::HasObjects(const Entity::S3::Bucket &bucket) const {
        return std::ranges::count_if(_objects, [bucket](const auto &object) {
            return object.second.region == bucket.region && object.second.bucket == bucket.name;
        }) > 0;
    }

    std::vector<Entity::S3::Object> S3MemoryDb::GetBucketObjectList(const std::string &region, const std::string &bucket, const long maxKeys) {

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

    long S3MemoryDb::GetBucketObjectCount(const std::string &region, const std::string &bucket) const {

        return std::ranges::count_if(_objects, [region, bucket](const auto &object) {
            return object.second.region == region && object.second.bucket == bucket;
        });
    }

    long S3MemoryDb::GetBucketSize(const std::string &region, const std::string &bucket) const {
        return std::accumulate(_objects.begin(), _objects.end(), 0L,
                               [region, bucket](long sum, const auto &item) {
                                   return item.second.region == region && item.second.bucket == bucket ? sum + item.second.size : sum;
                               });
    }

    long S3MemoryDb::BucketCount() const {

        return static_cast<long>(_buckets.size());
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListBucket(const std::string &bucket, const std::string &prefix) const {

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

    long S3MemoryDb::PurgeBucket(const Entity::S3::Bucket &bucket) {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto count = std::erase_if(_objects, [bucket](const auto &item) {
            return item.second.region == bucket.region && item.second.bucket == bucket.name;
        });
        return static_cast<long>(count);
    }

    Entity::S3::Bucket S3MemoryDb::UpdateBucket(const Entity::S3::Bucket &bucket) {
        boost::mutex::scoped_lock lock(_bucketMutex);
        const auto it = std::ranges::find_if(_buckets,
                                             [bucket](const std::pair<std::string, Entity::S3::Bucket> &b) {
                                                 return b.second.region == bucket.region && b.second.name == bucket.name;
                                             });
        _buckets[it->first] = bucket;
        return _buckets[it->first];
    }

    void S3MemoryDb::UpdateBucketCounter(const std::string &bucketArn, const long keys, const long size) {

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

    void S3MemoryDb::DeleteBucket(const Entity::S3::Bucket &bucket) {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto count = std::erase_if(_buckets, [bucket](const auto &item) {
            return item.second.region == bucket.region && item.second.name == bucket.name;
        });
        log_debug << "Bucket deleted, count: " << count;
    }

    long S3MemoryDb::DeleteAllBuckets() {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const long count = static_cast<long>(_buckets.size());
        log_debug << "All buckets deleted, count: " << _buckets.size();
        _buckets.clear();
        return count;
    }

    bool S3MemoryDb::ObjectExists(const Entity::S3::Object &object) {

        return std::ranges::find_if(_objects,
                                    [object](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.region == object.region && o.second.bucket == object.bucket && o.second.key == object.key;
                                    }) != _objects.end();
    }

    bool S3MemoryDb::ObjectExists(const std::string &oid) const {

        return std::ranges::find_if(_objects,
                                    [oid](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.oid == oid;
                                    }) != _objects.end();
    }

    bool S3MemoryDb::ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const {

        return std::ranges::find_if(_objects,
                                    [region, bucket, key](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.region == region && o.second.bucket == bucket && o.second.key == key;
                                    }) != _objects.end();
    }

    bool S3MemoryDb::ObjectExistsInternalName(const std::string &filename) {

        return std::ranges::find_if(_objects,
                                    [filename](const std::pair<std::string, Entity::S3::Object> &object) {
                                        return object.second.internalName == filename;
                                    }) != _objects.end();
    }

    Entity::S3::Object S3MemoryDb::CreateObject(const Entity::S3::Object &object) {
        boost::mutex::scoped_lock lock(_objectMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _objects[oid] = object;
        log_trace << "Object created, oid: " << oid;
        return GetObjectById(oid);
    }

    Entity::S3::Object S3MemoryDb::UpdateObject(const Entity::S3::Object &object) {
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

    Entity::S3::Object S3MemoryDb::GetObjectById(const std::string &oid) {

        const auto it = std::ranges::find_if(_objects, [oid](const std::pair<std::string, Entity::S3::Object> &object) {
            return object.first == oid;
        });

        if (it != _objects.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::S3::Object S3MemoryDb::GetObject(const std::string &region, const std::string &bucket, const std::string &key) {

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

    Entity::S3::Object S3MemoryDb::GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) {

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

    long S3MemoryDb::ObjectCount(const std::string &region, const std::string &bucket) {
        boost::mutex::scoped_lock lock(_objectMutex);

        auto q = Core::from(_objects | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::S3::Object &object) { return object.region == region; });
        }
        if (!bucket.empty()) {
            q = q.where([bucket](const Entity::S3::Object &object) { return object.bucket == bucket; });
        }
        log_debug << "Object count, region: " << region << ", bucket: " << bucket << ", count: " << q.count();
        return static_cast<long>(q.count());
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListObjects(const std::string &region, const std::string &prefix, const std::string &bucket, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) {

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
                for (const auto &[column, sortDirection]: sortColumns) {
                    if (column == "bucket") {
                        return sortDirection == 1 ? a.bucket < b.bucket : b.bucket < a.bucket;
                    }
                    if (column == "size") {
                        return sortDirection == 1 ? a.size < b.size : b.size < a.size;
                    }
                    if (column == "created") {
                        return sortDirection == 1 ? a.created < b.created : b.created < a.created;
                    }
                    if (column == "modified") {
                        return sortDirection == 1 ? a.modified < b.modified : b.modified < a.modified;
                    }
                }
                return false;
            });
        }
        log_trace << "Objects list, prefix: " << prefix << ", count: " << q.count();
        return Core::PageVector(q.to_vector(), pageSize, pageIndex);
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {

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

    long S3MemoryDb::DeleteObject(const Entity::S3::Object &object) {
        boost::mutex::scoped_lock lock(_objectMutex);

        const auto count = std::erase_if(_objects, [object](const auto &item) {
            return item.second.bucket == object.bucket && item.second.key == object.key;
        });
        log_debug << "Object deleted, count: " << count;
        return count;
    }

    void S3MemoryDb::DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys) {
        const std::unordered_set keySet(keys.begin(), keys.end());
        long count = 0;
        {
            // Minimal Lock Zone
            boost::mutex::scoped_lock lock(_objectMutex);

            count = static_cast<long>(std::erase_if(_objects, [&](const auto &item) {
                return item.second.bucket == bucket && keySet.contains(item.second.key);
            }));
        }
        log_debug << "Objects deleted, count: " << count;
    }

    long S3MemoryDb::DeleteAllObjects() {
        boost::mutex::scoped_lock lock(_objectMutex);

        const long count = static_cast<long>(_objects.size());
        log_debug << "Deleting objects, size: " << _objects.size();
        _objects.clear();
        return count;
    }

    void S3MemoryDb::AdjustObjectCounters() {
        for (const auto &bucket: _buckets | std::views::values) {
            _buckets[bucket.oid].keys = ObjectCount(bucket.region, bucket.name);
        }
        log_debug << "All object counters updated, count: " << _buckets.size();
    }
} // namespace AwsMock::Database
