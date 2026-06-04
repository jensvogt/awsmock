//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/s3/S3MongoRepository.h>

namespace Awsmock::Database {

    std::map<std::string, std::vector<std::string>> S3MongoRepository::allowedEventTypes = {
            {"Created", {"s3:ObjectCreated:Put", "s3:ObjectCreated:Post", "s3:ObjectCreated:Copy", "s3:ObjectCreated:CompleteMultipartUpload"}},
            {"Deleted", {"s3:ObjectRemoved:Delete", "s3:ObjectRemoved:DeleteMarkerCreated"}}};

    bool S3MongoRepository::BucketExists(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_exists");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _bucketCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool S3MongoRepository::BucketExists(const std::string &bucketArn) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_exists");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _bucketCollection.count_documents(make_document(kvp("arn", bucketArn)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool S3MongoRepository::BucketExists(const Entity::S3::Bucket &bucket) const {
        return BucketExists(bucket.region, bucket.name);
    }

    Entity::S3::Bucket S3MongoRepository::CreateBucket(Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "create_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {

            const auto insert_one_result = _bucketCollection.insert_one(bucket.ToDocument());
            log_trace << "Bucket created, oid: " << insert_one_result->inserted_id().get_oid().value.to_string();

            bucket.oid = insert_one_result->inserted_id().get_oid().value.to_string();
            return bucket;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long S3MongoRepository::BucketCount(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_count");

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

            const long count = static_cast<long>(_bucketCollection.count_documents(query.view()));
            log_trace << "Bucket count: " << count;
            return count;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Bucket count failed, error: " << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    Entity::S3::Bucket S3MongoRepository::GetBucketById(const bsoncxx::oid &oid) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        const auto mResult = _bucketCollection.find_one(make_document(kvp("_id", oid)));

        return Entity::S3::Bucket::FromDocument(mResult->view());
    }

    Entity::S3::Bucket S3MongoRepository::GetBucketByArn(const std::string &bucketArn) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        const auto mResult = _bucketCollection.find_one(make_document(kvp("arn", bucketArn)));
        return Entity::S3::Bucket::FromDocument(mResult->view());
    }

    Entity::S3::Bucket S3MongoRepository::GetBucketById(const std::string &oid) const {
        return GetBucketById(bsoncxx::oid(oid));
    }

    Entity::S3::Bucket S3MongoRepository::GetBucketByRegionName(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
        if (const auto mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name))); mResult) {
            return Entity::S3::Bucket::FromDocument(mResult->view());
        }
        return {};
    }

    Entity::S3::BucketList S3MongoRepository::ListBuckets(const std::string &region, const std::string &prefix, const long maxResults, const long skip, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        mongocxx::options::find opts;
        if (!sortColumns.empty()) {
            document sort = {};
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

        document query = {};
        if (!region.empty()) {
            query.append(kvp("region", region));
        }
        if (!prefix.empty()) {
            query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
        }

        Entity::S3::BucketList bucketList;
        auto cursor = _bucketCollection.find(query.extract(), opts);
        std::transform(cursor.begin(), cursor.end(), std::back_inserter(bucketList), [](const auto &doc) {
            return Entity::S3::Bucket::FromDocument(doc);
        });
        return bucketList;
    }

    Entity::S3::BucketList S3MongoRepository::ExportBuckets(const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "export_buckets");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        mongocxx::options::find opts;
        if (!sortColumns.empty()) {
            document sort = {};
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        }

        Entity::S3::BucketList bucketList;
        auto cursor = _bucketCollection.find({}, opts);
        std::transform(cursor.begin(), cursor.end(), std::back_inserter(bucketList), [](const auto &doc) {
            return Entity::S3::Bucket::FromDocument(doc);
        });
        return bucketList;
    }

    bool S3MongoRepository::HasObjects(const Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "has_objects");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        // Set limit to 1 (Very important for performance!)
        mongocxx::options::count options;
        options.limit(1);

        return _objectCollection.count_documents(make_document(kvp("region", bucket.region), kvp("bucket", bucket.name)), options) > 0;
    }

    std::vector<Entity::S3::Object> S3MongoRepository::GetBucketObjectList(const std::string &region, const std::string &bucket, const long maxKeys) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_list");

        std::vector<Entity::S3::Object> objectList;

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        mongocxx::options::find opts;
        if (maxKeys > 0) {
            opts.limit(maxKeys);
        }
        for (auto objectCursor = _objectCollection.find(make_document(kvp("region", region), kvp("bucket", bucket)), opts); auto object: objectCursor) {
            objectList.push_back(Entity::S3::Object::FromDocument(object));
        }

        log_trace << "Objects exists, count: " << objectList.size();
        return objectList;
    }

    long S3MongoRepository::GetBucketObjectCount(const std::string &region, const std::string &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_count");

        std::vector<Entity::S3::Object> objectList;
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
        const long objectCount = static_cast<long>(_objectCollection.count_documents(make_document(kvp("region", region), kvp("bucket", bucket))));

        log_trace << "Objects count, count: " << objectCount;
        return objectCount;
    }

    long S3MongoRepository::PurgeBucket(Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "purge_count");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {

            const auto mResult = _objectCollection.delete_many(make_document(kvp("region", bucket.region), kvp("bucket", bucket.name)));
            log_trace << "Bucket purged: " << bucket.ToString();

            if (mResult) {
                log_debug << "Bucket purged, name: " << bucket.name << " deleted: " << mResult->deleted_count();
                return mResult.value().deleted_count();
            }
            return -1;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Bucket S3MongoRepository::UpdateBucket(Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_bucket");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {

            bucket.modified = system_clock::now();
            const auto mResult = _bucketCollection.find_one_and_update(make_document(kvp("region", bucket.region), kvp("name", bucket.name)), bucket.ToDocument(), opts);
            log_trace << "Bucket updated: " << bucket.ToString();

            if (mResult) {
                return Entity::S3::Bucket::FromDocument(mResult.value());
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void S3MongoRepository::UpdateBucketCounter(const std::string &bucketArn, const long keys, const long size) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_bucket_counter");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {


            document filterQuery;
            filterQuery.append(kvp("arn", bucketArn));

            document setQuery;
            setQuery.append(kvp("keys", static_cast<bsoncxx::types::b_int64>(keys)));
            setQuery.append(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));

            document updateQuery;
            updateQuery.append(kvp("$set", setQuery));

            _bucketCollection.update_one(filterQuery.extract(), updateQuery.extract());
            log_trace << "Bucket counter updated";

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Bucket S3MongoRepository::CreateOrUpdateBucket(Entity::S3::Bucket &bucket) const {

        if (BucketExists(bucket)) {
            return UpdateBucket(bucket);
        }
        return CreateBucket(bucket);
    }

    std::vector<Entity::S3::Object> S3MongoRepository::ListBucket(const std::string &bucket, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_objects");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        document query;
        if (!bucket.empty()) {
            query.append(kvp("bucket", bucket));
        }
        if (!prefix.empty()) {
            query.append(kvp("key", bsoncxx::types::b_regex{"^" + prefix + ".*"}));
        }

        std::vector<Entity::S3::Object> objectList;
        for (auto objectCursor = _objectCollection.find(query.view()); auto object: objectCursor) {
            objectList.push_back(Entity::S3::Object::FromDocument(object));
        }
        return objectList;
    }

    long S3MongoRepository::GetBucketSize(const std::string &region, const std::string &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket_size");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {
            mongocxx::pipeline p{};
            p.match(make_document(kvp("region", region), kvp("bucket", bucket)));
            p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
            p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
            auto totalSizeCursor = _objectCollection.aggregate(p);
            if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                std::cerr << bsoncxx::to_json(t) << std::endl;
                return t["totalSize"].get_int64().value;
            }
            return 0;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    void S3MongoRepository::DeleteBucket(const Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {

            const auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("name", bucket.name)));
            log_debug << "Bucket deleted, count: " << delete_many_result->deleted_count();
            return;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long S3MongoRepository::DeleteAllBuckets() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_all_buckets");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {

            const auto delete_many_result = _bucketCollection.delete_many({});
            log_debug << "All buckets deleted, count: " << delete_many_result->deleted_count();
            const long deleted = delete_many_result->deleted_count();
            return deleted;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool S3MongoRepository::ObjectExists(const Entity::S3::Object &object) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        // Set limit to 1 (Very important for performance!)
        mongocxx::options::count options;
        options.limit(1);

        return _objectCollection.count_documents(make_document(kvp("region", object.region), kvp("bucket", object.bucket), kvp("key", object.key)), options) > 0;
    }

    bool S3MongoRepository::ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        // Set limit to 1 (Very important for performance!)
        mongocxx::options::count options;
        options.limit(1);

        return _objectCollection.count_documents(make_document(kvp("region", region), kvp("bucket", bucket), kvp("key", key)), options) > 0;
    }

    bool S3MongoRepository::ObjectExists(const std::string &oid) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
        const int64_t count = _objectCollection.count_documents(make_document(kvp("_id", bsoncxx::oid(oid))));
        log_trace << "Object exists: " << std::boolalpha << count;
        return count > 0;
    }

    bool S3MongoRepository::ObjectExistsInternalName(const std::string &filename) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        // Set limit to 1 (Very important for performance!)
        mongocxx::options::count options;
        options.limit(1);

        document query;
        if (!filename.empty()) {
            query.append(kvp("internalName", filename));
        }

        return _objectCollection.count_documents(query.view(), options) > 0;
    }

    Entity::S3::Object S3MongoRepository::CreateObject(Entity::S3::Object &object) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "create_object");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {
            const auto insert_one_result = _objectCollection.insert_one(object.ToDocument().view());
            object.oid = insert_one_result->inserted_id().get_oid().value.to_string();
            return object;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Object S3MongoRepository::GetObjectById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const auto mResult = _objectCollection.find_one(make_document(kvp("_id", oid)));

            if (mResult->empty()) {
                return {};
            }
            return Entity::S3::Object::FromDocument(mResult->view());

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Get object by ID failed, error: " << e.what();
        }
        return {};
    }

    Entity::S3::Object S3MongoRepository::GetObjectById(const std::string &oid) const {
        return GetObjectById(bsoncxx::oid(oid));
    }

    Entity::S3::Object S3MongoRepository::CreateOrUpdateObject(Entity::S3::Object &object) const {
        if (ObjectExists(object)) {
            return UpdateObject(object);
        }
        return CreateObject(object);
    }

    Entity::S3::Object S3MongoRepository::UpdateObject(Entity::S3::Object &object) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_object");

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {

            object.modified = system_clock::now();
            const auto mResult = _objectCollection.find_one_and_update(make_document(kvp("region", object.region), kvp("bucket", object.bucket), kvp("key", object.key)), object.ToDocument(), opts);
            log_trace << "Object updated: " << object.ToString();

            if (mResult) {
                return Entity::S3::Object::FromDocument(mResult->view());
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Object S3MongoRepository::GetObject(const std::string &region, const std::string &bucket, const std::string &key) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            if (const auto mResult = _objectCollection.find_one(make_document(kvp("region", region), kvp("bucket", bucket), kvp("key", key)))) {
                return Entity::S3::Object::FromDocument(mResult->view());
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Object S3MongoRepository::GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const auto mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                          kvp("bucket", bucket),
                                                                          kvp("key", key),
                                                                          kvp("md5sum", md5sum)));
            if (mResult) {
                return Entity::S3::Object::FromDocument(mResult->view());
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Object S3MongoRepository::GetObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_version");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const auto mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                          kvp("bucket", bucket),
                                                                          kvp("key", key),
                                                                          kvp("versionId", version)));
            if (mResult) {
                return Entity::S3::Object::FromDocument(mResult->view());
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
        return {};
    }

    std::vector<Entity::S3::Object> S3MongoRepository::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_object_versions");

        std::vector<Entity::S3::Object> objectList;

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

        for (auto objectCursor = _objectCollection.find(query.view()); const auto &object: objectCursor) {
            objectList.push_back(Entity::S3::Object::FromDocument(object));
        }
        log_trace << "Got versioned list, size:" << objectList.size();
        return objectList;
    }

    long S3MongoRepository::ObjectCount(const std::string &region, const std::string &prefix, const std::string &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "count_objects");

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

            count = static_cast<long>(_objectCollection.count_documents(query.view()));
            log_trace << "Object count: " << count;

            return count;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Object count failed, error: " << e.what();
        }
        return -1;
    }

    std::vector<Entity::S3::Object> S3MongoRepository::ListObjects(const std::string &region, const std::string &prefix, const std::string &bucket, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_objects");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        mongocxx::options::find opts;
        if (!sortColumns.empty()) {
            document sort = {};
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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

        std::vector<Entity::S3::Object> objectList;
        auto cursor = _objectCollection.find(query.extract(), opts);
        std::transform(cursor.begin(), cursor.end(), std::back_inserter(objectList),
                       [](const auto &doc) {
                           return Entity::S3::Object::FromDocument(doc);
                       });
        return objectList;
    }

    long S3MongoRepository::DeleteObject(const Entity::S3::Object &object) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_objects");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {


            const auto result = _objectCollection.delete_many(make_document(kvp("region", object.region),
                                                                            kvp("bucket", object.bucket),
                                                                            kvp("key", object.key)));
            log_debug << "Objects deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long S3MongoRepository::DeleteObjects(const std::string &region, const std::string &bucketName, const std::vector<std::string> &keys) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_objects");

        array array{};
        for (const auto &key: keys) {
            array.append(key);
        }

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!bucketName.empty()) {
                query.append(kvp("bucket", bucketName));
            }
            if (!keys.empty()) {
                query.append(kvp("key", make_document(kvp("$in", array))));
            }

            const auto result = _objectCollection.delete_many(query.extract());
            log_debug << "Objects deleted, count: " << result->result().deleted_count();
            return result->result().deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long S3MongoRepository::DeleteAllObjects() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_all_objects");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

        try {

            const auto result = _objectCollection.delete_many({});

            log_debug << "All objects deleted, count: " << result->deleted_count();
            const long deleted = result->deleted_count();
            return deleted;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::S3::Bucket S3MongoRepository::CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {

        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);
        //        internBucket.notifications.clear();
        if (Core::StringUtils::Contains(bucketNotification.event, "*")) {

            std::vector<std::string> allowedEvents;
            if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectCreated:")) {
                allowedEvents = allowedEventTypes["Created"];
            } else if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectRemoved:")) {
                allowedEvents = allowedEventTypes["Deleted"];
            }

            for (const auto &it: allowedEvents) {

                Entity::S3::BucketNotification notification;
                notification.event = it;
                notification.notificationId = bucketNotification.notificationId;
                notification.queueArn = bucketNotification.queueArn;
                notification.lambdaArn = bucketNotification.lambdaArn;
                // internBucket.notifications.emplace_back(notification);
            }

        } else {

            // internBucket.notifications.emplace_back(bucketNotification);
        }

        log_debug << "Bucket notification added, notification: " << bucketNotification.ToString();

        return UpdateBucket(internBucket);
    }

    Entity::S3::Bucket S3MongoRepository::DeleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const {

        Entity::S3::Bucket internBucket = GetBucketByRegionName(bucket.region, bucket.name);

        if (Core::StringUtils::Contains(bucketNotification.event, "*")) {

            std::vector<std::string> allowedEvents;
            if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectCreated:")) {
                allowedEvents = allowedEventTypes["Created"];
            } else if (Core::StringUtils::StartsWith(bucketNotification.event, "s3:ObjectRemoved:")) {
                allowedEvents = allowedEventTypes["Deleted"];
            }

            // for (const auto &it: allowedEvents) {
            //     std::erase_if(internBucket.notifications,
            //                   [it](const Entity::S3::BucketNotification &notification) {
            //                       return it == notification.event;
            //                   });
            // }

        } else {

            // std::erase_if(internBucket.notifications,
            //               [bucketNotification](const Entity::S3::BucketNotification &notification) {
            //                   return bucketNotification.event == notification.event;
            //               });
        }

        log_trace << "Bucket notification deleted, notification: " << bucketNotification.ToString();

        return UpdateBucket(internBucket);
    }

    void S3MongoRepository::AdjustObjectCounters() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "adjust_object_counter");

        const auto client = ConnectionPool::instance().GetConnection();
        auto objectCollection = (*client)[_databaseName][_objectCollectionName];
        auto bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        try {
            mongocxx::pipeline p{};
            p.group(make_document(
                    kvp("_id", "$bucketArn"),
                    kvp("size", make_document(kvp("$sum", "$size"))),
                    kvp("keys", make_document(kvp("$sum", 1)))));

            document projectDocument;
            projectDocument.append(kvp("_id", 0),
                                   kvp("bucketArn", "$_id"),
                                   kvp("size", 1),
                                   kvp("keys", 1));
            p.project(projectDocument.extract());

            std::set<std::string> bucketsWithObjects;
            auto bulk = bucketCollection.create_bulk_write();
            for (auto cursor = objectCollection.aggregate(p); const auto t: cursor) {
                bucketsWithObjects.insert(Core::Bson::BsonUtils::GetStringValue(t, "bucketArn"));
                bulk.append(mongocxx::model::update_one(
                        make_document(kvp("arn", Core::Bson::BsonUtils::GetStringValue(t, "bucketArn"))),
                        make_document(kvp("$set", make_document(
                                                          kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                          kvp("keys", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "keys"))))))));
            }
            for (auto bucketCursor = bucketCollection.find({}); const auto &b: bucketCursor) {
                if (const auto bucketArn = Core::Bson::BsonUtils::GetStringValue(b, "arn"); !bucketsWithObjects.contains(bucketArn)) {
                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("arn", bucketArn)),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64()),
                                                              kvp("keys", bsoncxx::types::b_int64()))))));
                }
            }
            // Bulk updates
            if (!bulk.empty()) {
                auto result = bulk.execute();
                log_debug << "Bulk write result: " << result->modified_count();
            }

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace Awsmock::Database
