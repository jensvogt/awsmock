package de.awsmock.lambda.filecopy.service;

import static de.awsmock.lambda.filecopy.service.FileCopyValidator.FileCopyValidationStatus.IGNORED;
import static de.awsmock.lambda.filecopy.service.FileCopyValidator.FileCopyValidationStatus.INVALID;
import static de.awsmock.lambda.filecopy.service.FileCopyValidator.FileCopyValidationStatus.VALID;
import static de.awsmock.lambda.filecopy.service.S3FileCopyExecutor.FileCopyMetadata.ORIGINAL_VERSION_ID;

import com.amazonaws.services.lambda.runtime.events.models.s3.S3EventNotification;
import de.awsmock.lambda.filecopy.protokollierung.ProtocolMessage;
import de.awsmock.lambda.filecopy.protokollierung.ProcessingProtocol;

import java.util.List;
import java.util.Map;
import java.util.Optional;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FilenameUtils;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import software.amazon.awssdk.services.s3.S3Client;
import software.amazon.awssdk.services.s3.model.HeadObjectRequest;
import software.amazon.awssdk.services.s3.model.ObjectVersion;
import software.amazon.awssdk.services.s3.model.S3Exception;

@Slf4j
@Service
@RequiredArgsConstructor
public class FileCopyValidator {

    @Value("${aws.s3.destination.bucket}")
    private String destinationBucket;

    private final S3Client s3Client;

    /**
     * Validates incoming file for correct format, destination and persistence
     *
     * @param s3Entity     id of the s3 object
     * @param s3ObjectSize size of the s3 object
     * @return status of the validation
     */
    public FileCopyValidationResult validateIncomingFile(S3EventNotification.S3Entity s3Entity, Long s3ObjectSize) {

        if (checkIfObjectExists(s3Entity)) {
            return validateIncomingS3Object(s3Entity, s3ObjectSize);
        }
        return new FileCopyValidationResult(VALID, null, false);
    }

    /**
     * If there are uploads for the same object with a time interval of a few milliseconds it might
     * happen, that the latest object is already copied by the previous Event call. Check if object is
     * with same or newer version available in destination bucket
     *
     * @param s3Entity id of source S3 Object (bucket + key)
     * @return status of the validation
     */
    public FileCopyValidationResult checkIfFileWasProcessedAlready(
            S3EventNotification.S3Entity s3Entity) {

        String targetObjectKey = s3Entity.getObject().getKey();

        if (targetObjectKey.isEmpty()) {

            log.debug("Error getting object {} with version {} from bucket {}. No object found in target bucket. New Object was not found anymore and is not copied to " +
                    "destination bucket.", s3Entity.getObject().getUrlDecodedKey(), s3Entity.getObject().getVersionId(), s3Entity.getBucket());
            return new FileCopyValidationResult(INVALID, ProtocolMessage.technicalError(s3Entity.getObject().getUrlDecodedKey()), true);

        }
        if (checkIfTargetFileHasSameVersionThanSourceFile(s3Entity, destinationBucket, targetObjectKey)) {

            log.debug("Copy of object {} from bucket {} skipped. Object with same version exists in target bucket already.", s3Entity.getObject().getUrlDecodedKey(),
                    s3Entity.getBucket().getName());
            return new FileCopyValidationResult(IGNORED, null, false);

        } else {
            log.debug("Error getting object {} with version {} from bucket {}. Destination Bucket contains only files with older version. Probably the newest version " +
                            "needs to be restored. New Object was not found anymore and is not copied to destination bucket.",
                    s3Entity.getObject().getUrlDecodedKey(),s3Entity.getObject().getVersionId(),s3Entity.getBucket().getName());
            return new FileCopyValidationResult(INVALID,ProtocolMessage.technicalError(s3Entity.getObject().getUrlDecodedKey()),true);
        }
    }

    private FileCopyValidationResult validateIncomingS3Object(
            S3EventNotification.S3Entity s3Entity, Long s3ObjectSize) {

        return new FileCopyValidationResult(VALID, null, false);
    }

    private boolean checkIfObjectExists(S3EventNotification.S3Entity s3Entity) {

        try {

            s3Client.headObject(HeadObjectRequest.builder().bucket(s3Entity.getBucket().getName()).key(s3Entity.getObject().getUrlDecodedKey()).build());

        } catch (S3Exception e) {
            return false;
        }
        return true;
    }

    private boolean checkIfS3ObjectHasContent(Long s3ObjectSize) {
        return s3ObjectSize > 0;
    }

    private boolean checkIfTargetFileHasSameVersionThanSourceFile(S3EventNotification.S3Entity sourceS3Entity,String destinationBucket,String targetObjectKey) {
        Map<String, String> destMetadata =
                s3Client.headObject(HeadObjectRequest.builder().bucket(destinationBucket).key(targetObjectKey).build()).metadata();
        return destMetadata.containsKey(ORIGINAL_VERSION_ID.getKey()) && destMetadata.get(ORIGINAL_VERSION_ID.getKey()).equals(sourceS3Entity.getObject().getVersionId());
    }

    public enum FileCopyValidationStatus {
        VALID,
        IGNORED,
        INVALID
    }

    public record FileCopyValidationResult(FileCopyValidationStatus status, ProcessingProtocol protokollMeldung, boolean technicalError) {
    }
}
