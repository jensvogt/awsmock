package de.awsmock.lambda.filecopy.protokollierung;

import static lombok.AccessLevel.PRIVATE;

import lombok.AllArgsConstructor;

@AllArgsConstructor(access = PRIVATE)
public class ProtocolMessage {

  public static ProcessingProtocol success(String message) {
    return ProcessingProtocol.builder()
            .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
        .status(ProcessingProtocol.ProcessingStatus.SUCCESS)
        .message(message)
        .build();
  }

  public static ProcessingProtocol technicalError(String fileName) {
    return ProcessingProtocol.builder()
        .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
        .status(ProcessingProtocol.ProcessingStatus.ERROR)
        .message(String.format("Error during copying of file '%s'.", fileName))
        .build();
  }

  public static ProcessingProtocol emptyFileError(String fileName) {
    return ProcessingProtocol.builder()
            .processingStep(ProcessingProtocol.ProcessingStep.FILE_COPY)
            .status(ProcessingProtocol.ProcessingStatus.ERROR)
        .message(String.format("Empty file: '%s'", fileName))
        .build();
  }
}
