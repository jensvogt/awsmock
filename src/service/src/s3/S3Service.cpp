//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

    bool S3Service::BucketExists(const std::string &region, const std::string &bucket) const { return _database.BucketExists({.region = region, .name = bucket}); }

    Dto::S3::CreateBucketResponse S3Service::CreateBucket(const Dto::S3::CreateBucketRequest &s3Request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "create_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "create_bucket");
        log_trace << "Create bucket request, s3Request: " << s3Request.ToString();

        // Get region
        const std::string region = s3Request.region;
        const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");

        // Check existence
        CheckBucketNonExistence(region, s3Request.name);

        try {

            // Update database
            const std::string arn = Core::AwsUtils::CreateS3BucketArn(region, accountId, s3Request.name);
            Database::Entity::S3::Bucket bucket = {.region = region, .name = s3Request.name, .owner = s3Request.owner, .arn = arn};
            bucket = _database.CreateBucket(bucket);

            auto createBucketResponse = Dto::S3::CreateBucketResponse(region, Core::CreateArn("s3", region, accountId, s3Request.name));
            log_trace << "S3 create bucket response: " << createBucketResponse.ToXml();
            log_debug << "Bucket created, bucket: " << s3Request.name;
            return createBucketResponse;
        } catch (Core::JsonException &exc) {
            log_error << "S3 create bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void S3Service::PurgeBucket(const Dto::S3::PurgeBucketRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "purge_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "purge_bucket");
        log_trace << "Purge bucket request, s3Request: " << request.ToString();

        CheckBucketExistence(request.region, request.bucketName);

        try {
            // Get bucket
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucketName);

            // Purge bucket
            const long deleted = _database.PurgeBucket(bucket);
            AdjustBucketCounters(bucket.region, bucket.name);

            log_debug << "Bucket purged, region: " << request.region << " bucket: " << request.bucketName << "deleted: " << deleted;
        } catch (Core::JsonException &exc) {
            log_error << "S3 purge bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void S3Service::UpdateBucket(const Dto::S3::UpdateBucketRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "update_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "update_bucket");
        log_trace << "Update bucket request, s3Request: " << request.ToString();

        // Get region
        const std::string region = request.bucket.region;
        std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");

        // Check existence
        CheckBucketExistence(request.bucket.region, request.bucket.bucketName);

        try {

            // Mapp DTO to entity
            Dto::S3::CreateBucketResponse createBucketResponse;
            Database::Entity::S3::Bucket bucket = Dto::S3::Mapper::map(request.bucket);

            // Update database
            _database.UpdateBucket(bucket);
            log_debug << "Bucket updated, bucket: " << request.bucket.bucketName;
        } catch (Core::JsonException &exc) {
            log_error << "S3 create bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::S3::GetMetadataResponse S3Service::GetBucketMetadata(const Dto::S3::GetMetadataRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "get_bucket_metadata");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "get_bucket_metadata");
        log_trace << "Get bucket metadata request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);
            Dto::S3::GetMetadataResponse response = {
                    .region = bucket.region,
                    .bucket = bucket.name,
                    .created = bucket.created,
                    .modified = bucket.modified};

            log_trace << "S3 get bucket metadata response: " + response.ToString();
            log_debug << "Metadata returned, bucket: " << request.bucket << " key: " << request.key;

            return response;
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get object metadata failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetBucketResponse S3Service::GetBucket(const Dto::S3::GetBucketRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "get_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "get_bucket");
        log_trace << "Get bucket request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucketName);

        try {
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucketName);
            log_debug << "Bucket returned, bucket: " << request.bucketName;

            return Dto::S3::Mapper::map(request, bucket);
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetMetadataResponse S3Service::GetObjectMetadata(const Dto::S3::GetMetadataRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "get_object_metadata");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "get_object_metadata");
        log_trace << "Get metadata request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        if (!request.key.empty()) {
            if (!_database.ObjectExists({.region = request.region, .bucket = request.bucket, .key = request.key})) {
                log_debug << "Object does not exists, bucket: " << request.bucket << ", key: " << request.key;
                throw Core::NotFoundException("Object does not exists, bucket: " + request.bucket + ", key: " + request.key);
            }
        }

        try {
            const Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            Dto::S3::GetMetadataResponse response = {
                    .bucket = object.bucket,
                    .key = object.key,
                    .md5Sum = object.md5sum,
                    .contentType = object.contentType,
                    .size = object.size,
                    .metadata = object.metadata,
                    .created = object.created,
                    .modified = object.modified};

            log_trace << "S3 get object metadata response: " + response.ToString();
            log_debug << "Metadata returned, bucket: " << request.bucket << " key: " << request.key << " size: " << response.size;

            return response;
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get object metadata failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetObjectResponse S3Service::GetObject(const Dto::S3::GetObjectRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "get_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "get_object");
        log_trace << "Get object request, s3Request: " << request.ToString();
        const std::string s3DataDir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        if (!request.key.empty()) {
            if (!_database.ObjectExists({.region = request.region, .bucket = request.bucket, .key = request.key})) {
                log_error << "Object " << request.key << " does not exist";
                throw Core::NotFoundException("Object does not exist");
            }
        }

        try {
            const Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(request.region, request.bucket);

            Database::Entity::S3::Object object;
            if (bucketEntity.IsVersioned() && !request.versionId.empty()) {
                object = _database.GetObjectVersion(request.region, request.bucket, request.key, request.versionId);
                if (object.oid.empty()) {
                    log_error << "Object " << request.key << " does not exist";
                    throw Core::ServiceException("Object does not exist");
                }
            } else {
                object = _database.GetObject(request.region, request.bucket, request.key);
            }

            std::string filename = s3DataDir + Core::FileUtils::separator() + object.internalName;

            // Check decryption
            CheckDecryption(bucketEntity, object, filename);

            Dto::S3::GetObjectResponse response = {
                    .bucket = object.bucket,
                    .key = object.key,
                    .size = object.size,
                    .filename = filename,
                    .contentType = object.contentType,
                    .metadata = object.metadata,
                    .md5sum = object.md5sum,
                    .created = object.created,
                    .modified = object.modified};
            log_trace << "S3 get object response: " << response.ToString();
            log_debug << "Object returned, bucket: " << request.bucket << ", key: " << request.key << ", size: " << response.size;
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "S3 get object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListAllBucketResponse S3Service::ListAllBuckets() const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "list_buckets");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "list_buckets");
        log_trace << "List all buckets request";

        try {

            const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();
            Dto::S3::ListAllBucketResponse listAllBucketResponse;
            listAllBucketResponse.bucketList = bucketList;
            listAllBucketResponse.total = bucketList.size();
            log_debug << "Count all buckets, size: " << bucketList.size();
            return listAllBucketResponse;

        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListBucketCounterResponse S3Service::ListBucketCounters(const Dto::S3::ListBucketCounterRequest &s3Request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "list_bucket_counters");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "list_bucket_counters");
        log_trace << "List buckets counters request";

        try {
            Database::Entity::S3::BucketList bucketList = _database.ListBuckets(s3Request.region, s3Request.prefix, s3Request.pageSize, s3Request.pageIndex, s3Request.sortColumns);

            Dto::S3::ListBucketCounterResponse listAllBucketResponse;
            listAllBucketResponse.total = _database.BucketCount(s3Request.region, s3Request.prefix);

            for (const auto &bucket: bucketList) {
                Dto::S3::BucketCounter bucketCounter;
                bucketCounter.bucketName = bucket.name;
                bucketCounter.keys = bucket.keys;
                bucketCounter.size = bucket.size;
                bucketCounter.owner = bucket.owner;
                bucketCounter.created = bucket.created;
                bucketCounter.modified = bucket.modified;
                listAllBucketResponse.bucketCounters.emplace_back(bucketCounter);
            }
            log_debug << "Count all buckets, size: " << bucketList.size();

            return listAllBucketResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListBucketResponse S3Service::ListBucket(const Dto::S3::ListBucketRequest &s3Request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "list_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "list_bucket");
        log_trace << "List bucket request: " + s3Request.ToString();

        try {
            const Database::Entity::S3::ObjectList objectList = _database.ListBucket(s3Request.name, s3Request.prefix);
            auto listBucketResponse = Dto::S3::ListBucketResponse(s3Request.name, objectList);
            log_debug << "Bucket list returned, count: " << objectList.size();

            return listBucketResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::PutBucketVersioning(const Dto::S3::PutBucketVersioningRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "put_bucket_versioning");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "put_bucket_versioning");
        log_trace << "Put bucket versioning request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Update bucket
        Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);
        bucket.versionStatus = Database::Entity::S3::BucketVersionStatusFromString(Core::StringUtils::ToLower(request.status));

        _database.UpdateBucket(bucket);
        log_debug << "Put bucket versioning, bucket: " << request.bucket << " state: " << request.status;
    }

    Dto::S3::CreateMultipartUploadResult S3Service::CreateMultipartUpload(const Dto::S3::CreateMultipartUploadRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "create_multipart_upload");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "create_multipart_upload");
        log_trace << "CreateMultipartUpload request, bucket: " + request.bucket << " key: " << request.key << " region: " << request.region << " user: " << request.user;

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        const std::string uploadId = Core::StringUtils::GenerateRandomString(58);

        // Create upload directory, if not existing
        const std::string uploadDir = GetMultipartUploadDirectory(uploadId);
        Core::DirUtils::EnsureDirectory(uploadDir);

        // Create database object
        Database::Entity::S3::Object object = {.region = request.region,
                                               .bucket = request.bucket,
                                               .key = request.key,
                                               .owner = request.user,
                                               .metadata = request.metadata};
        object = _database.CreateOrUpdateObject(object);

        log_debug << "Multipart upload started, bucket: " << request.bucket << " key: " << request.key << " uploadId: " << uploadId;
        return {.region = request.region, .bucket = request.bucket, .key = request.key, .uploadId = uploadId};
    }

    std::string S3Service::UploadPart(std::istream &stream, int part, const std::string &updateId) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "upload_part");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "upload_part");
        log_trace << "UploadPart request, part: " << part << " updateId: " << updateId;

        std::string uploadDir = GetMultipartUploadDirectory(updateId);
        log_trace << "Using uploadDir: " << uploadDir;

        std::string fileName = uploadDir + Core::FileUtils::separator() + updateId + "-" + std::to_string(part);
        std::ofstream ofs(fileName, std::ios::binary);
        std::copy(std::istream_iterator<unsigned char>(stream), std::istream_iterator<unsigned char>(), std::ostream_iterator<unsigned char>(ofs));
        ofs.close();
        log_trace << "Part uploaded, part: " << part << " dir: " << uploadDir;

        // Get md5sum as ETag
        std::string eTag = Core::Crypto::GetMd5FromFile(fileName);
        log_debug << "Upload part succeeded, part: " << part << " filename: " << fileName;
        return eTag;
    }

    Dto::S3::UploadPartCopyResponse S3Service::UploadPartCopy(const Dto::S3::UploadPartCopyRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "upload_part_copy");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "upload_part_copy");
        log_trace << "UploadPart copy request, part: " << request.partNumber << " updateId: " << request.uploadId;

        const std::string s3DataDir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

        const std::string sourceFile = s3DataDir + Core::FileUtils::separator() + sourceObject.internalName;
        const std::string uploadDir = GetMultipartUploadDirectory(request.uploadId);
        log_trace << "Using uploadDir: " << uploadDir;

        long start = request.min;
        long length = request.max - request.min + 1;
        const std::string destFile = uploadDir + Core::FileUtils::separator() + request.uploadId + "-" + std::to_string(request.partNumber);
        const int dest = open(destFile.c_str(), O_WRONLY | O_CREAT, 0644);
        const int source = open(sourceFile.c_str(), O_RDONLY, 0);
#if __APPLE__
        const long copied = sendfile(dest, source, start, reinterpret_cast<off_t *>(&length), nullptr, 0);
#else
        const long copied = sendfile(dest, source, &start, length);
#endif
        close(source);
        close(dest);

        // Get md5sum as ETag
        Dto::S3::UploadPartCopyResponse response;
        response.eTag = Core::Crypto::GetMd5FromFile(destFile);
        response.checksumSHA1 = Core::Crypto::GetSha1FromFile(destFile);
        response.checksumSHA256 = Core::Crypto::GetSha256FromFile(destFile);
        response.lastModified = system_clock::now();
        log_debug << "Upload part copy succeeded, part: " << request.partNumber << " filename: " << destFile << " copied: " << copied;

        return response;
    }

    Dto::S3::CompleteMultipartUploadResult S3Service::CompleteMultipartUpload(const Dto::S3::CompleteMultipartUploadRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "complete_multipart_upload");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "complete_multipart_upload");
        log_trace << "CompleteMultipartUpload request, uploadId: " << request.uploadId << " bucket: " << request.bucket << " key: " << request.key << " region: " << request.region;

        // Get database object
        Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

        const std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Get all file parts
        if (const std::string uploadDir = GetMultipartUploadDirectory(request.uploadId); Core::DirUtils::DirectoryExists(uploadDir)) {
            const std::vector<std::string> files = Core::DirUtils::ListFilesByPrefix(uploadDir, request.uploadId);

            // Output file
            const std::string filename = Core::AwsUtils::CreateS3FileName();
            const std::string outFile = dataS3Dir + Core::FileUtils::separator() + filename;
            log_debug << "Output file, outFile: " << outFile;

            // Append all parts to the output file
            long fileSize = 0;
            try {
                fileSize = Core::FileUtils::AppendBinaryFiles(outFile, uploadDir, files);
                log_debug << "Input files appended to outfile, outFile: " << outFile << " size: " << fileSize;
            } catch (Core::JsonException &exc) { log_error << "Append to binary file failed, error: " << exc.message(); }

            // Get file size, MD5 sum
            const std::string md5sum = Core::Crypto::GetMd5FromFile(outFile);
            const std::string sha1sum = Core::Crypto::GetSha1FromFile(outFile);
            const std::string sha256sum = Core::Crypto::GetSha256FromFile(outFile);
            log_debug << "Metadata, bucket: " << request.bucket << " key: " << request.key << " md5: " << md5sum;

            // Update database object
            object.size = fileSize;
            object.md5sum = md5sum;
            object.internalName = filename;
            object.contentType = request.contentType;
            object = _database.UpdateObject(object);

            // Calculate the hashes asynchronously
            /*if (!request.checksumAlgorithm.empty()) {

                S3HashCreator s3HashCreator;
                const std::vector algorithms = {request.checksumAlgorithm};
                boost::thread t(boost::ref(s3HashCreator), algorithms, object);
                t.detach();
                log_debug << "Checksums, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum << " sha256: " << object.sha256sum;
            }*/

            // Cleanup
            Core::DirUtils::DeleteDirectory(uploadDir);

            // Check notifications
            CheckNotifications(request.region, request.bucket, request.key, object.size, "ObjectCreated");
            log_debug << "Multipart upload finished, bucket: " << request.bucket << " key: " << request.key;

            return {
                    .location = request.region,
                    .bucket = request.bucket,
                    .key = request.key,
                    .etag = md5sum,
                    .md5sum = md5sum,
                    .checksumSha1 = sha1sum,
                    .checksumSha256 = sha256sum};
        }
        return {
                .location = request.region,
                .bucket = request.bucket,
                .key = request.key,
                .etag = object.md5sum,
                .md5sum = object.md5sum,
                .checksumSha1 = object.sha1sum,
                .checksumSha256 = object.sha256sum};
    }

    Dto::S3::PutObjectResponse S3Service::PutObject(Dto::S3::PutObjectRequest &request, std::istream &stream) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "put_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "put_object");
        log_trace << "Put object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {

            // Get bucket
            if (const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket); bucket.IsVersioned()) {
                return SaveVersionedObject(request, bucket, stream);
            } else {
                return SaveUnversionedObject(request, bucket, stream, request.contentLength);
            }

            // Get bucket
            if (const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket); bucket.IsVersioned()) {
                return SaveVersionedObject(request, bucket, stream);
            } else {
                return SaveUnversionedObject(request, bucket, stream, request.contentLength);
            }
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::TouchObject(const Dto::S3::TouchObjectRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "touch_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "touch_object");
        log_trace << "Touch object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check existence
        if (!_database.ObjectExists({.region = request.region, .bucket = request.bucket, .key = request.key})) {
            log_error << "Bucket does not exist, region: " << request.region + " bucket: " << request.bucket;
            throw Core::NotFoundException("Bucket does not exist");
        }

        try {
            // Get the object
            const Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            // Check notification
            CheckNotifications(object.region, object.bucket, object.key, object.size, "ObjectCreated");
            log_debug << "Notifications send, bucket: " << request.bucket << " key: " << request.key;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 touch object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::UpdateObject(const Dto::S3::UpdateObjectRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "update_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "update_object");
        log_trace << "Update object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check existence
        if (!_database.ObjectExists({.region = request.region, .bucket = request.bucket, .key = request.key})) {
            log_error << "Bucket does not exist, region: " << request.region + " bucket: " << request.bucket;
            throw Core::NotFoundException("Bucket does not exist");
        }

        try {
            // Get the object
            Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            // Change metadata
            object.metadata = request.metadata;
            object = _database.UpdateObject(object);
            log_debug << "Metadata updated, bucket: " << object.bucket << " key: " << object.key;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 update object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::CopyObjectResponse S3Service::CopyObject(const Dto::S3::CopyObjectRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "copy_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "copy_object");
        log_trace << "Copy object request: " << request.ToString();

        const std::string dataDir = Core::Configuration::instance().GetValueString("awsmock.data-dir");
        const std::string dataS3Dir = dataDir + Core::FileUtils::separator() + "s3";
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Check existence
        CheckBucketExistence(request.region, request.sourceBucket);

        // Check existence of source key
        if (!_database.ObjectExists({.region = request.region, .bucket = request.sourceBucket, .key = request.sourceKey})) {
            log_error << "Source object does not exist, region: " << request.region + " bucket: " << request.sourceBucket << " key: " << request.sourceKey;
            throw Core::NotFoundException("Source object does not exist");
        }

        Dto::S3::CopyObjectResponse response;
        Database::Entity::S3::Object targetObject;
        try {

            // Check existence of target bucket
            if (!_database.BucketExists({.region = request.region, .name = request.targetBucket})) {
                log_error << "Target bucket does not exist, region: " << request.region + " bucket: " << request.targetBucket;
                throw Core::NotFoundException("Target bucket does not exist");
            }

            // Get the source object from the database
            const Database::Entity::S3::Bucket targetBucket = _database.GetBucketByRegionName(request.region, request.targetBucket);
            const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

            // Copy physical file
            const std::string targetFile = Core::AwsUtils::CreateS3FileName();
            const std::string sourcePath = dataS3Dir + Core::FileUtils::separator() + sourceObject.internalName;
            const std::string targetPath = dataS3Dir + Core::FileUtils::separator() + targetFile;
            Core::FileUtils::CopyTo(sourcePath, targetPath);

            // Update database
            targetObject = {
                    .region = request.region,
                    .bucket = request.targetBucket,
                    .key = request.targetKey,
                    .owner = sourceObject.owner,
                    .size = sourceObject.size,
                    .md5sum = sourceObject.md5sum,
                    .sha1sum = sourceObject.sha1sum,
                    .sha256sum = sourceObject.sha256sum,
                    .contentType = sourceObject.contentType,
                    .metadata = request.metadata,
                    .internalName = targetFile,
            };

            // Create version ID
            if (targetBucket.IsVersioned()) { targetObject.versionId = Core::AwsUtils::CreateS3VersionId(); }

            // Create object
            targetObject = _database.CreateObject(targetObject);
            log_debug << "Database updated, bucket: " << targetObject.bucket << " key: " << targetObject.key;

            // Check notification
            CheckNotifications(targetObject.region, targetObject.bucket, targetObject.key, targetObject.size, "ObjectCreated");
            log_debug << "Copy object succeeded, sourceBucket: " << request.sourceBucket << " sourceKey: " << request.sourceKey << " targetBucket: " << request.targetBucket << " targetKey: " << request.targetKey;

            // Adjust bucket key counter on target bucket
            AdjustBucketCounters(request.region, request.targetBucket);
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 copy object request failed, error: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        return {.eTag = targetObject.md5sum, .modified = system_clock::now()};
    }

    Dto::S3::MoveObjectResponse S3Service::MoveObject(const Dto::S3::MoveObjectRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "move_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "move_object");
        log_trace << "Move object request: " << request.ToString();

        const std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Check existence
        CheckBucketExistence(request.region, request.sourceBucket);

        // Check existence of source key
        if (!_database.ObjectExists({.region = request.region, .bucket = request.sourceBucket, .key = request.sourceKey})) {
            log_error << "Source object does not exist, region: " << request.region + " bucket: " << request.sourceBucket << " key: " << request.sourceKey;
            throw Core::NotFoundException("Source object does not exist");
        }

        Dto::S3::CopyObjectResponse response;
        Database::Entity::S3::Object targetObject;
        try {

            // Check existence of target bucket
            if (!_database.BucketExists({.region = request.region, .name = request.targetBucket})) {
                log_error << "Target bucket does not exist, region: " << request.region + " bucket: " << request.targetBucket;
                throw Core::ServiceException("Target bucket does not exist");
            }

            // Get the source object from the database
            const Database::Entity::S3::Bucket targetBucket = _database.GetBucketByRegionName(request.region, request.targetBucket);
            const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

            // Copy physical file
            const std::string targetFile = Core::AwsUtils::CreateS3FileName();
            const std::string sourcePath = dataS3Dir + Core::FileUtils::separator() + sourceObject.internalName;
            const std::string targetPath = dataS3Dir + Core::FileUtils::separator() + targetFile;
            Core::FileUtils::CopyTo(sourcePath, targetPath);

            // Update database
            targetObject = {
                    .region = request.region,
                    .bucket = request.targetBucket,
                    .key = request.targetKey,
                    .owner = sourceObject.owner,
                    .size = sourceObject.size,
                    .md5sum = sourceObject.md5sum,
                    .sha1sum = sourceObject.sha1sum,
                    .sha256sum = sourceObject.sha256sum,
                    .contentType = sourceObject.contentType,
                    .metadata = request.metadata,
                    .internalName = targetFile,
            };

            // Create version ID
            if (targetBucket.IsVersioned()) { targetObject.versionId = Core::AwsUtils::CreateS3VersionId(); }

            // Create object
            targetObject = _database.CreateObject(targetObject);
            log_debug << "Database updated, bucket: " << targetObject.bucket << " key: " << targetObject.key;

            // Check notification
            CheckNotifications(targetObject.region, targetObject.bucket, targetObject.key, targetObject.size, "ObjectCreated");
            log_debug << "Move object succeeded, sourceBucket: " << request.sourceBucket << " sourceKey: " << request.sourceKey << " targetBucket: "
                      << request.targetBucket << " targetKey: " << request.targetKey;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 copy object request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        return {.eTag = targetObject.md5sum, .lastModified = Core::DateTimeUtils::ToISO8601(system_clock::now())};
    }

    void S3Service::DeleteObject(const Dto::S3::DeleteObjectRequest &request) {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "delete_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "delete_object");
        log_trace << "Delete object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        if (_database.ObjectExists({.region = request.region, .bucket = request.bucket, .key = request.key})) {
            try {

                // Get object from database
                Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

                // Delete from database
                _database.DeleteObject(object);
                log_debug << "Database object deleted, bucket: " + request.bucket + " key: " << request.key;

                // Delete file system object
                DeleteObject(object.bucket, object.key, object.internalName);

                // Adjust bucket counters
                AdjustBucketCounters(request.region, request.bucket);

                // Check notifications
                CheckNotifications(request.region, request.bucket, request.key, 0, "ObjectRemoved");

                log_debug << "Object deleted, bucket: " << request.bucket << " key: " << request.key;
            } catch (Core::JsonException &exc) {
                log_error << "S3 delete object failed, message: " + exc.message();
                throw Core::ServiceException(exc.message());
            }
        }
    }

    Dto::S3::DeleteObjectsResponse S3Service::DeleteObjects(const Dto::S3::DeleteObjectsRequest &request) {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "delete_objects");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "delete_objects");
        log_trace << "Delete objects request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        Dto::S3::DeleteObjectsResponse response;
        response.keys = request.keys;
        try {

            // Delete file system objects
            for (const auto &key: request.keys) {
                if (!key.empty()) {

                    // Delete from database
                    Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, key);
                    log_debug << "Database object deleted, count: " << request.keys.size();

                    DeleteObject(object.bucket, object.key, object.internalName);
                    log_debug << "File system object deleted: " << key;

                    // Check notifications
                    CheckNotifications(request.region, request.bucket, key, 0, "ObjectRemoved");
                }
            }

            // Delete from database
            _database.DeleteObjects(request.bucket, request.keys);
            log_debug << "Database object deleted, count: " << request.keys.size();

            // Adjust bucket counters
            AdjustBucketCounters(request.region, request.bucket);

            log_debug << "Objects deleted, bucket: " << request.bucket << " count: " << request.keys.size();
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 delete objects failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        log_debug << "DeleteObjects succeeded, bucket: " << request.bucket;
        return response;
    }

    /*    void S3Service::PutBucketNotification(const Dto::S3::PutBucketNotificationRequest &request) {
        log_trace << "Put bucket notification request, userPoolId: " << request.notificationId;

        // Check bucket existence
        if (!_database.BucketExists({.region = request.region, .name = request.bucket})) {
            log_error << "Bucket does not exist, name: " << request.bucket;
            throw Core::NotFoundException("Bucket does not exist, name: " + request.bucket);
        }

        // Check notification existence
        Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);

        try {
            if (!request.lambdaArn.empty()) {
                CreateLambdaConfiguration(bucket, request);
            } else if (!request.queueArn.empty()) {
                CreateQueueConfiguration(bucket, request);
            }
            log_debug << "PutBucketNotification succeeded, bucket: " << request.bucket;

        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put bucket notification request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }*/

    void S3Service::PutBucketEncryption(const Dto::S3::PutBucketEncryptionRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "put_bucket_encryption");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "put_bucket_encryption");
        log_trace << "Put bucket encryption request, algorithm: " << request.sseAlgorithm;

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(request.region, request.bucket);

            const Database::Entity::S3::BucketEncryption bucketEncryption = {.sseAlgorithm = request.sseAlgorithm, .kmsKeyId = request.kmsKeyId};
            bucketEntity.bucketEncryption = bucketEncryption;
            bucketEntity = _database.UpdateBucket(bucketEntity);
            log_debug << "PutBucketEncryption succeeded, bucket: " << request.bucket;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put bucket encryption request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListObjectVersionsResponse S3Service::ListObjectVersions(const Dto::S3::ListObjectVersionsRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "list_option_versions");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "list_option_versions");
        log_trace << "List object versions request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Get bucket
        if (const Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(request.region, request.bucket); !bucketEntity.IsVersioned()) { throw Core::NotFoundException("Bucket is not versioned"); }

        Dto::S3::ListObjectVersionsResponse response;
        try {
            const std::vector<Database::Entity::S3::Object> objectList = _database.ListObjectVersions(request.region, request.bucket, request.prefix);
            return Dto::S3::Mapper::map(request, objectList);
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list object versions request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::DeleteBucket(const Dto::S3::DeleteBucketRequest &request) {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "delete_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "delete_bucket");
        log_trace << "Delete bucket request, name: " << request.bucket;

        const Database::Entity::S3::Bucket bucket = {.region = request.region, .name = request.bucket};

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check empty
        if (_database.HasObjects(bucket)) {
            log_error << "Bucket is not empty";
            throw Core::NotFoundException("Bucket is not empty");
        }

        try {

            // Delete directory
            DeleteBucket(request.bucket);

            // Delete bucket from database
            _database.DeleteBucket(bucket);
            log_debug << "Bucket deleted, bucket: " << bucket.name;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Delete Bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::CheckNotifications(const std::string &region, const std::string &bucket, const std::string &key, long size, const std::string &event) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "check_notifications");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "check_notifications");
        log_debug << "Check notifications, region: " << region << " bucket: " << bucket << " event: " << event;

        Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(region, bucket);

        // Create S3 bucket and object
        Dto::S3::Object s3Object = {.key = key, .size = size, .etag = Core::StringUtils::CreateRandomUuid()};
        Dto::S3::Bucket s3Bucket = {.bucketName = bucketEntity.name};

        if (bucketEntity.HasQueueNotificationEvent(event)) {
            if (Database::Entity::S3::QueueNotification notification = bucketEntity.GetQueueNotification(event); notification.CheckFilter(key)) {

                // Create the event record
                Dto::S3::NotificationBucket notificationBucket;
                notificationBucket.name = s3Bucket.bucketName;
                notificationBucket.arn = s3Bucket.arn;
                notificationBucket.ownerIdentity.principalId = s3Bucket.owner;
                Dto::S3::S3 s3 = {.configurationId = notification.id, .bucket = notificationBucket, .object = s3Object};

                Dto::S3::Record record = {.region = region, .eventName = event, .s3 = s3};
                Dto::S3::EventNotification eventNotification;

                eventNotification.records.push_back(record);
                log_debug << "Found notification records, count: " << eventNotification.records.size();

                // Queue notification
                SendQueueNotificationRequest(eventNotification, notification);
                log_trace << "SQS message created, eventNotification: " + eventNotification.ToString();
                log_debug << "SQS message created, queueArn: " << notification.queueArn;
            }
        }

        if (bucketEntity.HasTopicNotificationEvent(event)) {
            if (Database::Entity::S3::TopicNotification notification = bucketEntity.GetTopicNotification(event); notification.CheckFilter(key)) {

                // Create the event record
                Dto::S3::NotificationBucket notificationBucket;
                notificationBucket.name = s3Bucket.bucketName;
                notificationBucket.arn = s3Bucket.arn;
                notificationBucket.ownerIdentity.principalId = s3Bucket.owner;
                Dto::S3::S3 s3 = {.configurationId = notification.id, .bucket = notificationBucket, .object = s3Object};

                Dto::S3::Record record = {.region = region, .eventName = event, .s3 = s3};
                Dto::S3::EventNotification eventNotification;

                eventNotification.records.push_back(record);
                log_debug << "Found notification records, count: " << eventNotification.records.size();

                // Queue notification
                SendTopicNotificationRequest(eventNotification, notification);
                log_trace << "SNS message created, eventNotification: " + eventNotification.ToString();
                log_debug << "SNS message created, topicArn: " << notification.topicArn;
            }
        }

        if (bucketEntity.HasLambdaNotificationEvent(event)) {
            if (Database::Entity::S3::LambdaNotification notification = bucketEntity.GetLambdaNotification(event); notification.CheckFilter(key)) {

                // Create the event record
                Dto::S3::NotificationBucket notificationBucket;
                notificationBucket.name = s3Bucket.bucketName;
                notificationBucket.arn = s3Bucket.arn;
                notificationBucket.ownerIdentity.principalId = s3Bucket.owner;
                Dto::S3::S3 s3 = {.configurationId = notification.id, .bucket = notificationBucket, .object = s3Object};

                Dto::S3::Record record = {.region = region, .eventName = event, .s3 = s3};
                Dto::S3::EventNotification eventNotification;

                eventNotification.records.push_back(record);
                log_debug << "Found notification records, count: " << eventNotification.records.size();

                // Lambda notification
                SendLambdaInvocationRequest(eventNotification, notification);
                log_trace << "Lambda function invoked, eventNotification: " + eventNotification.ToString();
                log_debug << "Lambda function invoked, lambdaArn:" << notification.lambdaArn;
            }
        }
    }

    Dto::S3::PutBucketNotificationConfigurationResponse S3Service::PutBucketNotificationConfiguration(const Dto::S3::PutBucketNotificationConfigurationRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "put_bucket_notification");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "put_bucket_notification");

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            Dto::S3::PutBucketNotificationConfigurationResponse response;

            // Get bucket
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);
            log_debug << "Bucket received, region:" << bucket.region << " bucket: " << bucket.name;

            // Add notification configurations
            if (!request.queueConfigurations.empty()) { PutQueueNotificationConfigurations(bucket, request.queueConfigurations); }
            if (!request.topicConfigurations.empty()) { PutTopicNotificationConfigurations(bucket, request.topicConfigurations); }
            if (!request.lambdaConfigurations.empty()) { PutLambdaNotificationConfigurations(bucket, request.lambdaConfigurations); }

            // Update database
            bucket = _database.UpdateBucket(bucket);
            log_debug << "Bucket updated, region:" << bucket.region << " bucket: " << bucket.name;

            // Copy configurations
            response.queueConfigurations = request.queueConfigurations;
            response.topicConfigurations = request.topicConfigurations;
            response.lambdaConfigurations = request.lambdaConfigurations;

            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put notification configurations failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListObjectCounterResponse S3Service::ListObjectCounters(const Dto::S3::ListObjectCounterRequest &s3Request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "list_object_counters");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "list_object_counters");
        log_trace << "List objects counters request";

        try {
            Database::Entity::S3::ObjectList objectList = _database.ListObjects(s3Request.region, s3Request.prefix, s3Request.bucket, s3Request.pageSize, s3Request.pageIndex, s3Request.sortColumns);

            Dto::S3::ListObjectCounterResponse listAllObjectResponse;
            listAllObjectResponse.total = _database.ObjectCount(s3Request.region, s3Request.prefix, s3Request.bucket);

            for (const auto &object: objectList) {
                Dto::S3::ObjectCounter objectCounter;
                objectCounter.oid = object.oid;
                objectCounter.bucketName = object.bucket;
                objectCounter.key = object.key;
                objectCounter.size = object.size;
                objectCounter.contentType = object.contentType;
                objectCounter.created = object.created;
                objectCounter.modified = object.modified;
                objectCounter.metadata = object.metadata;
                listAllObjectResponse.objectCounters.emplace_back(objectCounter);
            }
            log_debug << "Count all objects, size: " << objectList.size();

            return listAllObjectResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetObjectCounterResponse S3Service::GetObjectCounters(const Dto::S3::GetObjectCounterRequest &request) const {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "get_object_counters");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "get_object_counters");
        log_trace << "Get objects counters request";

        // Check existence
        if (!_database.ObjectExists(request.oid)) {
            log_error << "Object does not exists, name: " << request.oid;
            throw Core::NotFoundException("Object does not exists, id: " + request.oid);
        }

        try {
            const Database::Entity::S3::Object object = _database.GetObjectById(request.oid);

            Dto::S3::GetObjectCounterResponse getObjectCounterResponse;

            Dto::S3::ObjectCounter objectCounter;
            objectCounter.oid = object.oid;
            objectCounter.region = object.region;
            objectCounter.bucketName = object.bucket;
            objectCounter.key = object.key;
            objectCounter.size = object.size;
            objectCounter.contentType = object.contentType;
            objectCounter.internalName = object.internalName;
            objectCounter.created = object.created;
            objectCounter.modified = object.modified;
            getObjectCounterResponse.objectCounter = objectCounter;
            return getObjectCounterResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::DeleteObject(const std::string &bucket, const std::string &key, const std::string &internalName) {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "delete_object");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "delete_object");

        const std::string dataDir = Core::Configuration::instance().GetValueString("awsmock.data-dir");
        const std::string dataS3Dir = dataDir + Core::FileUtils::separator() + "s3";
        const std::string transferDir = dataDir + Core::FileUtils::separator() + "transfer";
        Core::DirUtils::EnsureDirectory(dataS3Dir);
        Core::DirUtils::EnsureDirectory(transferDir);

        if (!internalName.empty()) {
            std::string filename = dataS3Dir + Core::FileUtils::separator() + internalName;
            Core::FileUtils::DeleteFile(filename);
            log_debug << "File system object deleted, filename: " << filename;

            if (const std::string transferBucket = Core::Configuration::instance().GetValueString("awsmock.modules.transfer.bucket"); bucket == transferBucket) {
                filename = transferDir + Core::FileUtils::separator() + key;
                Core::FileUtils::DeleteFile(filename);
                log_debug << "Transfer file system object deleted, filename: " << filename;
            }
        }
    }

    void S3Service::DeleteBucket(const std::string &bucket) {
        Monitoring::MetricServiceTimer measure(S3_SERVICE_TIMER, "action", "delete_bucket");
        Monitoring::MetricService::instance().IncrementCounter(S3_SERVICE_COUNTER, "action", "delete_bucket");

        const std::string dataDir = Core::Configuration::instance().GetValueString("awsmock.data-dir");
        const std::string dataS3Dir = dataDir + Core::FileUtils::separator() + "s3";
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        if (const std::string bucketDir = dataS3Dir + Core::FileUtils::separator() + bucket; Core::DirUtils::DirectoryExists(bucketDir)) {
            Core::DirUtils::DeleteDirectory(bucketDir);
            log_debug << "Bucket directory deleted, bucketDir: " + bucketDir;
        }
    }

    std::string S3Service::GetMultipartUploadDirectory(const std::string &uploadId) {
        const std::string tempDir = Core::Configuration::instance().GetValueString("awsmock.temp-dir");
        Core::DirUtils::EnsureDirectory(tempDir);
        return tempDir + Core::FileUtils::separator() + uploadId;
    }

    void S3Service::SendQueueNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::QueueNotification &queueNotification) {

        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");

        // Get queue URL
        const std::string queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(queueNotification.queueArn);

        const SQSService _sqsService;
        const Dto::SQS::SendMessageRequest request = {.region = region, .queueUrl = queueUrl, .queueArn = queueNotification.queueArn, .body = eventNotification.ToJson()};
        const Dto::SQS::SendMessageResponse response = _sqsService.SendMessage(request);
        log_debug << "SQS message request send, messageId: " << response.messageId;
    }

    void S3Service::SendTopicNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::TopicNotification &topicNotification) {

        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");

        const SNSService _snsService;
        const Dto::SNS::PublishRequest request = {.region = region, .targetArn = topicNotification.topicArn, .message = eventNotification.ToJson()};
        auto [messageId, requestId] = _snsService.Publish(request);
        log_debug << "SNS message request send, messageId: " << messageId;
    }

    void S3Service::SendLambdaInvocationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::LambdaNotification &lambdaNotification) const {

        const std::string region = Core::Configuration::instance().GetValueString("awsmock.region");
        const std::string user = Core::Configuration::instance().GetValueString("awsmock.user");

        const std::vector<std::string> parts = Core::StringUtils::Split(lambdaNotification.lambdaArn, ':');
        const std::string &functionName = parts[6];
        log_debug << "Invocation request function name: " << functionName;

        _lambdaService.InvokeLambdaFunction(region, functionName, eventNotification.ToJson());
        log_debug << "Lambda invocation finished send";
    }

    Dto::S3::PutObjectResponse S3Service::SaveUnversionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream, long size) const {

        std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Write file
        std::string fileName = Core::AwsUtils::CreateS3FileName();
        std::string filePath = dataS3Dir + Core::FileUtils::separator() + fileName;

        // Write file in chunks
        std::ofstream ofs(filePath, std::ios::binary | std::ios::trunc);
        constexpr std::size_t buffer_size = 4096;
        char buffer[buffer_size];
        long count = size;
        while (count > buffer_size) {
            stream.read(buffer, buffer_size);
            ofs.write(buffer, buffer_size);
            count -= buffer_size;
        }
        stream.read(buffer, count);
        ofs.write(buffer, count);
        ofs.close();

        // Check file encoding
        if (Core::FileUtils::IsBase64(filePath)) {
            log_debug << "File is base64 encoded, file: " << filePath;
            Core::FileUtils::Base64DecodeFile(filePath);
        }

        // Get content type
        std::string contentType = Core::FileUtils::GetContentType(filePath);

        // Create entity
        Database::Entity::S3::Object object = {
                .region = request.region,
                .bucket = request.bucket,
                .key = request.key,
                .owner = request.owner,
                .size = size,
                .contentType = contentType,
                .metadata = request.metadata,
                .internalName = fileName};

        // Meta data
        object.md5sum = Core::Crypto::GetMd5FromFile(filePath);
        log_debug << "Checksum, bucket: " << request.bucket << " key: " << request.key << " md5: " << object.md5sum;
        if (!request.checksumAlgorithm.empty()) {
            S3HashCreator s3HashCreator;
            std::vector algorithms = {request.checksumAlgorithm};
            boost::thread t(boost::ref(s3HashCreator), algorithms, object);
            t.detach();
            log_debug << "Checksums, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum << " sha256: " << object.sha256sum;
        }

        // Update database
        object = _database.CreateOrUpdateObject(object);
        log_debug << "Database updated, bucket: " << object.bucket << " key: " << object.key;

        // Check encryption
        CheckEncryption(bucket, object);
        log_debug << "Put object succeeded, bucket: " << request.bucket << " key: " << request.key;

        // Check notification
        CheckNotifications(request.region, request.bucket, request.key, object.size, "ObjectCreated");
        log_debug << "Notifications send, bucket: " << request.bucket << " key: " << request.key;

        return {
                .bucket = request.bucket,
                .key = request.key,
                .etag = object.md5sum,
                .md5Sum = object.md5sum,
                .contentLength = size,
                .sha1Sum = object.sha1sum,
                .sha256sum = object.sha256sum,
                .metadata = request.metadata};
    }

    Dto::S3::PutObjectResponse S3Service::SaveVersionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream) const {

        // S3 data directory
        std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Write file
        std::string fileName = Core::AwsUtils::CreateS3FileName();
        std::string filePath = dataS3Dir + Core::FileUtils::separator() + fileName;

        std::ofstream ofs(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
        ofs << stream.rdbuf();
        ofs.close();

        // Get size
        long size = Core::FileUtils::FileSize(filePath);
        log_debug << "File received, filePath: " << filePath << " size: " << size;

        Database::Entity::S3::Object object;

        // Check existence by
        std::string md5sum = Core::Crypto::HexEncode(Core::Crypto::Base64Decode(request.md5Sum));
        if (Database::Entity::S3::Object existingObject = _database.GetObjectMd5(request.region, request.bucket, request.key, md5sum); existingObject.oid.empty()) {

            // Version ID
            std::string versionId = Core::AwsUtils::CreateS3VersionId();

            // Create new version of new object
            object = {
                    .region = request.region,
                    .bucket = request.bucket,
                    .key = request.key,
                    .owner = request.owner,
                    .size = size,
                    .contentType = request.contentType,
                    .metadata = request.metadata,
                    .internalName = fileName,
                    .versionId = versionId,
            };

            // Checksums
            object.md5sum = Core::Crypto::GetMd5FromFile(filePath);
            log_debug << "Checksum, bucket: " << request.bucket << " key: " << request.key << " md5: " << object.md5sum;

            // Create new version in database
            object = _database.CreateObject(object);
            log_debug << "Database updated, bucket: " << object.bucket << " key: " << object.key;

            // Checksums
            if (!request.checksumAlgorithm.empty()) {
                S3HashCreator s3HashCreator;
                std::vector algorithms = {request.checksumAlgorithm};
                boost::thread t(boost::ref(s3HashCreator), algorithms, object);
                t.detach();
                //Core::TaskPool::instance().Add<std::string, S3HashCreator>("s3-hashing", S3HashCreator({request.checksumAlgorithm}, object));
                log_debug << "Checksums, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum << " sha256: " << object.sha256sum;
            }

            // Check encryption
            CheckEncryption(bucket, object);
            log_debug << "Put object succeeded, bucket: " << request.bucket << " key: " << request.key;

            // Check notification
            CheckNotifications(request.region, request.bucket, request.key, object.size, "ObjectCreated");
            log_debug << "Put object succeeded, bucket: " << request.bucket << " key: " << request.key;
        } else {

            // Delete local file
            Core::FileUtils::DeleteFile(filePath);
        }

        // Meta data
        object.md5sum = Core::Crypto::GetMd5FromFile(filePath);
        log_debug << "Checksum, bucket: " << request.bucket << " key: " << request.key << "md5: " << object.md5sum;
        if (request.checksumAlgorithm == "SHA1") {
            object.sha1sum = Core::Crypto::GetSha1FromFile(filePath);
            log_debug << "Checksum SHA1, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum;
        }
        if (request.checksumAlgorithm == "SHA256") {
            object.sha256sum = Core::Crypto::GetSha256FromFile(filePath);
            log_debug << "Checksum SHA256, bucket: " << request.bucket << " key: " << request.key << " sha256: " << object.sha256sum;
        }

        return {
                .bucket = request.bucket,
                .key = request.key,
                .etag = object.md5sum,
                .md5Sum = object.md5sum,
                .contentLength = size,
                .sha1Sum = object.sha1sum,
                .sha256sum = object.sha256sum,
                .metadata = request.metadata,
                .versionId = object.versionId};
    }

    void S3Service::PutQueueNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::QueueConfiguration> &queueConfigurations) {
        for (const auto &[id, queueArn, filterRules, events]: queueConfigurations) {

            // Check existence
            if (!id.empty() && bucket.HasQueueNotificationId(id)) {
                log_debug << "Queue notification configuration exists already, id: " << id;
                break;
            }

            // General attributes
            const std::string attrId = id.empty()
                                               ? Core::StringUtils::CreateRandomUuid()
                                               : id;
            Database::Entity::S3::QueueNotification queueNotification = {.id = attrId, .queueArn = queueArn};

            // Get events
            for (const auto &event: events) { queueNotification.events.emplace_back(EventTypeToString(event)); }

            // Get filter rules
            for (const auto &[name, value]: filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity = {.name = NameTypeToString(name), .value = value};
                queueNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.queueNotifications.emplace_back(queueNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::PutTopicNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::TopicConfiguration> &topicConfigurations) {
        for (const auto &[id, topicArn, filterRules, events]: topicConfigurations) {

            // Check existence
            if (!id.empty() && bucket.HasTopicNotificationId(id)) {
                log_debug << "Topic notification configuration exists already, id: " << id;
                break;
            }

            // General attributes
            const std::string attrId = id.empty()
                                               ? Core::StringUtils::CreateRandomUuid()
                                               : id;
            Database::Entity::S3::TopicNotification topicNotification = {.id = attrId, .topicArn = topicArn};

            // Get events
            for (const auto &event: events) { topicNotification.events.emplace_back(Dto::S3::EventTypeToString(event)); }

            // Get filter rules
            for (const auto &[name, value]: filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity = {.name = Dto::S3::NameTypeToString(name), .value = value};
                topicNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.topicNotifications.emplace_back(topicNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::PutLambdaNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::LambdaConfiguration> &lambdaConfigurations) {
        for (const auto &[id, lambdaArn, filterRules, events]: lambdaConfigurations) {

            // Check existence
            if (!id.empty() && bucket.HasLambdaNotificationId(id)) {
                log_debug << "Lambda notification configuration exists already, id: " << id;
                break;
            }

            // General attributes
            const std::string attrId = id.empty()
                                               ? Core::StringUtils::CreateRandomUuid()
                                               : id;
            Database::Entity::S3::LambdaNotification lambdaNotification = {.id = attrId, .lambdaArn = lambdaArn};

            // Get events
            for (const auto &event: events) { lambdaNotification.events.emplace_back(EventTypeToString(event)); }

            // Get filter rules
            for (const auto &[name, value]: filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity = {.name = Dto::S3::NameTypeToString(name), .value = value};
                lambdaNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.lambdaNotifications.emplace_back(lambdaNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::CheckEncryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object) {
        const std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);
        if (bucket.HasEncryption()) {
            const Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();
            const Database::Entity::KMS::Key kmsKey = kmsDatabase.GetKeyByKeyId(bucket.bucketEncryption.kmsKeyId);
            log_debug << kmsKey.keyId << " " << kmsKey.aes256Key;
            unsigned char *rawKey = Core::Crypto::HexDecode(kmsKey.aes256Key);
            Core::Crypto::Aes256EncryptFile(dataS3Dir + "/" + object.internalName, rawKey);
        }
    }

    void S3Service::CheckDecryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object, std::string &outFile) {
        const std::string dataS3Dir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);
        if (bucket.HasEncryption()) {
            const Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();
            const Database::Entity::KMS::Key kmsKey = kmsDatabase.GetKeyByKeyId(bucket.bucketEncryption.kmsKeyId);
            log_debug << kmsKey.keyId << " " << kmsKey.aes256Key;
            unsigned char *rawKey = Core::Crypto::HexDecode(kmsKey.aes256Key);
            Core::Crypto::Aes256DecryptFile(dataS3Dir + "/" + object.internalName, outFile, rawKey);
        }
    }

    void S3Service::AdjustBucketCounters(const std::string &region, const std::string &bucketName) const {
        Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(region, bucketName);
        bucket.keys = _database.GetBucketObjectCount(region, bucketName);
        bucket.size = _database.GetBucketSize(region, bucketName);
        bucket = _database.UpdateBucket(bucket);
    }

    void S3Service::CheckBucketExistence(const std::string &region, const std::string &name) {

        // Check existence
        if (!Database::S3Database::instance().BucketExists({.region = region, .name = name})) {
            log_warning << "Bucket does not exists, region: " << region << " name: " << name;
            throw Core::NotFoundException("Bucket does not exists, region: " + region + " name: " + name);
        }
    }

    void S3Service::CheckBucketNonExistence(const std::string &region, const std::string &name) {

        // Check existence
        if (Database::S3Database::instance().BucketExists({.region = region, .name = name})) {
            log_warning << "Bucket exists already, region: " << region << " name: " << name;
            throw Core::NotFoundException("Bucket exists already, region: " + region + " name: " + name);
        }
    }

}// namespace AwsMock::Service
