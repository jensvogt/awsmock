package de.awsmock.lambda.filecopy.function;

import com.amazonaws.services.lambda.runtime.events.S3Event;
import com.amazonaws.services.lambda.runtime.events.models.s3.S3EventNotification;
import de.awsmock.lambda.filecopy.exception.ProcessException;
import de.awsmock.lambda.filecopy.protokollierung.ProtokollierungService;
import de.awsmock.lambda.filecopy.protokollierung.ProtocolMessage;
import de.awsmock.lambda.filecopy.protokollierung.ProcessingProtocol;
import de.awsmock.lambda.filecopy.service.FileCopyProcessor;
import de.awsmock.lambda.filecopy.service.FileCopyProcessor.FileCopyResult;
import de.awsmock.lambda.filecopy.service.FileCopyValidator;
import de.awsmock.lambda.filecopy.service.FileCopyValidator.FileCopyValidationResult;

import java.time.LocalDateTime;
import java.util.Collection;
import java.util.function.Function;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.slf4j.MDC;
import org.springframework.messaging.Message;
import org.springframework.stereotype.Component;
import software.amazon.awssdk.services.s3.S3Client;
import software.amazon.awssdk.services.s3.model.DeleteObjectRequest;

@Slf4j
@Component
@RequiredArgsConstructor
public class FileCopyFunction implements Function<Message<S3Event>, String> {

    private static final String RETURN_VALUE = "200 OK";

    private final S3Client s3Client;

    private final FileCopyValidator fileCopyValidator;

    private final FileCopyProcessor fileCopyProcessor;

    private final ProtokollierungService protokollierungService;

    @Override
    public String apply(Message<S3Event> s3EventMessage) {
        try {
            LocalDateTime deliveryDate = LocalDateTime.now();

            S3EventNotification.S3Entity s3Entity = getS3ObjectFromEvent(s3EventMessage.getPayload());
            Long s3ObjectSize = getS3ObjectSizeFormEvent(s3EventMessage.getPayload());
            log.info("Incoming request with file: {}/{}", s3Entity.getBucket().getName(), s3Entity.getObject().getUrlDecodedKey());

            FileCopyValidationResult validationResult =
                    fileCopyValidator.validateIncomingFile(s3Entity, s3ObjectSize);

            switch (validationResult.status()) {
                case VALID -> copyFile(deliveryDate, s3Entity);
                case IGNORED -> {
                    // intentionally left empty
                }
                case INVALID -> protocolizeValidationError(s3Entity, deliveryDate, validationResult);
            }

            resetMdcValues();

            return RETURN_VALUE;
        } catch (Exception e) {
            log.error("Unhandled exception thrown in Lambda function", e);
            throw e;
        }
    }

    private void copyFile(
            LocalDateTime deliveryDate, S3EventNotification.S3Entity s3Entity) {

        FileCopyResult fileCopyResult = fileCopyProcessor.copyFile(s3Entity);
        switch (fileCopyResult.status()) {
            case SUCCEED -> {
                log.info("Copied file: {} successful to destination bucket", s3Entity.getObject().getUrlDecodedKey());
                protocolizeSuccess(fileCopyResult.copiedFiles().values(), s3Entity);
            }
            case FAILED -> protocolizeCopyError(s3Entity, deliveryDate);
            case IGNORED -> {
                // intentionally left empty
            }
            case FILE_NOT_FOUND -> handleFileNotFound(s3Entity, deliveryDate);
        }
    }

    private void handleFileNotFound(S3EventNotification.S3Entity s3Entity, LocalDateTime anlieferungsdatum) {
        FileCopyValidator.FileCopyValidationResult fileCopyValidationResult = this.fileCopyValidator.checkIfFileWasProcessedAlready(s3Entity);
        if (fileCopyValidationResult.status().equals(FileCopyValidator.FileCopyValidationStatus.INVALID)) {
            protocolizeValidationError(s3Entity, anlieferungsdatum, fileCopyValidationResult);
        }
    }

    private S3EventNotification.S3Entity getS3ObjectFromEvent(S3Event event) {
        final S3EventNotification.S3EventNotificationRecord firstRecord = event.getRecords().get(0);
        return firstRecord.getS3();
    }

    private Long getS3ObjectSizeFormEvent(S3Event event) {
        final S3EventNotification.S3EventNotificationRecord firstRecord = event.getRecords().get(0);
        final S3EventNotification.S3Entity s3Entity = firstRecord.getS3();

        return s3Entity.getObject().getSizeAsLong();
    }

    private void resetMdcValues() {
        MDC.clear();
    }

    private void protocolizeValidationError(S3EventNotification.S3Entity s3Entity, LocalDateTime deliveryDate, FileCopyValidationResult validationError) {

        protokollierungService.protocolize(ProcessingProtocol.builder()
                .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
                .status(ProcessingProtocol.ProcessingStatus.ERROR)
                .timestamp(deliveryDate)
                .message(validationError.protokollMeldung().toString()).build());

        if (validationError.technicalError()) {
            throwProcessException(s3Entity);
        } else {
            s3Client.deleteObject(DeleteObjectRequest.builder().bucket(s3Entity.getBucket().getName()).key(s3Entity.getObject().getUrlDecodedKey()).build());
        }
    }

    private void protocolizeCopyError(S3EventNotification.S3Entity s3Entity, LocalDateTime deliveryDate) {
        protokollierungService.protocolize(ProcessingProtocol.builder()
                .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
                .status(ProcessingProtocol.ProcessingStatus.ERROR)
                .timestamp(deliveryDate)
                .message(ProtocolMessage.technicalError(s3Entity.getObject().getUrlDecodedKey()).toString()).build());
        throwProcessException(s3Entity);
    }

    private void protocolizeSuccess(Collection<String> copiedFiles, S3EventNotification.S3Entity s3Entity) {
        copiedFiles.forEach(
                copiedFile ->
                        protokollierungService.protocolize(ProcessingProtocol.builder()
                                .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
                                .status(ProcessingProtocol.ProcessingStatus.SUCCESS)
                                .timestamp(LocalDateTime.now())
                                .message("File successfully copied: " + s3Entity.getObject().getUrlDecodedKey()).build()));
    }

    private void throwProcessException(S3EventNotification.S3Entity s3Entity) {
        throw new ProcessException(String.format("Error getting object %s from bucket %s. Object was not copied to destination bucket.",
                s3Entity.getObject().getUrlDecodedKey(), s3Entity.getBucket().getName()));
    }
}
