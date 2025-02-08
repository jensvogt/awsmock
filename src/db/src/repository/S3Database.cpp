//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/S3Database.h>

namespace AwsMock::Database {

    std::map<std::string, std::vector<std::string>> S3Database::allowedEventTypes = {
            {"Created", {"s3:ObjectCreated:Put", "s3:ObjectCreated:Post", "s3:ObjectCreated:Copy", "s3:ObjectCreated:CompleteMultipartUpload"}},
            {"Deleted", {"s3:ObjectRemoved:Delete", "s3:ObjectRemoved:DeleteMarkerCreated"}}};

    S3Database::S3Database() : _databaseName(GetDatabaseName()), _bucketCollectionName("s3_bucket"), _objectCollectionName("s3_object"), _memoryDb(S3MemoryDb::instance()) {}

    bool S3Database::BucketExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

                const int64_t count = _bucketCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "Bucket exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.BucketExists(region, name);
    }

    bool S3Database::BucketExists(const Entity::S3::Bucket &bucket) const {
        return BucketExists(bucket.region, bucket.name);
    }

    Entity::S3::Bucket S3Database::CreateBucket(Entity::S3::Bucket &bucket) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto insert_one_result = _bucketCollection.insert_one(bucket.ToDocument());
                log_trace << "Bucket created, oid: " << insert_one_result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();

                bucket.oid = insert_one_result->inserted_id().get_oid().value.to_string();
                return bucket;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CreateBucket(bucket);
    }

    long S3Database::BucketCount(const std::string &region, const std::string &prefix) const {

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

                document query = {};

                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }

                const long count = _bucketCollection.count_documents(query.extract());
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

    Entity::S3::Bucket S3Database::GetBucketById(bsoncxx::oid oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        const std::optional<value> mResult = _bucketCollection.find_one(make_document(kvp("_id", oid)));
        Entity::S3::Bucket result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::S3::Bucket S3Database::GetBucketById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetBucketById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetBucketById(oid);
    }

    Entity::S3::Bucket S3Database::GetBucketByRegionName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            if (const std::optional<value> mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name)))) {
                Entity::S3::Bucket result;
                result.FromDocument(mResult->view());
                log_trace << "Got bucket: " << result.ToString();
                return result;
            }
            return {};
        }
        return _memoryDb.GetBucketByRegionName(region, name);
    }

    Entity::S3::BucketList S3Database::ListBuckets(const std::string &region, const std::string &prefix, const long maxResults, const long skip, const std::vector<Core::SortColumn> &sortColumns) const {

        Entity::S3::BucketList bucketList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (skip > 0) {
                opts.skip(skip);
            }
            if (maxResults > 0) {
                opts.limit(maxResults);
            }

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }

            for (auto bucketCursor = _bucketCollection.find(query.extract(), opts); const auto &bucket: bucketCursor) {
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

    Entity::S3::BucketList S3Database::ExportBuckets(const std::vector<Core::SortColumn> &sortColumns) const {

        Entity::S3::BucketList bucketList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            }

            for (auto bucketCursor = _bucketCollection.find({}, opts); const auto &bucket: bucketCursor) {
                Entity::S3::Bucket result;
                result.FromDocument(bucket);
                result.keys = 0;
                result.size = 0;
                bucketList.push_back(result);
            }

        } else {

            bucketList = _memoryDb.ExportBuckets();
        }
        log_trace << "Export bucket list, size:" << bucketList.size();
        return bucketList;
    }

    bool S3Database::HasObjects(const Entity::S3::Bucket &bucket) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            const int64_t count = _objectCollection.count_documents(make_document(kvp("region", bucket.region), kvp("bucket", bucket.name)));
            log_trace << "Objects exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.HasObjects(bucket);
    }

    std::vector<Entity::S3::Object> S3Database::GetBucketObjectList(const std::string &region, const std::string &bucket, const long maxKeys) const {

        if (HasDatabase()) {
            std::vector<Entity::S3::Object> objectList;

            const auto client = ConnectionPool::instance().GetConnection();
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
        }
        return _memoryDb.GetBucketObjectList(region, bucket, maxKeys);
    }

    long S3Database::GetBucketObjectCount(const std::string &region, const std::string &bucket) const {

        std::vector<Entity::S3::Object> objectList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const long objectCount = _objectCollection.count_documents(make_document(kvp("region", region), kvp("bucket", bucket)));

            log_trace << "Objects count, count: " << objectCount;
            return objectCount;
        }
        return _memoryDb.GetBucketObjectCount(region, bucket);
    }

    long S3Database::PurgeBucket(Entity::S3::Bucket &bucket) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _objectCollection.delete_many(make_document(kvp("region", bucket.region), kvp("bucket", bucket.name)));
                log_trace << "Bucket purged: " << bucket.ToString();
                session.commit_transaction();

                if (mResult) {
                    log_debug << "Bucket purged, name: " << bucket.name << " deleted: " << mResult->deleted_count();
                    return mResult.value().deleted_count();
                }
                return 0;
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.PurgeBucket(bucket);
    }

    Entity::S3::Bucket S3Database::UpdateBucket(Entity::S3::Bucket &bucket) const {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                bucket.modified = system_clock::now();
                const auto mResult = _bucketCollection.find_one_and_update(make_document(kvp("region", bucket.region), kvp("name", bucket.name)), bucket.ToDocument(), opts);
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
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateBucket(bucket);
    }

    Entity::S3::Bucket S3Database::CreateOrUpdateBucket(Entity::S3::Bucket &bucket) const {

        if (BucketExists(bucket)) {
            return UpdateBucket(bucket);
        }
        return CreateBucket(bucket);
    }

    // TODO: Combine with Listobject
    Entity::S3::ObjectList S3Database::ListBucket(const std::string &bucket, const std::string &prefix) const {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            document query;
            if (!bucket.empty()) {
                query.append(kvp("bucket", bucket));
            }
            if (!prefix.empty()) {
                query.append(kvp("key", bsoncxx::types::b_regex{"^" + prefix + ".*"}));
            }

            for (auto objectCursor = _objectCollection.find(query.extract()); auto object: objectCursor) {
                Entity::S3::Object result;
                result.FromDocument(object);
                objectList.push_back(result);
            }

        } else {

            objectList = _memoryDb.ListBucket(bucket, prefix);
        }
        log_trace << "Got object list, size:" << objectList.size();
        return objectList;
    }

    long S3Database::GetBucketSize(const std::string &region, const std::string &bucket) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            try {
                mongocxx::pipeline p{};
                p.match(make_document(kvp("region", region), kvp("bucket", bucket)));
                p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
                auto totalSizeCursor = _objectCollection.aggregate(p);
                if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                    return t["totalSize"].get_int64().value;
                }
                return 0;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetBucketSize(region, bucket);
    }

    void S3Database::AdjustBucketCounters(const std::string &region, const std::string &bucketName) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            Entity::S3::Bucket bucket = GetBucketByRegionName(region, bucketName);
            bucket.size = GetBucketSize(region, bucketName);
            bucket.keys = GetBucketObjectCount(region, bucketName);

            session.start_transaction();
            const auto mResult = _bucketCollection.find_one_and_update(make_document(kvp("region", region), kvp("name", bucketName)), bucket.ToDocument());
            session.commit_transaction();
        }
    }

    void S3Database::DeleteBucket(const Entity::S3::Bucket &bucket) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("name", bucket.name)));
                session.commit_transaction();
                log_debug << "Bucket deleted, count: " << delete_many_result->deleted_count();
                return;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.DeleteBucket(bucket);
    }

    long S3Database::DeleteAllBuckets() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _bucketCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All buckets deleted, count: " << delete_many_result->deleted_count();
                return delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteAllBuckets();
    }

    bool S3Database::ObjectExists(const Entity::S3::Object &object) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const int64_t count = _objectCollection.count_documents(make_document(kvp("region", object.region),
                                                                                  kvp("bucket", object.bucket),
                                                                                  kvp("key", object.key)));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.ObjectExists(object);
    }

    bool S3Database::ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const int64_t count = _objectCollection.count_documents(make_document(kvp("region", region), kvp("bucket", bucket), kvp("key", key)));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.ObjectExists(region, bucket, key);
    }

    bool S3Database::ObjectExists(const std::string &oid) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const int64_t count = _objectCollection.count_documents(make_document(kvp("_id", bsoncxx::oid(oid))));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.ObjectExists(oid);
    }

    bool S3Database::ObjectExistsInternalName(const std::string &filename) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            document query;
            if (!filename.empty()) {
                query.append(kvp("internalName", filename));
            }

            const int64_t count = _objectCollection.count_documents(query.extract());
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.ObjectExistsInternalName(filename);
    }

    Entity::S3::Object S3Database::CreateObject(Entity::S3::Object &object) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto insert_one_result = _objectCollection.insert_one(object.ToDocument().view());
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
                throw Core::DatabaseException(exc.what());
            }
        }

        // Adjust bucket counters
        AdjustBucketCounters(object.region, object.bucket);

        return _memoryDb.CreateObject(object);
    }

    Entity::S3::Object S3Database::GetObjectById(bsoncxx::oid oid) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const std::optional<value> mResult = _objectCollection.find_one(make_document(kvp("_id", oid)));

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

    Entity::S3::Object S3Database::GetObjectById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetObjectById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetObjectById(oid);
    }

    Entity::S3::Object S3Database::CreateOrUpdateObject(Entity::S3::Object &object) const {

        if (ObjectExists(object)) {
            return UpdateObject(object);
        }
        return CreateObject(object);
    }

    Entity::S3::Object S3Database::UpdateObject(Entity::S3::Object &object) const {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                object.modified = system_clock::now();
                const auto mResult = _objectCollection.find_one_and_update(make_document(kvp("region", object.region), kvp("bucket", object.bucket), kvp("key", object.key)), object.ToDocument(), opts);
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
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateObject(object);
    }

    Entity::S3::Object S3Database::GetObject(const std::string &region, const std::string &bucket, const std::string &key) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

                if (const auto mResult = _objectCollection.find_one(make_document(kvp("region", region), kvp("bucket", bucket), kvp("key", key)))) {
                    Entity::S3::Object result;
                    result.FromDocument(mResult->view());

                    log_trace << "Got object: " << result.ToString();
                    return result;
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.GetObject(region, bucket, key);
        }
        return {};
    }

    Entity::S3::Object S3Database::GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                const std::optional<value>
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
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetObjectMd5(region, bucket, key, md5sum);
    }

    Entity::S3::Object S3Database::GetObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const auto mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                          kvp("bucket", bucket),
                                                                          kvp("key", key),
                                                                          kvp("versionId", version)));
            if (mResult) {
                Entity::S3::Object result;
                result.FromDocument(mResult->view());

                log_trace << "Got object version: " << result.ToString();
                return result;
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
        return {};
    }

    Entity::S3::ObjectList S3Database::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {

        if (HasDatabase()) {
            Entity::S3::ObjectList objectList;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            document query;
            if (prefix.empty()) {
                query.append(kvp("region", region));
            }
            if (prefix.empty()) {
                query.append(kvp("bucket", bucket));
            }
            if (prefix.empty()) {
                query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
            }

            for (auto objectCursor = _objectCollection.find(query.extract()); const auto &object: objectCursor) {
                Entity::S3::Object result;
                result.FromDocument(object);
                objectList.push_back(result);
            }
            log_trace << "Got versioned list, size:" << objectList.size();
            return objectList;
        }
        return _memoryDb.ListObjectVersions(region, bucket, prefix);
    }

    long S3Database::ObjectCount(const std::string &region, const std::string &prefix, const std::string &bucket) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

                long count = 0;
                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!bucket.empty()) {
                    query.append(kvp("bucket", bucket));
                }
                if (!prefix.empty()) {
                    query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
                }

                count = _objectCollection.count_documents(query.extract());
                log_trace << "Object count: " << count;

                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Object count failed, error: " << e.what();
            }
            return -1;
        }
        return _memoryDb.ObjectCount(region, bucket);
    }

    Entity::S3::ObjectList S3Database::ListObjects(const std::string &region, const std::string &prefix, const std::string &bucket, const int pageSize, const int pageIndex, const std::vector<Core::SortColumn> &sortColumns) const {

        Entity::S3::ObjectList objectList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (pageSize > 0) {
                opts.limit(pageSize);
                if (pageIndex > 0) {
                    opts.skip(pageIndex * pageSize);
                }
            }

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!bucket.empty()) {
                query.append(kvp("bucket", bucket));
            }
            if (!prefix.empty()) {
                query.append(kvp("key", make_document(kvp("$regex", "^" + prefix))));
            }

            for (auto objectCursor = _objectCollection.find(query.extract(), opts); const auto &object: objectCursor) {
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

    void S3Database::DeleteObject(const Entity::S3::Object &object) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                const auto result = _objectCollection.delete_many(make_document(kvp("region", object.region),
                                                                                kvp("bucket", object.bucket),
                                                                                kvp("key", object.key)));
                log_debug << "Objects deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteObject(object);
        }

        // Adjust bucket counter
        AdjustBucketCounters(object.region, object.bucket);
    }

    void S3Database::DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys) const {

        if (HasDatabase()) {

            array array{};
            for (const auto &key: keys) {
                array.append(key);
            }

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                document query = {};
                if (!bucket.empty()) {
                    query.append(kvp("bucket", bucket));
                }
                if (!keys.empty()) {
                    query.append(kvp("key", make_document(kvp("$in", array))));
                }

                const auto result = _objectCollection.delete_many(query.extract());
                log_debug << "Objects deleted, count: " << result->result().deleted_count();
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteObjects(bucket, keys);
        }
    }

    void S3Database::DeleteAllObjects() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _objectCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All objects deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteAllObjects();
        }
    }

    Entity::S3::Bucket S3Database::CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {

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

    Entity::S3::Bucket S3Database::DeleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {

        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);

        if (Core::StringUtils::Contains(bucketNotification.event, "*")) {

            std::vector<std::string> allowedEvents;
            if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectCreated:")) {
                allowedEvents = allowedEventTypes["Created"];
            } else if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectRemoved:")) {
                allowedEvents = allowedEventTypes["Deleted"];
            }

            for (const auto &it: allowedEvents) {
                std::erase_if(internBucket.notifications,
                              [it](const Entity::S3::BucketNotification &notification) {
                                  return it == notification.event;
                              });
            }

        } else {

            std::erase_if(internBucket.notifications,
                          [bucketNotification](const Entity::S3::BucketNotification &notification) {
                              return bucketNotification.event == notification.event;
                          });
        }

        log_trace << "Bucket notification deleted, notification: " << bucketNotification.ToString();

        return UpdateBucket(internBucket);
    }

}// namespace AwsMock::Database
