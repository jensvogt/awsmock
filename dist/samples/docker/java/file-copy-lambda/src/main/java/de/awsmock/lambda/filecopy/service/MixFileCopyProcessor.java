package de.awsmock.lambda.filecopy.service;

import com.amazonaws.services.lambda.runtime.events.models.s3.S3EventNotification;

import java.time.LocalDateTime;
import java.util.Collections;
import java.util.Map;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

@Slf4j
@Service
@RequiredArgsConstructor
public class MixFileCopyProcessor {

    @Value("${aws.s3-destination-bucket}")
    private String destinationBucket;

    private final S3FileCopyExecutor s3FileCopyExecutor;

    public Map<String, String> copyMixFile(S3EventNotification.S3Entity s3Entity) {
        log.debug("processMixCopy: bucket name: {}, file name: {}",s3Entity.getBucket().getName(),s3Entity.getObject().getUrlDecodedKey());
        String destinationObjectKey =s3Entity.getObject().getUrlDecodedKey();

        this.s3FileCopyExecutor.copyFile(s3Entity.getBucket().getName(),s3Entity.getObject().getUrlDecodedKey(),destinationBucket,destinationObjectKey);

        return Collections.singletonMap(s3Entity.getObject().getUrlDecodedKey(), destinationObjectKey);
    }
}
