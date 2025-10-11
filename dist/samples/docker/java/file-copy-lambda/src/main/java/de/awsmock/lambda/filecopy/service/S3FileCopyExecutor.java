package de.awsmock.lambda.filecopy.service;

import static de.awsmock.lambda.filecopy.service.S3FileCopyExecutor.FileCopyMetadata.MESSAGE_ID;
import static de.awsmock.lambda.filecopy.service.S3FileCopyExecutor.FileCopyMetadata.ORIGINAL_VERSION_ID;

import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.springframework.stereotype.Service;
import software.amazon.awssdk.services.s3.S3Client;
import software.amazon.awssdk.services.s3.model.CopyObjectRequest;
import software.amazon.awssdk.services.s3.model.HeadObjectRequest;
import software.amazon.awssdk.services.s3.model.HeadObjectResponse;
import software.amazon.awssdk.services.s3.model.MetadataDirective;
import software.amazon.awssdk.transfer.s3.model.CopyRequest;

/**
 * Copies large files from one S3 bucket into another with multipart-upload.
 */
@Slf4j
@Service
@RequiredArgsConstructor
public class S3FileCopyExecutor {

    private static final int FILE_SIZE_THRESHOLD_FOR_MULTIPART = 100 * 1024 * 1024; // 100 MB

    private final S3Client s3Client;

    private Map<String, String> addMetadata(String messageId, String versionId, Map<String, String> originalObjectMetadata) {
        Map<String, String> objectMetadata = new HashMap<>(originalObjectMetadata);
        objectMetadata.put(MESSAGE_ID.getKey(), messageId);
        objectMetadata.put(ORIGINAL_VERSION_ID.getKey(), versionId);
        objectMetadata.put(FileCopyMetadata.FILE_ORIGIN.getKey(), "FTP_UPLOAD");
        return objectMetadata;
    }

    public void copyFile(String sourceBucket, String sourceKey, String destBucket, String destKey) {

        HeadObjectResponse objectHeadData = s3Client.headObject(HeadObjectRequest.builder().bucket(sourceBucket).key(sourceKey).build());
        Map<String, String> objectMetadata = objectHeadData.metadata();

        final String messageId = UUID.randomUUID().toString();
        final long fileContentLength = Long.parseLong(objectMetadata.get("content-length") == null ? "0" : objectMetadata.get("content-length"));

        String byteCount = FileUtils.byteCountToDisplaySize(fileContentLength);
        String byteCountThreshold = FileUtils.byteCountToDisplaySize(FILE_SIZE_THRESHOLD_FOR_MULTIPART);
        log.debug("Coping file with messageId={}, destKey={}, fileSize={}", messageId, destKey, byteCount);
        log.debug("Using multipart upload when file is greater than: {}", byteCountThreshold);
        copyObject(messageId, objectHeadData, sourceBucket, sourceKey, destBucket, destKey);
    }

    @SuppressWarnings({"java:S6244"})
    private void copyObject(String messageId, HeadObjectResponse objectHeadData, String sourceBucket, String sourceKey, String destBucket, String destKey) {

        log.debug("Start copying file from {}/{} to {}/{}.", sourceBucket, sourceKey, destBucket, destKey);

        Map<String, String> objectMetadata = addMetadata(messageId, objectHeadData.versionId(), objectHeadData.metadata());

        CopyObjectRequest copyObjectRequest =
                CopyObjectRequest.builder()
                        .sourceBucket(sourceBucket)
                        .sourceKey(sourceKey)
                        .destinationBucket(destBucket)
                        .destinationKey(destKey)
                        .metadataDirective(MetadataDirective.REPLACE)
                        .metadata(objectMetadata)
                        .build();
        s3Client.copyObject(copyObjectRequest);

        log.debug("File copy initialized from {}/{} to {}/{}.", sourceBucket, sourceKey, destBucket, destKey);
    }

    @Getter
    @AllArgsConstructor
    public enum FileCopyMetadata {
        USER_AGENT_ID("user-agent-id"),
        MESSAGE_ID("message-id"),
        ORIGINAL_VERSION_ID("original-version-id"),
        FILE_ORIGIN("file_origin");

        private final String key;
    }
}
