package de.awsmock.lambda.filecopy.protokollierung;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import java.time.LocalDateTime;

import lombok.Builder;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Builder
@ToString
@Getter
@Setter
@JsonIgnoreProperties(ignoreUnknown = true)
public class ProcessingProtocol {

  public enum ProcessingStatus {
    SUCCESS,
    INFO,
    ERROR
  }

  public enum ProcessingStep {
    FILE_COPY,
    FILE_DELIVERY,
  }

  ProcessingStatus status;

  ProcessingStep processingStep;

  String message;

  LocalDateTime timestamp;
}
