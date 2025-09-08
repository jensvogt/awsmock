package de.awsmock.lambda.filedistribution.protokollierung;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import lombok.Builder;

@Builder
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

}
