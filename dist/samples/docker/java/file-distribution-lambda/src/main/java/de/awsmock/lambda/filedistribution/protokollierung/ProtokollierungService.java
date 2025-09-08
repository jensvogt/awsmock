package de.awsmock.lambda.filedistribution.protokollierung;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.validation.annotation.Validated;
import software.amazon.awssdk.services.sns.SnsClient;
import software.amazon.awssdk.services.sns.model.PublishRequest;

@Slf4j
@Service
@RequiredArgsConstructor
public class ProtokollierungService {

  private final SnsClient snsClient;

  private final ObjectMapper objectMapper;

  @Value("${aws.sns.protocolizing-topic-arn}")
  public String protokollierungTopicArn;

  public void protocolize(ProcessingProtocol processingProtocol) {
    snsClient.publish(createSnsRequest(processingProtocol));
  }

  private PublishRequest createSnsRequest(ProcessingProtocol processingProtocol) {
    try {
      String messageJson = objectMapper.writeValueAsString(processingProtocol);
      log.debug("Publish to protocolizing-topic: {}", processingProtocol);
      return PublishRequest.builder()
          .message(messageJson)
          .topicArn(protokollierungTopicArn)
          .build();
    } catch (JsonProcessingException e) {
      throw new ProtokollierungException(e);
    }
  }

  static class ProtokollierungException extends RuntimeException {
    public ProtokollierungException(Throwable cause) {
      super(cause);
    }
  }
}
