//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {
    bool S3Service::BucketExists(const std::string &region, const std::string &bucket) const {
        return _database.BucketExists(region, bucket);
    }

    Dto::S3::CreateBucketResponse S3Service::CreateBucket(const Dto::S3::CreateBucketRequest &s3Request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "create_bucket");
        log_trace << "Create bucket request, s3Request: " << s3Request.ToString();

        // Get the region and account ID
        const std::string region = s3Request.region;
        const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");

        // Check existence
        CheckBucketNonExistence(region, s3Request.name);

        try {
            // Update database
            const std::string arn = Core::AwsUtils::CreateS3BucketArn(region, accountId, s3Request.name);
            Database::Entity::S3::Bucket bucket;
            bucket.region = region;
            bucket.name = s3Request.name;
            bucket.owner = s3Request.owner;
            bucket.arn = arn;
            bucket = _database.CreateBucket(bucket);

            Dto::S3::CreateBucketResponse response;
            response.region = region;
            response.arn = Core::CreateArn("s3", region, accountId, s3Request.name);

            log_trace << "S3 create bucket response: " << response.ToXml();
            log_debug << "Bucket created, bucket: " << s3Request.name;
            return response;
        } catch (Core::JsonException &exc) {
            log_error << "S3 create bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    long S3Service::PurgeBucket(const Dto::S3::PurgeBucketRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "purge_bucket");
        log_trace << "Purge bucket request, s3Request: " << request;

        CheckBucketExistence(request.region, request.bucketName);

        try {
            // Get bucket
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucketName);

            // Delete file system objects
            for (const auto &object: _database.ListBucket(request.bucketName)) {
                DeleteObject(object.bucket, object.key, object.internalName);
            }

            // Purge bucket
            const long deleted = _database.PurgeBucket(bucket);
            log_debug << "Bucket purged, region: " << request.region << " bucket: " << request.bucketName << "deleted: " << deleted;
            return deleted;
        } catch (Core::JsonException &exc) {
            log_error << "S3 purge bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void S3Service::UpdateBucket(const Dto::S3::UpdateBucketRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "update_bucket");
        log_trace << "Update bucket request, s3Request: " << request.ToString();

        // Get the region abd account ID
        const std::string region = request.bucket.region;
        auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");

        // Check existence
        CheckBucketExistence(request.region, request.bucket.bucketName);

        try {
            // Mapp DTO to entity
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket.bucketName);
            bucket.defaultMetadata = request.bucket.defaultMetadata;

            // Update database
            _database.UpdateBucket(bucket);
            log_debug << "Bucket updated, bucket: " << request.bucket.bucketName;
        } catch (Core::JsonException &exc) {
            log_error << "S3 create bucket failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::S3::GetMetadataResponse S3Service::GetBucketMetadata(const Dto::S3::GetMetadataRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_bucket_metadata");
        log_trace << "Get bucket metadata request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);
            Dto::S3::GetMetadataResponse response;
            response.region = bucket.region;
            response.bucket = bucket.name;
            response.created = bucket.created;
            response.modified = bucket.modified;

            log_trace << "S3 get bucket metadata response: " + response.ToString();
            log_debug << "Metadata returned, bucket: " << request.bucket << " key: " << request.key;

            return response;
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get object metadata failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetBucketResponse S3Service::GetBucket(const Dto::S3::GetBucketRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_bucket");
        log_trace << "Get bucket request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucketName);

        try {
            const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucketName);
            log_debug << "Bucket returned, bucket: " << request.bucketName;

            return Dto::S3::Mapper::map(request, bucket);
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetEventSourceResponse S3Service::GetEventSource(const Dto::S3::GetEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_event_source");
        log_trace << "Get event source request, s3Request: " << request.ToString();

        // Check existence
        if (!_database.BucketExists(request.eventSourceArn)) {
            log_warning << "Bucket does not exists, arn: " << request.eventSourceArn;
            throw Core::NotFoundException("Bucket does not exists, arn: " + request.eventSourceArn);
        }

        try {
            Database::Entity::S3::Bucket bucket = _database.GetBucketByArn(request.eventSourceArn);
            log_debug << "Bucket returned, bucket: " << bucket.name;

            Dto::S3::GetEventSourceResponse response;
            Database::Entity::S3::LambdaNotification lambdaNotification = bucket.GetLambdaNotification(request.functionArn);
            response.lambdaConfiguration = Dto::S3::Mapper::map(bucket.GetLambdaNotification(request.functionArn));
            return response;
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get event source failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetMetadataResponse S3Service::GetObjectMetadata(const Dto::S3::GetMetadataRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_object_metadata");
        log_trace << "Get metadata request, s3Request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check existence. If it does not exist, log an info message as this method is also used to check the existence of the object
        if (!request.key.empty() && !_database.ObjectExists(request.region, request.bucket, request.key)) {
            log_info << "Object does not exist, region: " << request.region << ", bucket: " << request.bucket << ", key: " << request.key;
            throw Core::NotFoundException("Object does not exist, region: " + request.region + ", bucket: " + request.bucket + ", key: " + request.key);
        }

        try {
            const Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            Dto::S3::GetMetadataResponse response;
            response.bucket = object.bucket;
            response.key = object.key;
            response.md5Sum = object.md5sum;
            response.contentType = object.contentType;
            response.size = object.size;
            response.metadata = object.metadata;
            response.created = object.created;
            response.modified = object.modified;

            log_trace << "S3 get object metadata response: " + response.ToString();
            log_debug << "Metadata returned, bucket: " << request.bucket << " key: " << request.key << " size: " << response.size;

            return response;
        } catch (bsoncxx::exception &ex) {
            log_warning << "S3 get object metadata failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::GetObjectResponse S3Service::GetObject(const Dto::S3::GetObjectRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_object");
        log_trace << "Get object request, s3Request: " << request.ToJson();
        const auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        if (!request.key.empty()) {
            if (!_database.ObjectExists(request.region, request.bucket, request.key)) {
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

            Dto::S3::GetObjectResponse response;
            response.bucket = object.bucket;
            response.key = object.key;
            response.size = object.size;
            response.filename = filename;
            response.contentType = object.contentType;
            response.metadata = object.metadata;
            response.md5sum = object.md5sum;
            response.created = object.created;
            response.modified = object.modified;
            log_trace << "S3 get object response: " << response.ToString();
            log_debug << "Object returned, bucket: " << request.bucket << ", key: " << request.key << ", size: " << response.size;
            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 get object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListAllBucketResponse S3Service::ListAllBuckets() const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_buckets");
        log_trace << "List all buckets request";

        try {
            const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();
            Dto::S3::ListAllBucketResponse listAllBucketResponse;
            for (const auto &b: bucketList) {
                listAllBucketResponse.bucketList.emplace_back(Dto::S3::Mapper::map(b));
            }
            listAllBucketResponse.total = static_cast<long>(bucketList.size());
            log_debug << "Count all buckets, size: " << bucketList.size();
            return listAllBucketResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListBucketCounterResponse S3Service::ListBucketCounters(const Dto::S3::ListBucketCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_bucket_counters");
        log_trace << "List buckets counters request";

        try {
            const Database::Entity::S3::BucketList bucketList = _database.ListBuckets(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));

            Dto::S3::ListBucketCounterResponse listAllBucketResponse;
            listAllBucketResponse.total = _database.BucketCount(request.region, request.prefix);

            for (const auto &bucket: bucketList) {
                Dto::S3::BucketCounter bucketCounter;
                bucketCounter.region = bucket.region;
                bucketCounter.bucketName = bucket.name;
                bucketCounter.bucketArn = bucket.arn;
                bucketCounter.keys = bucket.keys;
                bucketCounter.size = bucket.size;
                bucketCounter.owner = bucket.owner;
                bucketCounter.created = bucket.created;
                bucketCounter.modified = bucket.modified;
                listAllBucketResponse.bucketCounters.emplace_back(bucketCounter);
            }
            log_debug << "List bucket counters, size: " << bucketList.size();

            return listAllBucketResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list bucket counters failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListBucketArnsResponse S3Service::ListBucketArns() const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_bucket_arns");
        log_trace << "List buckets ARNs request";

        try {
            const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

            Dto::S3::ListBucketArnsResponse listBucketArnsResponse;
            for (const auto &bucket: bucketList) {
                listBucketArnsResponse.bucketArns.emplace_back(bucket.arn);
            }
            log_debug << "List bucket ARNs, size: " << bucketList.size();
            return listBucketArnsResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list bucket ARNs failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListBucketResponse S3Service::ListBucket(const Dto::S3::ListBucketRequest &s3Request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_bucket");
        log_trace << "List bucket request: " + s3Request.ToString();

        try {
            const std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket(s3Request.name, s3Request.prefix);

            // TODO: mapper implementation
            Dto::S3::ListBucketResponse listBucketResponse;
            listBucketResponse.name = s3Request.name;
            for (auto &it: objectList) {
                Dto::S3::Owner owner;
                owner.displayName = it.owner;
                owner.id = it.owner;

                Dto::S3::Content content;
                content.key = it.key;
                content.etag = it.md5sum;
                content.size = it.size;
                content.owner = owner;
                content.storageClass = "STANDARD";
                content.modified = system_clock::now();
                listBucketResponse.contents.push_back(content);
            }
            log_debug << "Bucket list returned, count: " << objectList.size();
            return listBucketResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list bucket failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::PutBucketVersioning(const Dto::S3::PutBucketVersioningRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "put_bucket_versioning");
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
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "create_multipart_upload");
        log_trace << "CreateMultipartUpload request, bucket: " + request.bucket << " key: " << request.key << " region: " << request.region << " user: " << request.user;

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        const std::string uploadId = Core::StringUtils::GenerateRandomString(58);

        // Create an upload directory, if not existing
        const std::string uploadDir = GetMultipartUploadDirectory(uploadId);
        Core::DirUtils::EnsureDirectory(uploadDir);

        // Get the bucket
        const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);

        // Create database object
        Database::Entity::S3::Object object;
        object.region = request.region;
        object.bucket = request.bucket;
        object.bucketArn = bucket.arn;
        object.key = request.key;
        object.owner = request.user;
        object.metadata = request.metadata;
        object = _database.CreateOrUpdateObject(object);

        log_debug << "Multipart upload started, bucket: " << request.bucket << " key: " << request.key << " uploadId: " << uploadId;
        Dto::S3::CreateMultipartUploadResult response;
        response.region = request.region;
        response.bucket = request.bucket;
        response.key = request.key;
        response.uploadId = uploadId;
        return response;
    }

    std::string S3Service::UploadPart(std::istream &stream, int part, const std::string &updateId, long length) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "upload_part");
        log_trace << "UploadPart request, part: " << part << ", updateId: " << updateId;

        std::string uploadDir = GetMultipartUploadDirectory(updateId);
        log_trace << "Using uploadDir: " << uploadDir;

        std::string fileName = uploadDir + Core::FileUtils::separator() + updateId + "-" + std::to_string(part);
        std::ofstream ofs(fileName, std::ios::binary);
        long count = Core::FileUtils::StreamCopier(stream, ofs, length);
        ofs.close();
        log_trace << "Part uploaded, part: " << part << ", dir: " << uploadDir << ", count: " << count;

        // Get md5sum as ETag
        std::string eTag = Core::Crypto::GetMd5FromFile(fileName);
        log_debug << "Upload part succeeded, part: " << part << ", filename: " << fileName << ", size: " << count;
        return eTag;
    }

    Dto::S3::UploadPartCopyResponse S3Service::UploadPartCopy(const Dto::S3::UploadPartCopyRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "upload_part_copy");
        log_trace << "UploadPart copy request, part: " << request.partNumber << " updateId: " << request.uploadId;

        const auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

        const std::string sourceFile = s3DataDir + Core::FileUtils::separator() + sourceObject.internalName;
        const std::string uploadDir = GetMultipartUploadDirectory(request.uploadId);
        log_trace << "Using uploadDir: " << uploadDir;

        long start = request.min;
        long length = request.max - request.min + 1;
        const std::string destFile = uploadDir + Core::FileUtils::separator() + request.uploadId + "-" + std::to_string(request.partNumber);

        // Copy part of the file
        long copied = Core::FileUtils::StreamCopier(sourceFile, destFile, start, length);

        // Get md5sum as ETag
        Dto::S3::UploadPartCopyResponse response;
        response.eTag = Core::Crypto::GetMd5FromFile(destFile);
        response.checksumSHA1 = Core::Crypto::GetSha1FromFile(destFile);
        response.checksumSHA256 = Core::Crypto::GetSha256FromFile(destFile);
        response.lastModified = system_clock::now();
        log_debug << "Upload part copy succeeded, part: " << request.partNumber << " filename: " << destFile << " copied: " << copied;

        return response;
    }

    Dto::S3::CompleteMultipartUploadResult S3Service::CompleteMultipartUpload(const Dto::S3::CompleteMultipartUploadRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "complete_multipart_upload");
        log_trace << "CompleteMultipartUpload request, uploadId: " << request.uploadId << " bucket: " << request.bucket << " key: " << request.key << " region: " << request.region;

        // Get database object
        Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
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
            } catch (Core::JsonException &exc) {
                log_error << "Append to binary file failed, error: " << exc.message();
            }

            // Get file size, MD5 sum
            const std::string md5sum = Core::Crypto::GetMd5FromFile(outFile);
            const std::string sha1sum = Core::Crypto::GetSha1FromFile(outFile);
            const std::string sha256sum = Core::Crypto::GetSha256FromFile(outFile);
            log_debug << "Metadata, bucket: " << request.bucket << " key: " << request.key << " md5: " << md5sum;

            // Update database object
            object.size = fileSize;
            object.md5sum = md5sum;
            object.internalName = filename;
            object.contentType = Core::FileUtils::GetContentType(outFile, request.key);
            object = _database.UpdateObject(object);

            // Calculate the hashes asynchronously
            /*if (!request.checksumAlgorithm.empty()) {

                S3HashCreator s3HashCreator;
                const std::vector algorithms = {request.checksumAlgorithm};
                boost::thread t(boost::ref(s3HashCreator), algorithms, object);
                t.detach();
                log_debug << "Checksums, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum << " sha256: " << object.sha256sum;
            }
            */

            // Cleanup
            Core::DirUtils::DeleteDirectory(uploadDir);

            // Check notifications
            CheckNotifications(request.region, request.bucket, request.key, object.size, "ObjectCreated");
            log_debug << "Multipart upload finished, bucket: " << request.bucket << " key: " << request.key;
            Dto::S3::CompleteMultipartUploadResult response;
            response.location = request.region;
            response.bucket = request.bucket;
            response.key = request.key;
            response.etag = md5sum;
            response.checksumSha1 = sha1sum;
            response.checksumSha256 = sha256sum;
            return response;
        }
        Dto::S3::CompleteMultipartUploadResult response;
        response.location = request.region;
        response.bucket = request.bucket;
        response.key = request.key;
        response.checksumSha1 = object.sha1sum;
        response.checksumSha256 = object.sha256sum;
        return response;
    }

    Dto::S3::PutObjectResponse S3Service::PutObject(Dto::S3::PutObjectRequest &request, std::istream &stream) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "put_object");
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
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::PutObject(const std::string &username, const std::string &filename, const std::string &serverId) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "put_object");
        log_trace << "Put object request, username: " << username << ", filename: " << filename;

        // Get environment
        auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        auto userHomeDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        auto transferBucket = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.bucket");

        // Build metadata
        std::map<std::string, std::string> metadata;
        metadata["user-agent"] = "FTPService";
        metadata["user-agent-id"] = username + "@" + serverId;

        // Build request
        Dto::S3::PutObjectRequest request;
        request.region = region;
        request.bucket = transferBucket;
        request.owner = username;
        request.key = Core::StringUtils::StripBeginning(filename, userHomeDir + Core::FileUtils::separator());
        request.contentType = Core::FileUtils::GetContentType(filename, request.key);
        request.contentLength = Core::FileUtils::FileSize(filename);
        request.metadata = metadata;

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            // Get bucket
            std::ifstream ifs(filename, std::ios::binary);
            if (const Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket); bucket.IsVersioned()) {
                SaveVersionedObject(request, bucket, ifs);
            } else {
                SaveUnversionedObject(request, bucket, ifs, request.contentLength);
            }
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::TouchObject(const Dto::S3::TouchObjectRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "touch_object");
        log_trace << "Touch object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check existence
        if (!_database.ObjectExists(request.region, request.bucket, request.key)) {
            log_error << "Bucket does not exist, region: " << request.region + " bucket: " << request.bucket;
            throw Core::NotFoundException("Bucket does not exist");
        }

        try {
            // Get the object
            const Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            // Check notification
            CheckNotifications(object.region, object.bucket, object.key, object.size, "ObjectCreated");
            log_info << "Touch object, bucket: " << request.bucket << ", key: " << request.key;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 touch object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::UpdateObject(const Dto::S3::UpdateObjectRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "update_object");
        log_trace << "Update object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Check existence
        if (!_database.ObjectExists(request.region, request.bucket, request.key)) {
            log_error << "Bucket does not exist, region: " << request.region + " bucket: " << request.bucket;
            throw Core::NotFoundException("Bucket does not exist");
        }

        try {
            // Get the object
            Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

            // Change metadata
            object.metadata = request.metadata;
            object = _database.UpdateObject(object);
            log_info << "Object updated, bucket: " << request.bucket << ", key: " << request.key;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 update object failed, message: " << ex.what() << " key: " << request.key;
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::CopyObjectResponse S3Service::CopyObject(const Dto::S3::CopyObjectRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "copy_object");
        log_trace << "Copy object request: " << request.ToString();

        const auto dataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.data-dir");
        const std::string dataS3Dir = dataDir + Core::FileUtils::separator() + "s3";
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Check existence
        CheckBucketExistence(request.region, request.sourceBucket);

        // Check the existence of the source key
        if (!_database.ObjectExists(request.region, request.sourceBucket, request.sourceKey)) {
            log_error << "Source object does not exist, region: " << request.region + " bucket: " << request.sourceBucket << " key: " << request.sourceKey;
            throw Core::NotFoundException("Source object does not exist");
        }

        Database::Entity::S3::Object targetObject;
        try {
            // Get account ID for ARN
            const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");

            // Check the existence of the target bucket
            if (!_database.BucketExists(request.region, request.targetBucket)) {
                log_error << "Target bucket does not exist, region: " << request.region + " bucket: " << request.targetBucket;
                throw Core::NotFoundException("Target bucket does not exist");
            }

            // Get the source object from the database
            const Database::Entity::S3::Bucket targetBucket = _database.GetBucketByRegionName(request.region, request.targetBucket);
            const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

            // Copy the physical file
            const std::string targetFile = Core::AwsUtils::CreateS3FileName();
            const std::string sourcePath = dataS3Dir + Core::FileUtils::separator() + sourceObject.internalName;
            const std::string targetPath = dataS3Dir + Core::FileUtils::separator() + targetFile;
            Core::FileUtils::CopyTo(sourcePath, targetPath);

            // Update database
            targetObject.region = request.region;
            targetObject.bucket = request.targetBucket;
            targetObject.bucketArn = Core::AwsUtils::CreateS3BucketArn(request.region, accountId, request.targetBucket);
            targetObject.key = request.targetKey;
            targetObject.owner = sourceObject.owner;
            targetObject.size = sourceObject.size;
            targetObject.md5sum = sourceObject.md5sum;
            targetObject.sha1sum = sourceObject.sha1sum;
            targetObject.sha256sum = sourceObject.sha256sum;
            targetObject.contentType = sourceObject.contentType;
            targetObject.localName = sourceObject.localName;
            targetObject.metadata = request.metadata;
            targetObject.internalName = targetFile;

            // Create version ID
            if (targetBucket.IsVersioned()) {
                targetObject.versionId = Core::AwsUtils::CreateS3VersionId();
            }

            // Create the object
            targetObject = _database.CreateObject(targetObject);
            log_debug << "Database updated, bucket: " << targetObject.bucket << " key: " << targetObject.key;

            // Check notification
            CheckNotifications(targetObject.region, targetObject.bucket, targetObject.key, targetObject.size, "ObjectCreated");
            log_debug << "Copy object succeeded, sourceBucket: " << request.sourceBucket << " sourceKey: " << request.sourceKey << " targetBucket: " << request.targetBucket << " targetKey: " << request.targetKey;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 copy object request failed, error: " << ex.what();
            throw Core::ServiceException(ex.what());
        }

        // Prepare response
        Dto::S3::CopyObjectResponse response;
        response.eTag = targetObject.md5sum;
        response.modified = system_clock::now();

        return response;
    }

    Dto::S3::MoveObjectResponse S3Service::MoveObject(const Dto::S3::MoveObjectRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "move_object");
        log_trace << "Move object request: " << request.ToString();

        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Check existence
        CheckBucketExistence(request.region, request.sourceBucket);

        // Check the existence of the source key
        if (!_database.ObjectExists(request.region, request.sourceBucket, request.sourceKey)) {
            log_error << "Source object does not exist, region: " << request.region + " bucket: " << request.sourceBucket << " key: " << request.sourceKey;
            throw Core::NotFoundException("Source object does not exist");
        }

        Database::Entity::S3::Object targetObject;
        try {
            // Check the existence of the target bucket
            if (!_database.BucketExists(request.region, request.targetBucket)) {
                log_error << "Target bucket does not exist, region: " << request.region + " bucket: " << request.targetBucket;
                throw Core::ServiceException("Target bucket does not exist");
            }

            // Get the source object from the database
            const Database::Entity::S3::Bucket targetBucket = _database.GetBucketByRegionName(request.region, request.targetBucket);
            const Database::Entity::S3::Object sourceObject = _database.GetObject(request.region, request.sourceBucket, request.sourceKey);

            // Copy the physical file
            const std::string targetFile = Core::AwsUtils::CreateS3FileName();
            const std::string sourcePath = dataS3Dir + Core::FileUtils::separator() + sourceObject.internalName;
            const std::string targetPath = dataS3Dir + Core::FileUtils::separator() + targetFile;
            Core::FileUtils::CopyTo(sourcePath, targetPath);

            // Update database
            targetObject.region = request.region;
            targetObject.bucket = request.targetBucket;
            targetObject.key = request.targetKey;
            targetObject.owner = sourceObject.owner;
            targetObject.size = sourceObject.size;
            targetObject.md5sum = sourceObject.md5sum;
            targetObject.sha1sum = sourceObject.sha1sum;
            targetObject.sha256sum = sourceObject.sha256sum;
            targetObject.contentType = sourceObject.contentType;
            targetObject.localName = sourceObject.localName;
            targetObject.metadata = request.metadata;
            targetObject.internalName = targetFile;

            // Create version ID
            if (targetBucket.IsVersioned()) {
                targetObject.versionId = Core::AwsUtils::CreateS3VersionId();
            }

            // Create the object
            targetObject = _database.CreateObject(targetObject);
            log_debug << "Database updated, bucket: " << targetObject.bucket << " key: " << targetObject.key;

            // Check notification
            CheckNotifications(targetObject.region, targetObject.bucket, targetObject.key, targetObject.size, "ObjectCreated");
            log_debug << "Move object succeeded, sourceBucket: " << request.sourceBucket << " sourceKey: " << request.sourceKey << " targetBucket: " << request.targetBucket << " targetKey: " << request.targetKey;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 copy object request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }

        Dto::S3::MoveObjectResponse response;
        response.eTag = targetObject.md5sum;
        response.lastModified = Core::DateTimeUtils::ToISO8601(system_clock::now());
        return response;
    }

    void S3Service::DeleteObject(const Dto::S3::DeleteObjectRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "delete_object");
        log_trace << "Delete object request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        if (_database.ObjectExists(request.region, request.bucket, request.key)) {
            try {
                // Get the object from the database
                const Database::Entity::S3::Object object = _database.GetObject(request.region, request.bucket, request.key);

                // Delete from database
                _database.DeleteObject(object);
                log_debug << "Database object deleted, bucket: " + request.bucket + ", key: " << request.key;

                // Delete the file system object
                DeleteObject(object.bucket, object.key, object.internalName);

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
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "delete_objects");
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
            _database.DeleteObjects(request.region, request.bucket, request.keys);
            log_debug << "Database object deleted, count: " << request.keys.size();
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 delete objects failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
        log_debug << "DeleteObjects succeeded, bucket: " << request.bucket;
        return response;
    }

    void S3Service::PutBucketEncryption(const Dto::S3::PutBucketEncryptionRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "put_bucket_encryption");
        log_trace << "Put bucket encryption request, algorithm: " << request.sseAlgorithm;

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(request.region, request.bucket);

            Database::Entity::S3::BucketEncryption bucketEncryption;
            bucketEncryption.sseAlgorithm = request.sseAlgorithm;
            bucketEncryption.kmsKeyId = request.kmsKeyId;

            bucketEntity.bucketEncryption = bucketEncryption;
            bucketEntity = _database.UpdateBucket(bucketEntity);
            log_debug << "PutBucketEncryption succeeded, bucket: " << request.bucket;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 put bucket encryption request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::S3::ListObjectVersionsResponse S3Service::ListObjectVersions(const Dto::S3::ListObjectVersionsRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_option_versions");
        log_trace << "List object versions request: " << request.ToString();

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        // Get bucket
        if (const Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(request.region, request.bucket); !bucketEntity.IsVersioned()) {
            log_error << "Bucket is not versioning";
            throw Core::NotFoundException("Bucket is not versioned");
        }

        Dto::S3::ListObjectVersionsResponse response;
        try {
            const std::vector<Database::Entity::S3::Object> objectList = _database.ListObjectVersions(request.region, request.bucket, request.prefix);
            return Dto::S3::Mapper::map(request, objectList);
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 list object versions request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::DeleteBucket(const Dto::S3::DeleteBucketRequest &request) const {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "delete_bucket");
        log_trace << "Delete bucket request, name: " << request.bucket;

        Database::Entity::S3::Bucket bucket;
        bucket.region = request.region;
        bucket.name = request.bucket;

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

    void S3Service::CheckNotifications(const std::string &region, const std::string &bucket, const std::string &key, long size, const std::string &event) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "check_notifications");
        log_debug << "Check notifications, region: " << region << " bucket: " << bucket << " event: " << event;

        Database::Entity::S3::Bucket bucketEntity = _database.GetBucketByRegionName(region, bucket);

        // Create S3 bucket and object
        Dto::S3::Object s3Object;
        s3Object.key = key;
        s3Object.size = size;
        s3Object.etag = Core::StringUtils::CreateRandomUuid();

        Dto::S3::Bucket s3Bucket;
        s3Bucket.bucketName = bucketEntity.name;

        if (bucketEntity.HasQueueNotificationEvent(event)) {
            if (Database::Entity::S3::QueueNotification notification = bucketEntity.GetQueueNotification(event); notification.CheckFilter(key)) {
                // Create the event record
                Dto::S3::NotificationBucket notificationBucket;
                notificationBucket.name = s3Bucket.bucketName;
                notificationBucket.arn = s3Bucket.arn;
                notificationBucket.ownerIdentity.displayName = s3Bucket.owner;
                Dto::S3::S3 s3;
                s3.configurationId = notification.id;
                s3.bucket = notificationBucket;
                s3.object = s3Object;

                Dto::S3::Record record;
                record.region = region;
                record.eventName = event;
                record.s3 = s3;
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
                notificationBucket.ownerIdentity.displayName = s3Bucket.owner;
                Dto::S3::S3 s3;
                s3.configurationId = notification.id;
                s3.bucket = notificationBucket;
                s3.object = s3Object;

                Dto::S3::Record record;
                record.region = region;
                record.eventName = event;
                record.s3 = s3;

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
                notificationBucket.ownerIdentity.displayName = s3Bucket.owner;
                Dto::S3::S3 s3;
                s3.configurationId = notification.id;
                s3.bucket = notificationBucket;
                s3.object = s3Object;

                Dto::S3::Record record;
                record.region = region;
                record.eventName = event;
                record.s3 = s3;
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
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "put_bucket_notification");

        // Check existence
        CheckBucketExistence(request.region, request.bucket);

        try {
            Dto::S3::PutBucketNotificationConfigurationResponse response;

            // Get bucket
            Database::Entity::S3::Bucket bucket = _database.GetBucketByRegionName(request.region, request.bucket);
            log_debug << "Bucket received, region:" << bucket.region << " bucket: " << bucket.name;

            // Add notification configurations
            if (!request.queueConfigurations.empty()) {
                PutQueueNotificationConfigurations(bucket, request.queueConfigurations);
            }
            if (!request.topicConfigurations.empty()) {
                PutTopicNotificationConfigurations(bucket, request.topicConfigurations);
            }
            if (!request.lambdaConfigurations.empty()) {
                PutLambdaNotificationConfigurations(bucket, request.lambdaConfigurations);
            }

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
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "list_object_counters");
        log_trace << "List objects counters request";

        try {
            std::vector<Database::SortColumn> sortColumns;
            for (const auto &sc: s3Request.sortColumns) {
                Database::SortColumn sortColumn;
                sortColumn.column = sc.column;
                sortColumn.sortDirection = sc.sortDirection;
                sortColumns.push_back(sortColumn);
            }
            const std::vector<Database::Entity::S3::Object> objectList = _database.ListObjects(s3Request.region, s3Request.prefix, s3Request.bucket, s3Request.pageSize, s3Request.pageIndex, sortColumns);

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
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "get_object_counters");
        log_trace << "Get objects counters request";

        // Check existence
        if (!_database.ObjectExists(request.oid)) {
            log_error << "Object does not exists, name: " << request.oid;
            throw Core::NotFoundException("Object does not exists, id: " + request.oid);
        }

        try {
            const Database::Entity::S3::Object object = _database.GetObjectById(request.oid);

            Dto::S3::GetObjectCounterResponse getObjectCounterResponse;
            auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
            std::string body = Core::FileUtils::ReadFile(dataS3Dir + Core::FileUtils::separator() + object.internalName);

            Dto::S3::ObjectCounter objectCounter;
            objectCounter.oid = object.oid;
            objectCounter.region = object.region;
            objectCounter.bucketName = object.bucket;
            objectCounter.key = object.key;
            objectCounter.owner = object.owner;
            objectCounter.size = object.size;
            objectCounter.contentType = object.contentType;
            objectCounter.internalName = object.internalName;
            objectCounter.body = Core::Crypto::Base64Encode(body);
            objectCounter.created = object.created;
            objectCounter.modified = object.modified;
            objectCounter.metadata = object.metadata;
            getObjectCounterResponse.objectCounter = objectCounter;
            return getObjectCounterResponse;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::UploadObjectCounter(const Dto::S3::UploadObjectCounterRequest &request) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "upload_object_counters");
        log_trace << "Upload objects counters request";

        // Check existence
        if (!_database.BucketExists(request.bucketArn)) {
            log_error << "Bucket does not exists, bucketArn: " << request.bucketArn;
            throw Core::NotFoundException("Bucket does not exists, bucketArn: " + request.bucketArn);
        }

        try {
            const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
            auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
            Core::DirUtils::EnsureDirectory(dataS3Dir);

            // Write the file
            std::string fileName = Core::AwsUtils::CreateS3FileName();
            std::string filePath = dataS3Dir + Core::FileUtils::separator() + fileName;

            // Write the file in chunks
            std::string data = Core::Crypto::Base64Decode(request.content);
            std::ofstream ofs(filePath, std::ios::binary | std::ios::trunc);
            ofs << data;
            ofs.close();
            log_debug << "File copied, count: " << data.size();

            // Get content type
            std::string contentType = SanitizeContentType(request.contentType, filePath, request.objectKey);

            // Create entity
            Database::Entity::S3::Object object;
            object.region = request.region;
            object.bucket = request.bucketName;
            object.bucketArn = Core::AwsUtils::CreateS3BucketArn(request.region, accountId, request.bucketName);
            object.key = request.objectKey;
            object.owner = request.user;
            object.size = static_cast<long>(data.size());
            object.contentType = contentType;
            object.metadata = request.metadata;
            object.internalName = fileName;

            // Meta data
            object.md5sum = Core::Crypto::GetMd5FromFile(filePath);

            // Update database
            object = _database.CreateOrUpdateObject(object);
            log_debug << "Database updated, bucket: " << object.bucket << " key: " << object.key;

            // Check encryption
            //CheckEncryption(request.bucketName, object);
            log_debug << "Put object succeeded, bucket: " << request.bucketName << " key: " << request.objectKey;

            // Check notification
            CheckNotifications(request.region, request.bucketName, request.objectKey, object.size, "ObjectCreated");
            log_debug << "Notifications send, bucket: " << request.bucketName << " key: " << request.objectKey;
        } catch (bsoncxx::exception &ex) {
            log_error << "S3 Create Object failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void S3Service::DeleteObject(const std::string &bucket, const std::string &key, const std::string &internalName) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "delete_object");

        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        const auto transferDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        const auto transferBucket = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.bucket");

        if (!internalName.empty()) {
            std::string filename = dataS3Dir + Core::FileUtils::separator() + internalName;
            Core::FileUtils::RemoveFile(filename);
            log_debug << "File system object deleted, filename: " << filename;

            if (bucket == transferBucket) {
                filename = key;
                filename = transferDir + Core::FileUtils::separator() + Core::FileUtils::SetFileSeparator(filename);
                Core::FileUtils::RemoveFile(filename);
                log_debug << "Transfer file system object deleted, filename: " << filename;
            }
        }
    }

    void S3Service::DeleteBucket(const std::string &bucket) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER, S3_SERVICE_COUNTER, "action", "delete_bucket");

        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // TODO: List object in bucket und delete each object individually
        if (const std::string bucketDir = dataS3Dir + Core::FileUtils::separator() + bucket; Core::DirUtils::DirectoryExists(bucketDir)) {
            Core::DirUtils::DeleteDirectory(bucketDir);
            log_debug << "Bucket directory deleted, bucketDir: " + bucketDir;
        }
    }

    std::string S3Service::GetMultipartUploadDirectory(const std::string &uploadId) {
        const auto tempDir = Core::Configuration::instance().GetValue<std::string>("awsmock.temp-dir");
        Core::DirUtils::EnsureDirectory(tempDir);
        return tempDir + Core::FileUtils::separator() + uploadId;
    }

    void S3Service::SendQueueNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::QueueNotification &queueNotification) {
        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");

        // Get queue URL
        const std::string queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(queueNotification.queueArn);

        SQSService _sqsService(_ioc);
        Dto::SQS::SendMessageRequest request;
        request.region = region;
        request.queueUrl = queueUrl;
        request.body = eventNotification.ToJson();
        const Dto::SQS::SendMessageResponse response = _sqsService.SendMessage(request);
        log_debug << "SQS message request send, messageId: " << response.messageId;
    }

    void S3Service::SendTopicNotificationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::TopicNotification &topicNotification) {
        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");

        SNSService _snsService(_ioc);
        Dto::SNS::PublishRequest request;
        request.region = region;
        request.targetArn = topicNotification.topicArn;
        request.message = eventNotification.ToJson();
        Dto::SNS::PublishResponse response = _snsService.Publish(request);
        log_debug << "SNS message request send, messageId: " << response.messageId;
    }

    void S3Service::SendLambdaInvocationRequest(const Dto::S3::EventNotification &eventNotification, const Database::Entity::S3::LambdaNotification &lambdaNotification) const {
        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");

        const std::vector<std::string> parts = Core::StringUtils::Split(lambdaNotification.lambdaArn, ":");
        const std::string &functionName = parts[6];
        log_debug << "Invocation request function name: " << functionName;

        std::string payload = eventNotification.ToJson();
        Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, functionName, payload, Dto::Lambda::LambdaInvocationType::EVENT);
        log_debug << "Lambda invocation send";
    }

    Dto::S3::PutObjectResponse S3Service::SaveUnversionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream, long size) {
        const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Write the file
        std::string fileName = Core::AwsUtils::CreateS3FileName();
        std::string filePath = dataS3Dir + Core::FileUtils::separator() + fileName;

        // Write the file in chunks
        std::ofstream ofs(filePath, std::ios::binary | std::ios::trunc);
        long count = Core::FileUtils::StreamCopier(stream, ofs, request.contentLength);
        ofs.close();
        log_debug << "File copied, count: " << count;

        // Check file encoding
        if (Core::FileUtils::IsBase64(filePath)) {
            log_debug << "File is base64 encoded, file: " << filePath;
            Core::FileUtils::Base64DecodeFile(filePath);
        }

        // Get content type
        std::string contentType = SanitizeContentType(request.contentType, filePath, request.key);

        // Create entity
        Database::Entity::S3::Object object;
        object.region = request.region;
        object.bucket = request.bucket;
        object.bucketArn = Core::AwsUtils::CreateS3BucketArn(request.region, accountId, request.bucket);
        object.key = request.key;
        object.owner = request.owner;
        object.size = size;
        object.contentType = contentType;
        object.metadata = request.metadata;
        object.internalName = fileName;

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

        Dto::S3::PutObjectResponse response;
        response.bucket = request.bucket;
        response.key = request.key;
        response.etag = object.md5sum;
        response.md5Sum = object.md5sum;
        response.contentLength = size;
        response.contentType = contentType;
        response.sha1Sum = object.sha1sum;
        response.sha256sum = object.sha256sum;
        response.metadata = request.metadata;

        return response;
    }

    Dto::S3::PutObjectResponse S3Service::SaveVersionedObject(Dto::S3::PutObjectRequest &request, const Database::Entity::S3::Bucket &bucket, std::istream &stream) {
        // S3 data directory
        const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);

        // Write the file
        std::string fileName = Core::AwsUtils::CreateS3FileName();
        std::string filePath = dataS3Dir + Core::FileUtils::separator() + fileName;

        std::ofstream ofs(filePath, std::ios::binary | std::ios::trunc);
        long count = Core::FileUtils::StreamCopier(stream, ofs, request.contentLength);
        ofs.close();
        log_debug << "File received, filePath: " << filePath << " size: " << count;

        Database::Entity::S3::Object object;

        // Check existence by
        std::string md5sum = Core::Crypto::HexEncode(Core::Crypto::Base64Decode(request.md5Sum));
        if (Database::Entity::S3::Object existingObject = _database.GetObjectMd5(request.region, request.bucket, request.key, md5sum); existingObject.oid.empty()) {
            // Version ID
            std::string versionId = Core::AwsUtils::CreateS3VersionId();

            // Create a new version of the object
            object.region = request.region;
            object.bucket = request.bucket;
            object.bucketArn = Core::AwsUtils::CreateS3BucketArn(request.region, accountId, request.bucket);
            object.key = request.key;
            object.owner = request.owner;
            object.size = count;
            object.contentType = request.contentType;
            object.metadata = request.metadata;
            object.internalName = fileName;
            object.versionId = versionId;

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

            // Create a new version in the database
            object = _database.CreateObject(object);
            log_debug << "Database updated, bucket: " << object.bucket << " key: " << object.key;

            // Check encryption
            CheckEncryption(bucket, object);
            log_debug << "Put object succeeded, bucket: " << request.bucket << " key: " << request.key;

            // Check notification
            CheckNotifications(request.region, request.bucket, request.key, object.size, "ObjectCreated");
            log_debug << "Put object succeeded, bucket: " << request.bucket << " key: " << request.key;
        } else {
            // Delete the local file
            Core::FileUtils::RemoveFile(filePath);
        }

        Dto::S3::PutObjectResponse response;
        response.bucket = request.bucket;
        response.key = request.key;
        response.etag = object.md5sum;
        response.md5Sum = object.md5sum;
        response.contentLength = object.size;
        response.contentType = object.contentType;
        response.sha1Sum = object.sha1sum;
        response.sha256sum = object.sha256sum;
        response.metadata = request.metadata;

        return response;
    }

    void S3Service::PutQueueNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::QueueConfiguration> &queueConfigurations) {
        for (const auto &queueConfiguration: queueConfigurations) {
            // Check existence
            if (!queueConfiguration.id.empty() && bucket.HasQueueNotificationId(queueConfiguration.id)) {
                log_debug << "Queue notification configuration exists already, id: " << queueConfiguration.id;
                break;
            }

            // General attributes
            const std::string attrId = queueConfiguration.id.empty() ? Core::StringUtils::CreateRandomUuid() : queueConfiguration.id;
            Database::Entity::S3::QueueNotification queueNotification;
            queueNotification.id = attrId;
            queueNotification.queueArn = queueConfiguration.queueArn;

            // Get events
            for (const auto &event: queueConfiguration.events) {
                queueNotification.events.emplace_back(EventTypeToString(event));
            }

            // Get filter rules
            for (const auto &filterRule: queueConfiguration.filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity;
                filterRuleEntity.name = Dto::S3::NameTypeToString(filterRule.name);
                filterRuleEntity.value = filterRule.filterValue;
                queueNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.queueNotifications.emplace_back(queueNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::PutTopicNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::TopicConfiguration> &topicConfigurations) {
        for (const auto &topicConfiguration: topicConfigurations) {
            // Check existence
            if (!topicConfiguration.id.empty() && bucket.HasTopicNotificationId(topicConfiguration.id)) {
                log_debug << "Topic notification configuration exists already, id: " << topicConfiguration.id;
                break;
            }

            // General attributes
            const std::string attrId = topicConfiguration.id.empty() ? Core::StringUtils::CreateRandomUuid() : topicConfiguration.id;
            Database::Entity::S3::TopicNotification topicNotification;
            topicNotification.id = attrId;
            topicNotification.topicArn = topicConfiguration.topicArn;

            // Get events
            for (const auto &event: topicConfiguration.events) {
                topicNotification.events.emplace_back(Dto::S3::EventTypeToString(event));
            }

            // Get filter rules
            for (const auto &filterRule: topicConfiguration.filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity;
                filterRuleEntity.name = Dto::S3::NameTypeToString(filterRule.name);
                filterRuleEntity.value = filterRule.filterValue;
                topicNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.topicNotifications.emplace_back(topicNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::PutLambdaNotificationConfigurations(Database::Entity::S3::Bucket &bucket, const std::vector<Dto::S3::LambdaConfiguration> &lambdaConfigurations) {
        for (const auto &lambdaConfiguration: lambdaConfigurations) {
            // Check existence
            if (!lambdaConfiguration.id.empty() && bucket.HasLambdaNotificationId(lambdaConfiguration.id)) {
                log_debug << "Lambda notification configuration exists already, id: " << lambdaConfiguration.id;
                break;
            }

            // General attributes
            const std::string attrId = lambdaConfiguration.id.empty() ? Core::StringUtils::CreateRandomUuid() : lambdaConfiguration.id;
            Database::Entity::S3::LambdaNotification lambdaNotification;
            lambdaNotification.id = attrId;
            lambdaNotification.lambdaArn = lambdaConfiguration.lambdaArn;

            // Get events
            for (const auto &event: lambdaConfiguration.events) {
                lambdaNotification.events.emplace_back(Dto::S3::EventTypeToString(event));
            }

            // Get filter rules
            for (const auto &filterRule: lambdaConfiguration.filterRules) {
                Database::Entity::S3::FilterRule filterRuleEntity;
                filterRuleEntity.name = Dto::S3::NameTypeToString(filterRule.name);
                filterRuleEntity.value = filterRule.filterValue;
                lambdaNotification.filterRules.emplace_back(filterRuleEntity);
            }
            bucket.lambdaNotifications.emplace_back(lambdaNotification);
            log_debug << "Added queue notification configurations, count: " << bucket.queueNotifications.size();
        }
    }

    void S3Service::CheckEncryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object) {
        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);
        if (bucket.HasEncryption()) {
            const Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();
            const Database::Entity::KMS::Key kmsKey = kmsDatabase.GetKeyByKeyId(bucket.bucketEncryption.kmsKeyId);
            log_debug << kmsKey.keyId << " " << kmsKey.aes256Key;
            const auto rawKey = static_cast<unsigned char *>(malloc(kmsKey.aes256Key.length() * 2));
            Core::Crypto::HexDecode(kmsKey.aes256Key, rawKey);
            Core::Crypto::Aes256EncryptFile(dataS3Dir + "/" + object.internalName, rawKey);
            free(rawKey);
        }
    }

    void S3Service::CheckDecryption(const Database::Entity::S3::Bucket &bucket, const Database::Entity::S3::Object &object, std::string &outFile) {
        const auto dataS3Dir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        Core::DirUtils::EnsureDirectory(dataS3Dir);
        if (bucket.HasEncryption()) {
            const Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();
            const Database::Entity::KMS::Key kmsKey = kmsDatabase.GetKeyByKeyId(bucket.bucketEncryption.kmsKeyId);
            log_debug << kmsKey.keyId << " " << kmsKey.aes256Key;
            const auto rawKey = static_cast<unsigned char *>(malloc(kmsKey.aes256Key.length() * 2));
            Core::Crypto::HexDecode(kmsKey.aes256Key, rawKey);
            Core::Crypto::Aes256DecryptFile(dataS3Dir + "/" + object.internalName, outFile, rawKey);
            free(rawKey);
        }
    }

    void S3Service::CheckBucketExistence(const std::string &region, const std::string &name) {
        // Check existence
        if (!Database::S3Database::instance().BucketExists(region, name)) {
            log_warning << "Bucket does not exists, region: " << region << " name: " << name;
            throw Core::NotFoundException("Bucket does not exists, region: " + region + " name: " + name);
        }
    }

    void S3Service::CheckBucketNonExistence(const std::string &region, const std::string &name) {
        // Check existence
        if (Database::S3Database::instance().BucketExists(region, name)) {
            log_warning << "Bucket exists already, region: " << region << " name: " << name;
            throw Core::NotFoundException("Bucket exists already, region: " + region + ", name: " + name);
        }
    }

    std::string S3Service::SanitizeContentType(const std::string &contentType, const std::string &filePath, const std::string &s3Key) {
        std::string sContentType = contentType;
        if (contentType.empty() || contentType == "application/octet-stream" || contentType == "binary/octet-stream") {
            sContentType = Core::FileUtils::GetContentType(filePath, s3Key);
        }
        return sContentType;
    }
} // namespace AwsMock::Service
