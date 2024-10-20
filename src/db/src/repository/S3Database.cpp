//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/S3Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    std::map<std::string, std::vector<std::string>> S3Database::allowedEventTypes = {
            {"Created", {"s3:ObjectCreated:Put", "s3:ObjectCreated:Post", "s3:ObjectCreated:Copy", "s3:ObjectCreated:CompleteMultipartUpload"}},
            {"Deleted", {"s3:ObjectRemoved:Delete", "s3:ObjectRemoved:DeleteMarkerCreated"}}};

    S3Database::S3Database() : _memoryDb(S3MemoryDb::instance()), _databaseName(GetDatabaseName()), _bucketCollectionName("s3_bucket"), _objectCollectionName("s3_object") {}

    bool S3Database::BucketExists(const std::string &region, const std::string &name) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

                int64_t count = _bucketCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "Bucket exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.BucketExists(region, name);
        }
    }

    bool S3Database::BucketExists(const Entity::S3::Bucket &bucket) {
        return BucketExists(bucket.region, bucket.name);
    }

    Entity::S3::Bucket S3Database::CreateBucket(Entity::S3::Bucket &bucket) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto insert_one_result = _bucketCollection.insert_one(bucket.ToDocument());
                log_trace << "Bucket created, oid: " << insert_one_result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();

                bucket.oid = insert_one_result->inserted_id().get_oid().value.to_string();
                return bucket;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.CreateBucket(bucket);
        }
    }

    long S3Database::BucketCount(const std::string &region, const std::string &prefix) {

        if (HasDatabase()) {

            try {
                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

                bsoncxx::builder::basic::document query;

                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }

                long count = _bucketCollection.count_documents(query.extract());
                log_trace << "Bucket count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Bucket count failed, error: " << e.what();
            }

        } else {

            return _memoryDb.BucketCount();
        }
        return -1;
    }

    Entity::S3::Bucket S3Database::GetBucketById(bsoncxx::oid oid) {

        auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        mongocxx::stdx::optional<bsoncxx::document::value>
                mResult = _bucketCollection.find_one(make_document(kvp("_id", oid)));
        Entity::S3::Bucket result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::S3::Bucket S3Database::GetBucketById(const std::string &oid) {

        if (HasDatabase()) {

            return GetBucketById(bsoncxx::oid(oid));

        } else {

            return _memoryDb.GetBucketById(oid);
        }
    }

    Entity::S3::Bucket S3Database::GetBucketByRegionName(const std::string &region, const std::string &name) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            mongocxx::stdx::optional<bsoncxx::document::value> mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
            if (mResult) {
                Entity::S3::Bucket result;
                result.FromDocument(mResult->view());
                log_trace << "Got bucket: " << result.ToString();
                return result;
            }
            return {};

        } else {

            return _memoryDb.GetBucketByRegionName(region, name);
        }
    }

    Entity::S3::BucketList S3Database::ListBuckets(const std::string &region, const std::string &prefix, long maxResults, long skip, const std::vector<Core::SortColumn> &sortColumns) {

        Entity::S3::BucketList bucketList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                bsoncxx::builder::basic::document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (skip > 0) {
                opts.skip(skip);
            }
            if (maxResults > 0) {
                opts.limit(maxResults);
            }

            bsoncxx::builder::basic::document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }

            auto bucketCursor = _bucketCollection.find(query.extract(), opts);
            for (const auto &bucket: bucketCursor) {
                Entity::S3::Bucket result;
                result.FromDocument(bucket);
                bucketList.push_back(result);
            }

        } else {

            bucketList = _memoryDb.ListBuckets();
        }
        log_trace << "Got bucket list, size:" << bucketList.size();
        return bucketList;
    }

    bool S3Database::HasObjects(const Entity::S3::Bucket &bucket) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            int64_t count =
                    _objectCollection.count_documents(make_document(kvp("region", bucket.region), kvp("bucket", bucket.name)));
            log_trace << "Objects exists: " << std::boolalpha << count;
            return count > 0;

        } else {

            return _memoryDb.HasObjects(bucket);
        }
    }

    std::vector<Entity::S3::Object> S3Database::GetBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys) {

        std::vector<Entity::S3::Object> objectList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            mongocxx::options::find opts;
            if (maxKeys > 0) {
                opts.limit(maxKeys);
            }
            auto objectCursor = _objectCollection.find(make_document(kvp("region", region), kvp("bucket", bucket)), opts);
            for (auto object: objectCursor) {
                Entity::S3::Object result;
                result.FromDocument(object);
                objectList.push_back(result);
            }

            log_trace << "Objects exists, count: " << objectList.size();
            return objectList;

        } else {

            return _memoryDb.GetBucketObjectList(region, bucket, maxKeys);
        }
    }

    Entity::S3::Bucket S3Database::UpdateBucket(Entity::S3::Bucket &bucket) {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                bucket.modified = system_clock::now();
                auto mResult = _bucketCollection.find_one_and_update(make_document(kvp("region", bucket.region), kvp("name", bucket.name)), bucket.ToDocument(), opts);
                log_trace << "Bucket updated: " << bucket.ToString();
                session.commit_transaction();

                if (mResult) {
                    bucket.FromDocument(mResult->view());
                    return bucket;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.UpdateBucket(bucket);
        }
    }

    Entity::S3::Bucket S3Database::CreateOrUpdateBucket(Entity::S3::Bucket &bucket) {

        if (BucketExists(bucket)) {
            return UpdateBucket(bucket);
        } else {
            return CreateBucket(bucket);
        }
    }

    // TODO: Combine with Listobject
    Entity::S3::ObjectList S3Database::ListBucket(const std::string &bucket, const std::string &prefix) {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            if (prefix.empty()) {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                auto objectCursor = _objectCollection.find(make_document(kvp("bucket", bucket)));
                for (auto object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }

            } else {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                auto objectCursor = _objectCollection.find(make_document(kvp("bucket", bucket),
                                                                         kvp("key",
                                                                             bsoncxx::types::b_regex{"^" + prefix + ".*"})));
                for (auto object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }
            }

        } else {

            objectList = _memoryDb.ListBucket(bucket, prefix);
        }

        log_trace << "Got object list, size:" << objectList.size();

        return objectList;
    }

    /*Entity::S3::ObjectList S3Database::ListObjects(const std::string &prefix) {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            if (prefix.empty()) {

                auto objectCursor = _objectCollection.find({});
                for (const auto &object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }
            } else {

                auto objectCursor = _objectCollection.find(make_document(kvp("key", bsoncxx::types::b_regex{"^" + prefix + ".*"})));
                for (const auto &object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }
            }

        } else {

            objectList = _memoryDb.ListObjects(prefix);
        }

        log_trace << "Got object list in all buckets, size:" << objectList.size();
        return objectList;
    }*/

    long S3Database::BucketSize(const std::string &region, const std::string &bucket) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            try {
                mongocxx::pipeline p{};
                p.match(make_document(kvp("region", region), kvp("bucket", bucket)));
                p.group(make_document(kvp("_id", ""),
                                      kvp("totalSize",
                                          make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
                auto totalSizeCursor = _objectCollection.aggregate(p);
                auto t = *totalSizeCursor.begin();
                if (!t.empty()) {
                    return t["totalSize"].get_int64().value;
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }
        }
        return 0;
    }

    void S3Database::DeleteBucket(const Entity::S3::Bucket &bucket) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("name", bucket.name)));
                session.commit_transaction();
                log_debug << "Bucket deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteBucket(bucket);
        }
    }

    void S3Database::DeleteAllBuckets() {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto delete_many_result = _bucketCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All buckets deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteAllBuckets();
        }
    }

    bool S3Database::ObjectExists(const Entity::S3::Object &object) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            int64_t count = _objectCollection.count_documents(make_document(kvp("region", object.region),
                                                                            kvp("bucket", object.bucket),
                                                                            kvp("key", object.key)));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;

        } else {
            return _memoryDb.ObjectExists(object);
        }
    }

    bool S3Database::ObjectExists(const std::string &filename) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            int64_t count = _objectCollection.count_documents(make_document(kvp("internalName", filename)));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;

        } else {

            return _memoryDb.ObjectExists(filename);
        }
    }

    Entity::S3::Object S3Database::CreateObject(Entity::S3::Object &object) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto insert_one_result = _objectCollection.insert_one(object.ToDocument().view());
                object.oid = insert_one_result->inserted_id().get_oid().value.to_string();

                // Update bucket counters
                Entity::S3::Bucket bucket = GetBucketByRegionName(object.region, object.bucket);
                bucket.size += object.size;
                bucket.keys++;
                UpdateBucket(bucket);
                session.commit_transaction();

                return object;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.CreateObject(object);
        }
    }

    Entity::S3::Object S3Database::GetObjectById(bsoncxx::oid oid) {

        try {
            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            mongocxx::stdx::optional<bsoncxx::document::value>
                    mResult = _objectCollection.find_one(make_document(kvp("_id", oid)));

            if (mResult->empty()) {
                return {};
            }
            Entity::S3::Object result;
            result.FromDocument(mResult->view());
            return result;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Get object by ID failed, error: " << e.what();
        }
        return {};
    }

    Entity::S3::Object S3Database::GetObjectById(const std::string &oid) {

        if (HasDatabase()) {

            return GetObjectById(bsoncxx::oid(oid));

        } else {

            return _memoryDb.GetObjectById(oid);
        }
    }

    Entity::S3::Object S3Database::CreateOrUpdateObject(Entity::S3::Object &object) {

        if (ObjectExists(object)) {
            return UpdateObject(object);
        } else {
            return CreateObject(object);
        }
    }

    Entity::S3::Object S3Database::UpdateObject(Entity::S3::Object &object) {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                object.modified = system_clock::now();
                auto mResult = _objectCollection.find_one_and_update(make_document(kvp("region", object.region), kvp("bucket", object.bucket), kvp("key", object.key)), object.ToDocument(), opts);
                log_trace << "Object updated: " << object.ToString();
                session.commit_transaction();

                if (mResult) {
                    object.FromDocument(mResult->view());
                    return object;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.UpdateObject(object);
        }
    }

    Entity::S3::Object S3Database::GetObject(const std::string &region, const std::string &bucket, const std::string &key) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                mongocxx::stdx::optional<bsoncxx::document::value> mResult =
                        _objectCollection.find_one(make_document(kvp("region", region), kvp("bucket", bucket), kvp("key", key)));

                if (mResult.operator bool()) {
                    Entity::S3::Object result;
                    result.FromDocument(mResult->view());

                    log_trace << "Got object: " << result.ToString();
                    return result;
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.GetObject(region, bucket, key);
        }
        return {};
    }

    Entity::S3::Object S3Database::GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                mongocxx::stdx::optional<bsoncxx::document::value>
                        mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                           kvp("bucket", bucket),
                                                                           kvp("key", key),
                                                                           kvp("md5sum", md5sum)));
                if (mResult->begin() != mResult->end()) {
                    Entity::S3::Object result;
                    result.FromDocument(mResult->view());

                    log_trace << "Got object MD5: " << result.ToString();
                    return result;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }
        } else {

            return _memoryDb.GetObjectMd5(region, bucket, key, md5sum);
        }
    }

    Entity::S3::Object S3Database::GetObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &versionId) {

        try {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                    kvp("bucket", bucket),
                                                                    kvp("key", key),
                                                                    kvp("versionId", versionId)));
            if (mResult) {
                Entity::S3::Object result;
                result.FromDocument(mResult->view());

                log_trace << "Got object version: " << result.ToString();
                return result;
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what(), 500);
        }
        return {};
    }

    Entity::S3::ObjectList S3Database::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            if (prefix.empty()) {

                auto objectCursor = _objectCollection.find(make_document(kvp("region", region), kvp("bucket", bucket)));
                for (const auto &object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }
            } else {

                auto objectCursor = _objectCollection.find(make_document(kvp("region", region), kvp("bucket", bucket),
                                                                         kvp("key", make_document(kvp("$regex", "^" + prefix)))));
                for (const auto &object: objectCursor) {
                    Entity::S3::Object result;
                    result.FromDocument(object);
                    objectList.push_back(result);
                }
            }

        } else {

            objectList = _memoryDb.ListObjectVersions(region, bucket, prefix);
        }

        log_trace << "Got versioned list, size:" << objectList.size();
        return objectList;
    }

    long S3Database::ObjectCount(const std::string &region, const std::string &bucket) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

                long count = 0;
                if (region.empty() && bucket.empty()) {

                    count = static_cast<long>(_objectCollection.count_documents({}));

                } else if (!region.empty() && bucket.empty()) {

                    count = static_cast<long>(_objectCollection.count_documents(make_document(kvp("region", region))));

                } else if (region.empty() && !bucket.empty()) {

                    count = static_cast<long>(_objectCollection.count_documents(make_document(kvp("bucket", bucket))));

                } else {

                    count = static_cast<long>(_objectCollection.count_documents(make_document(kvp("region", region), kvp("bucket", bucket))));
                }

                log_trace << "Object count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Object count failed, error: " << e.what();
            }

        } else {

            return _memoryDb.ObjectCount(region, bucket);
        }
        return -1;
    }

    Entity::S3::ObjectList S3Database::ListObjects(const std::string &bucket, const std::string &prefix, long maxResults, long skip, const std::vector<Core::SortColumn> &sortColumns) {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                bsoncxx::builder::basic::document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (skip > 0) {
                opts.skip(skip);
            }
            if (maxResults > 0) {
                opts.limit(maxResults);
            }

            bsoncxx::builder::basic::document query = {};
            if (!bucket.empty()) {
                query.append(kvp("bucket", bucket));
            }
            if (!prefix.empty()) {
                query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
            }

            auto objectCursor = _objectCollection.find(query.extract(), opts);
            for (const auto &object: objectCursor) {
                Entity::S3::Object result;
                result.FromDocument(object);
                objectList.push_back(result);
            }

        } else {

            objectList = _memoryDb.ListObjects();
        }
        log_trace << "Got object list, size:" << objectList.size();
        return objectList;
    }

    void S3Database::DeleteObject(const Entity::S3::Object &object) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                auto result = _objectCollection.delete_many(make_document(kvp("region", object.region),
                                                                          kvp("bucket", object.bucket),
                                                                          kvp("key", object.key)));
                log_debug << "Objects deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteObject(object);
        }
    }

    void S3Database::DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys) {

        if (HasDatabase()) {

            bsoncxx::builder::basic::array array{};
            for (const auto &key: keys) {
                array.append(key);
            }

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                if (keys.empty()) {
                    auto result = _objectCollection.delete_many(make_document(kvp("bucket", bucket)));
                    log_debug << "Objects deleted, count: " << result->result().deleted_count();
                } else {
                    auto result = _objectCollection.delete_many(make_document(kvp("bucket", bucket),
                                                                              kvp("key", make_document(kvp("$in", array)))));
                    log_debug << "Objects deleted, count: " << result->result().deleted_count();
                }

                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteObjects(bucket, keys);
        }
    }

    void S3Database::DeleteAllObjects() {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _objectCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All objects deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteAllObjects();
        }
    }

    Entity::S3::Bucket S3Database::CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) {

        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);
        internBucket.notifications.clear();
        if (Core::StringUtils::Contains(bucketNotification.event, "*")) {

            std::vector<std::string> allowedEvents;
            if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectCreated:")) {
                allowedEvents = allowedEventTypes["Created"];
            } else if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectRemoved:")) {
                allowedEvents = allowedEventTypes["Deleted"];
            }

            for (const auto &it: allowedEvents) {

                Entity::S3::BucketNotification notification = {
                        .event = it,
                        .notificationId = bucketNotification.notificationId,
                        .queueArn = bucketNotification.queueArn,
                        .lambdaArn = bucketNotification.lambdaArn};
                internBucket.notifications.emplace_back(notification);
            }

        } else {

            internBucket.notifications.emplace_back(bucketNotification);
        }

        log_debug << "Bucket notification added, notification: " << bucketNotification.ToString();

        return UpdateBucket(internBucket);
    }

    Entity::S3::Bucket S3Database::DeleteBucketNotifications(const Entity::S3::Bucket &bucket,
                                                             const Entity::S3::BucketNotification &bucketNotification) {

        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);

        if (Core::StringUtils::Contains(bucketNotification.event, "*")) {

            std::vector<std::string> allowedEvents;
            if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectCreated:")) {
                allowedEvents = allowedEventTypes["Created"];
            } else if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectRemoved:")) {
                allowedEvents = allowedEventTypes["Deleted"];
            }

            for (const auto &it: allowedEvents) {
                internBucket.notifications.erase(std::remove_if(internBucket.notifications.begin(),
                                                                internBucket.notifications.end(),
                                                                [it](const Entity::S3::BucketNotification &notification) {
                                                                    return it == notification.event;
                                                                }),
                                                 internBucket.notifications.end());
            }

        } else {

            internBucket.notifications.erase(std::remove_if(internBucket.notifications.begin(),
                                                            internBucket.notifications.end(),
                                                            [bucketNotification](const Entity::S3::BucketNotification &notification) {
                                                                return bucketNotification.event == notification.event;
                                                            }),
                                             internBucket.notifications.end());
        }

        log_trace << "Bucket notification deleted, notification: " << bucketNotification.ToString();

        return UpdateBucket(internBucket);
    }

}// namespace AwsMock::Database
