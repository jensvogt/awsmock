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

        Entity::S3::Bucket result;

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

        Entity::S3::Bucket result;

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

        Entity::S3::BucketList bucketList;
        for (const auto &bucket: std::views::values(_buckets)) {
            bucketList.emplace_back(bucket);
        }

        log_trace << "Got bucket list, size: " << bucketList.size();
        return bucketList;
    }

    Entity::S3::BucketList S3MemoryDb::ExportBuckets(const std::vector<SortColumn> &sortColumns) const {

        Entity::S3::BucketList bucketList;
        for (const auto &bucket: std::views::values(_buckets)) {
            bucketList.emplace_back(bucket);
        }

        log_trace << "Got bucket list, size: " << bucketList.size();
        std::ranges::sort(bucketList, [](const Entity::S3::Bucket &a, const Entity::S3::Bucket &b) {
            return a.name < b.name;
        });
        return bucketList;
    }

    bool S3MemoryDb::HasObjects(const Entity::S3::Bucket &bucket) const {

        long count = 0;
        for (const auto &val: _objects | std::views::values) {
            if (val.region == bucket.region && val.bucket == bucket.name) {
                count++;
            }
        }
        return count > 0;
    }

    std::vector<Entity::S3::Object> S3MemoryDb::GetBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys) const {

        std::vector<Entity::S3::Object> objectList;
        for (const auto &val: _objects | std::views::values) {
            if (val.region == region && val.bucket == bucket && objectList.size() < maxKeys) {
                objectList.emplace_back(val);
            } else {
                break;
            }
        }
        return objectList;
    }

    long S3MemoryDb::GetBucketObjectCount(const std::string &region, const std::string &bucket) const {

        long count = 0;
        for (const auto &val: _objects | std::views::values) {
            if (val.region == region && val.bucket == bucket) {
                count++;
            } else {
                break;
            }
        }
        return count;
    }

    long S3MemoryDb::GetBucketSize(const std::string &region, const std::string &bucket) const {

        long size = 0;
        for (const auto &val: _objects | std::views::values) {
            if (val.region == region && val.bucket == bucket) {
                size += val.size;
            } else {
                break;
            }
        }
        return size;
    }

    long S3MemoryDb::BucketCount() const {

        return static_cast<long>(_buckets.size());
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListBucket(const std::string &bucket, const std::string &prefix) const {

        std::vector<Entity::S3::Object> objectList;

        if (prefix.empty()) {

            for (const auto &val: _objects | std::views::values) {
                if (val.bucket == bucket) {
                    objectList.emplace_back(val);
                }
            }

        } else {

            for (const auto &val: _objects | std::views::values) {
                if (val.bucket == bucket && Core::StringUtils::StartsWith(val.key, prefix)) {
                    objectList.emplace_back(val);
                }
            }
        }

        log_trace << "Got object list, size: " << objectList.size();
        return objectList;
    }

    long S3MemoryDb::PurgeBucket(const Entity::S3::Bucket &bucket) {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const auto count = std::erase_if(_objects, [bucket](const auto &item) {
            auto const &[key, value] = item;
            return value.region == bucket.region && value.bucket == bucket.name;
        });
        return static_cast<long>(count);
    }

    Entity::S3::Bucket S3MemoryDb::UpdateBucket(const Entity::S3::Bucket &bucket) {

        boost::mutex::scoped_lock lock(_bucketMutex);

        std::string region = bucket.region;
        std::string name = bucket.name;
        const auto it = std::ranges::find_if(_buckets,
                                             [region, name](const std::pair<std::string, Entity::S3::Bucket> &b) {
                                                 return b.second.region == region && b.second.name == name;
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

        std::string region = bucket.region;
        std::string name = bucket.name;
        const auto count = std::erase_if(_buckets, [region, name](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.name == name;
        });
        log_debug << "Bucket deleted, count: " << count;
    }

    long S3MemoryDb::DeleteAllBuckets() {
        boost::mutex::scoped_lock lock(_bucketMutex);

        const long count = _buckets.size();
        log_debug << "All buckets deleted, count: " << _buckets.size();
        _buckets.clear();
        return count;
    }

    bool S3MemoryDb::ObjectExists(const Entity::S3::Object &object) {

        std::string region = object.region;
        std::string bucket = object.bucket;
        std::string key = object.key;
        return std::ranges::find_if(_objects,
                                    [region, bucket, key](const std::pair<std::string, Entity::S3::Object> &o) {
                                        return o.second.region == region && o.second.bucket == bucket && o.second.key == key;
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

        std::string bucket = object.bucket;
        std::string key = object.key;
        const auto it = std::ranges::find_if(_objects,
                                             [bucket, key](const std::pair<std::string, Entity::S3::Object> &o) {
                                                 return o.second.bucket == bucket && o.second.key == key;
                                             });
        _objects[it->first] = object;
        return _objects[it->first];
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

    long S3MemoryDb::ObjectCount(const std::string &region, const std::string &bucket) const {
        boost::mutex::scoped_lock lock(_objectMutex);

        if (region.empty() && bucket.empty()) {
            return static_cast<long>(_objects.size());
        }

        long count = 0;
        if (!region.empty() && bucket.empty()) {
            for (const auto &val: _objects | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
        } else if (region.empty() && !bucket.empty()) {
            for (const auto &val: _objects | std::views::values) {
                if (val.bucket == bucket) {
                    count++;
                }
            }
        } else if (!region.empty() && !bucket.empty()) {
            for (const auto &val: _objects | std::views::values) {
                if (val.bucket == bucket && val.region == region) {
                    count++;
                }
            }
        }
        log_debug << "Count: " << count << " bucket: " << bucket << " region: " << region;
        return count;
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListObjects(const std::string &prefix) const {

        std::vector<Entity::S3::Object> objectList;
        if (prefix.empty()) {
            for (const auto &val: _objects | std::views::values) {
                objectList.emplace_back(val);
            }
        } else {
            for (const auto &val: _objects | std::views::values) {
                if (!prefix.empty() && Core::StringUtils::StartsWith(val.key, prefix)) {
                    objectList.emplace_back(val);
                }
            }
        }

        log_trace << "Got object list, size: " << objectList.size();
        return objectList;
    }

    std::vector<Entity::S3::Object> S3MemoryDb::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {

        std::vector<Entity::S3::Object> objectList;
        if (prefix.empty()) {
            for (const auto &val: _objects | std::views::values) {
                if (val.region == region && val.bucket == bucket) {
                    objectList.emplace_back(val);
                }
            }
        } else {
            for (const auto &val: _objects | std::views::values) {
                if (val.region == region && val.bucket == bucket) {
                    if (!prefix.empty() && Core::StringUtils::StartsWith(val.key, prefix)) {
                        objectList.emplace_back(val);
                    }
                }
            }
        }

        log_trace << "Got versioned object list, size: " << objectList.size();
        return objectList;
    }

    void S3MemoryDb::DeleteObject(const Entity::S3::Object &object) {
        boost::mutex::scoped_lock lock(_objectMutex);

        std::string bucket = object.bucket;
        std::string key = object.key;
        const auto count = std::erase_if(_objects, [bucket, key](const auto &item) {
            auto const &[k, v] = item;
            return v.bucket == bucket && v.key == key;
        });
        log_debug << "Object deleted, count: " << count;
    }

    void S3MemoryDb::DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys) {
        boost::mutex::scoped_lock lock(_objectMutex);

        auto count = 0;
        for (const auto &key: keys) {
            count += std::erase_if(_objects, [bucket, key](const auto &item) {
                auto const &[k, v] = item;
                return v.bucket == bucket && v.key == key;
            });
        }
        log_debug << "Objects deleted, count: " << count;
    }

    long S3MemoryDb::DeleteAllObjects() {
        boost::mutex::scoped_lock lock(_objectMutex);

        long count = _objects.size();
        log_debug << "Deleting objects, size: " << _objects.size();
        _objects.clear();
        return count;
    }
}// namespace AwsMock::Database