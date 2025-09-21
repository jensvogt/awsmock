//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/core/monitoring/MonitoringDefinition.h"
#include "awsmock/core/monitoring/MonitoringTimer.h"


#include <awsmock/repository/S3Database.h>

namespace AwsMock::Database {

    std::map<std::string, std::vector<std::string>> S3Database::allowedEventTypes = {
            {"Created", {"s3:ObjectCreated:Put", "s3:ObjectCreated:Post", "s3:ObjectCreated:Copy", "s3:ObjectCreated:CompleteMultipartUpload"}},
            {"Deleted", {"s3:ObjectRemoved:Delete", "s3:ObjectRemoved:DeleteMarkerCreated"}}};

    S3Database::S3Database() : _databaseName(GetDatabaseName()), _bucketCollectionName("s3_bucket"), _objectCollectionName("s3_object"), _memoryDb(S3MemoryDb::instance()) {}

    bool S3Database::BucketExists(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_exists");

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

    bool S3Database::BucketExists(const std::string &bucketArn) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_exists");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

                const int64_t count = _bucketCollection.count_documents(make_document(kvp("arn", bucketArn)));
                log_trace << "Bucket exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.BucketExists(bucketArn);
    }

    bool S3Database::BucketExists(const Entity::S3::Bucket &bucket) const {
        return BucketExists(bucket.region, bucket.name);
    }

    Entity::S3::Bucket S3Database::CreateBucket(Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "create_bucket");

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

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            bucket = _memoryDb.CreateBucket(bucket);
        }
        return bucket;
    }

    long S3Database::BucketCount(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "bucket_count");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        const auto mResult = _bucketCollection.find_one(make_document(kvp("_id", oid)));
        Entity::S3::Bucket result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::S3::Bucket S3Database::GetBucketByArn(const std::string &bucketArn) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];

        const auto mResult = _bucketCollection.find_one(make_document(kvp("arn", bucketArn)));
        Entity::S3::Bucket result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::S3::Bucket S3Database::GetBucketById(const std::string &oid) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        if (HasDatabase()) {

            return GetBucketById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetBucketById(oid);
    }

    Entity::S3::Bucket S3Database::GetBucketByRegionName(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            if (const auto mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name))); mResult) {
                Entity::S3::Bucket result;
                result.FromDocument(mResult->view());
                log_trace << "Got bucket: " << result.ToString();
                return result;
            }
            return {};
        }
        return _memoryDb.GetBucketByRegionName(region, name);
    }

    Entity::S3::BucketList S3Database::ListBuckets(const std::string &region, const std::string &prefix, const long maxResults, const long skip, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_bucket");

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

    Entity::S3::BucketList S3Database::ExportBuckets(const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "export_buckets");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "has_objects");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_list");

        if (HasDatabase()) {
            std::vector<Entity::S3::Object> objectList;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];

            mongocxx::options::find opts;
            if (maxKeys > 0) {
                opts.limit(maxKeys);
            }
            for (auto objectCursor = _objectCollection.find(make_document(kvp("region", region), kvp("bucket", bucket)), opts); auto object: objectCursor) {
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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_count");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "purge_count");

        long purged = 0;
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
                    purged = mResult.value().deleted_count();
                }

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {
            purged = _memoryDb.PurgeBucket(bucket);
        }
        return purged;
    }

    Entity::S3::Bucket S3Database::UpdateBucket(Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_bucket");

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

    void S3Database::UpdateBucketCounter(const std::string &bucketArn, const long keys, const long size) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_bucket_counter");

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();

                document filterQuery;
                filterQuery.append(kvp("arn", bucketArn));

                document setQuery;
                setQuery.append(kvp("keys", static_cast<bsoncxx::types::b_int64>(keys)));
                setQuery.append(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                _bucketCollection.update_one(filterQuery.extract(), updateQuery.extract());
                log_trace << "Bucket counter updated";
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.UpdateBucketCounter(bucketArn, keys, size);
    }

    Entity::S3::Bucket S3Database::CreateOrUpdateBucket(Entity::S3::Bucket &bucket) const {

        if (BucketExists(bucket)) {
            return UpdateBucket(bucket);
        }
        return CreateBucket(bucket);
    }

    // TODO: Combine with Listobject
    std::vector<Entity::S3::Object> S3Database::ListBucket(const std::string &bucket, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_objects");

        std::vector<Entity::S3::Object> objectList;
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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_bucket_size");

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

    void S3Database::DeleteBucket(const Entity::S3::Bucket &bucket) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_bucket");

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

        // Erase counter
        //_s3CounterMap.erase(bucket.arn);
    }

    long S3Database::DeleteAllBuckets() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_all_buckets");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _bucketCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All buckets deleted, count: " << delete_many_result->deleted_count();
                deleted = delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {
            deleted = _memoryDb.DeleteAllBuckets();
        }

        // Clear counters
        //_s3CounterMap.clear();

        return deleted;
    }

    bool S3Database::ObjectExists(const Entity::S3::Object &object) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const int64_t count = _objectCollection.count_documents(make_document(kvp("region", object.region), kvp("bucket", object.bucket), kvp("key", object.key)));
            log_trace << "Object exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.ObjectExists(object);
    }

    bool S3Database::ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "object_exists");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "create_object");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {
                session.start_transaction();
                const auto insert_one_result = _objectCollection.insert_one(object.ToDocument().view());
                object.oid = insert_one_result->inserted_id().get_oid().value.to_string();
                session.commit_transaction();
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {
            object = _memoryDb.CreateObject(object);
        }

        // Update counter
        //(*_s3CounterMap)[object.bucketArn].keys++;
        //(*_s3CounterMap)[object.bucketArn].size += object.size;

        return object;
    }

    Entity::S3::Object S3Database::GetObjectById(bsoncxx::oid oid) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            const auto mResult = _objectCollection.find_one(make_document(kvp("_id", oid)));

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "update_object");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
                const auto mResult = _objectCollection.find_one(make_document(kvp("region", region),
                                                                              kvp("bucket", bucket),
                                                                              kvp("key", key),
                                                                              kvp("md5sum", md5sum)));
                if (mResult) {
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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "get_object_version");

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

    std::vector<Entity::S3::Object> S3Database::ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_object_versions");

        if (HasDatabase()) {
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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "count_objects");

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

    std::vector<Entity::S3::Object> S3Database::ListObjects(const std::string &region, const std::string &prefix, const std::string &bucket, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "list_objects");

        std::vector<Entity::S3::Object> objectList;
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
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_objects");

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

        // Update monitoring counters
        AdjustObjectCounters();
    }

    void S3Database::DeleteObjects(const std::string &region, const std::string &bucketName, const std::vector<std::string> &keys) const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_objects");

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
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteObjects(bucketName, keys);
        }

        // Update monitoring counters
        AdjustObjectCounters();
    }

    long S3Database::DeleteAllObjects() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "delete_all_objects");

        long deleted = 0;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _objectCollection.delete_many({});
                session.commit_transaction();

                log_debug << "All objects deleted, count: " << result->deleted_count();
                deleted = result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {
            deleted = _memoryDb.DeleteAllObjects();
        }

        // Update monitoring counters
        AdjustObjectCounters();

        return deleted;
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

                Entity::S3::BucketNotification notification;
                notification.event = it;
                notification.notificationId = bucketNotification.notificationId;
                notification.queueArn = bucketNotification.queueArn;
                notification.lambdaArn = bucketNotification.lambdaArn;
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

    void S3Database::AdjustObjectCounters() const {
        Monitoring::MonitoringTimer measure(S3_DATABASE_TIMER, S3_DATABASE_COUNTER, "action", "adjust_object_counter");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto objectCollection = (*client)[_databaseName][_objectCollectionName];
            auto bucketCollection = (*client)[_databaseName][_bucketCollectionName];
            auto session = client->start_session();

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

                session.start_transaction();

                // Initialize all topics with zero message counts
                bucketCollection.update_many({}, make_document(kvp("$set", make_document(
                                                                                   kvp("size", bsoncxx::types::b_int64()),
                                                                                   kvp("keys", bsoncxx::types::b_int64())))));

                auto bulk = bucketCollection.create_bulk_write();
                for (auto cursor = objectCollection.aggregate(p); const auto t: cursor) {
                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("arn", Core::Bson::BsonUtils::GetStringValue(t, "bucketArn"))),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "size"))),
                                                              kvp("keys", bsoncxx::types::b_int64(Core::Bson::BsonUtils::GetLongValue(t, "keys"))))))));
                    log_debug << "Bucket: " << Core::Bson::BsonUtils::GetStringValue(t, "bucketArn")
                              << ", size: " << Core::Bson::BsonUtils::GetLongValue(t, "size")
                              << ", keys: " << Core::Bson::BsonUtils::GetLongValue(t, "keys");
                }

                // Bulk updates
                if (!bulk.empty()) {
                    try {
                        auto result = bulk.execute();
                        log_debug << "Bulk write result: " << result->modified_count();
                    } catch (const mongocxx::exception &exc) {
                        log_error << "Bulk write failed: " << exc.what();
                        throw Core::DatabaseException(exc.what());
                    }
                }
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
    }

}// namespace AwsMock::Database
