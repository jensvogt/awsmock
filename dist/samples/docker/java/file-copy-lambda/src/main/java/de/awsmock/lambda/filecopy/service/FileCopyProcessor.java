package de.awsmock.lambda.filecopy.service;

import com.amazonaws.services.lambda.runtime.events.models.s3.S3EventNotification;
import de.awsmock.lambda.filecopy.exception.FilesNotReadyException;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CompletionException;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.apache.http.HttpStatus;
import org.springframework.stereotype.Service;
import software.amazon.awssdk.core.exception.SdkClientException;
import software.amazon.awssdk.services.s3.S3Client;
import software.amazon.awssdk.services.s3.model.DeleteObjectRequest;
import software.amazon.awssdk.services.s3.model.S3Exception;

@Slf4j
@Service
@RequiredArgsConstructor
public class FileCopyProcessor {

    private final S3Client s3Client;
    private final MixFileCopyProcessor mixFileCopyProcessor;
    public FileCopyResult copyFile(S3EventNotification.S3Entity s3Entity) {

        try {
            Map<String, String> copiedFiles = copyFileBasingOnFolderType(s3Entity);
            deleteCopiedFiles(s3Entity.getBucket().getName(), copiedFiles.keySet());
            return new FileCopyResult(FileCopyStatus.SUCCEED, copiedFiles);
        } catch (S3Exception ex) {
            FileCopyStatus status = handleAmazonException(ex, s3Entity);
            return new FileCopyResult(status, new HashMap<>());
        } catch (SdkClientException | CompletionException e) {
            log.debug("Error getting object {} from bucket {}. Make sure they exist and your bucket is in the same region as this function.",
                    s3Entity.getObject().getUrlDecodedKey(), s3Entity.getBucket().getName());
            return new FileCopyResult(FileCopyStatus.FAILED, new HashMap<>());
        } catch (FilesNotReadyException e) {
            return new FileCopyResult(FileCopyStatus.IGNORED, new HashMap<>());
        }
    }

    private Map<String, String> copyFileBasingOnFolderType(S3EventNotification.S3Entity s3Entity) {
        Map<String, String> copyResult = mixFileCopyProcessor.copyMixFile(s3Entity);
        return new HashMap<>(copyResult);
    }

    private void deleteCopiedFiles(String bucket, Set<String> copiedFiles) {

        copiedFiles.forEach(copiedFile -> s3Client.deleteObject(DeleteObjectRequest.builder().bucket(bucket).key(copiedFile).build()));
    }

    private FileCopyStatus handleAmazonException(
            S3Exception exception, S3EventNotification.S3Entity s3Entity) {
        if (exception.awsErrorDetails().sdkHttpResponse().statusCode() == HttpStatus.SC_NOT_FOUND) {
            return FileCopyStatus.FILE_NOT_FOUND;
        } else {
            log.debug("S3 exception, error getting object {} from bucket {}. Make sure they exist and your bucket is in the same region as this function.",
                    s3Entity.getObject().getUrlDecodedKey(), s3Entity.getBucket().getName());
            return FileCopyStatus.FAILED;
        }
    }

    public record FileCopyResult(FileCopyStatus status, Map<String, String> copiedFiles) {
    }

    public enum FileCopyStatus {
        SUCCEED,
        FAILED,
        IGNORED,
        FILE_NOT_FOUND
    }
}
