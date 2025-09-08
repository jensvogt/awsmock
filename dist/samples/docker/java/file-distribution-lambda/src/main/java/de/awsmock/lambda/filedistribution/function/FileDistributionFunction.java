package de.awsmock.lambda.filedistribution.function;

import com.amazonaws.services.lambda.runtime.events.S3Event;
import com.amazonaws.services.lambda.runtime.events.models.s3.S3EventNotification;
import com.fasterxml.jackson.databind.ObjectMapper;
import de.awsmock.lambda.filedistribution.dto.FileProcessingDto;
import de.awsmock.lambda.filedistribution.protokollierung.ProcessingProtocol;
import de.awsmock.lambda.filedistribution.protokollierung.ProtokollierungService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FilenameUtils;
import org.slf4j.MDC;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.messaging.Message;
import org.springframework.stereotype.Component;
import software.amazon.awssdk.services.sqs.SqsClient;
import software.amazon.awssdk.services.sqs.model.GetQueueUrlRequest;
import software.amazon.awssdk.services.sqs.model.SendMessageRequest;

import java.util.function.Function;

import static de.awsmock.lambda.filedistribution.protokollierung.ProcessingProtocol.ProcessingStep.FILE_DELIVERY;

@Slf4j
@Component
@RequiredArgsConstructor
public class FileDistributionFunction implements Function<Message<S3Event>, String> {

    @Value("${aws.sqs.image-queue-name}")
    private String imageQueueName;

    @Value("${aws.sqs.xml-queue-name}")
    private String xmlQueueName;

    @Value("${aws.sqs.json-queue-name}")
    private String jsonQueueName;

    private static final String RETURN_VALUE = "200 OK";

    private final SqsClient sqsClient;
    private final ObjectMapper objectMapper;
    private final ProtokollierungService protokollierungService;

    @Override
    public String apply(Message<S3Event> s3EventMessage) {
        try {

            S3EventNotification.S3Entity s3Entity = getS3ObjectFromEvent(s3EventMessage.getPayload());
            Long s3ObjectSize = getS3ObjectSizeFormEvent(s3EventMessage.getPayload());
            log.info("Incoming request with file: {}/{}", s3Entity.getBucket().getName(), s3Entity.getObject().getUrlDecodedKey());

            if (s3ObjectSize == null || s3ObjectSize <= 0) {
                protokollierungService.protocolize(ProcessingProtocol.builder()
                        .processingStep(FILE_DELIVERY)
                        .status(ProcessingProtocol.ProcessingStatus.ERROR)
                        .message("Empty file: " + s3Entity.getObject().getKey()).build());
            }

            String imagePattern = ".*\\.jpg$|jpeg$|png$|gif$|tif$|tiff$";
            if (FilenameUtils.getExtension(s3Entity.getObject().getKey()).toLowerCase().matches(imagePattern)) {
                log.info("File {} has been successfully processed as XML file", s3Entity.getObject().getKey());
                String imageQueueUrl = sqsClient.getQueueUrl(GetQueueUrlRequest.builder().queueName(imageQueueName).build()).queueUrl();
                processFile(s3Entity, "IMAGE", imageQueueUrl);
            } else if (FilenameUtils.getExtension(s3Entity.getObject().getKey()).toLowerCase().contains("xml")) {
                log.info("File {} has been successfully processed as JSON file", s3Entity.getObject().getKey());
                String xmlQueueUrl = sqsClient.getQueueUrl(GetQueueUrlRequest.builder().queueName(xmlQueueName).build()).queueUrl();
                processFile(s3Entity, "XML", xmlQueueUrl);
            } else if (FilenameUtils.getExtension(s3Entity.getObject().getKey()).contains("json")) {
                log.info("File {} has been successfully processed as IMAGE file", s3Entity.getObject().getKey());
                String jsonQueueUrl = sqsClient.getQueueUrl(GetQueueUrlRequest.builder().queueName(jsonQueueName).build()).queueUrl();
                processFile(s3Entity, "JSON", jsonQueueUrl);
            }
            resetMdcValues();

            return RETURN_VALUE;
        } catch (Exception e) {
            log.error("Unhandled exception thrown in Lambda function", e);
            throw e;
        }
    }

    private void processFile(S3EventNotification.S3Entity s3Entity, String type, String queueUrl) {

        try {

            FileProcessingDto fileProcessingDto = FileProcessingDto.builder()
                    .bucket(s3Entity.getBucket().getName())
                    .key(s3Entity.getObject().getKey())
                    .fileType(type)
                    .build();
            String payload = objectMapper.writeValueAsString(fileProcessingDto);
            sqsClient.sendMessage(SendMessageRequest.builder().queueUrl(queueUrl).messageBody(payload).build());
            log.info("Sending message to SQS, queueUrl: {}", queueUrl);

        } catch (Exception e) {
            log.error("Unhandled exception thrown in Lambda function", e);
        }
    }

    private S3EventNotification.S3Entity getS3ObjectFromEvent(S3Event event) {
        final S3EventNotification.S3EventNotificationRecord firstRecord = event.getRecords().getFirst();
        return firstRecord.getS3();
    }

    private Long getS3ObjectSizeFormEvent(S3Event event) {
        final S3EventNotification.S3EventNotificationRecord firstRecord = event.getRecords().getFirst();
        final S3EventNotification.S3Entity s3Entity = firstRecord.getS3();

        return s3Entity.getObject().getSizeAsLong();
    }

    private void resetMdcValues() {
        MDC.clear();
    }
}
