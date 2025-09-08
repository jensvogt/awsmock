package de.awsmock.lambda.filedistribution.exception;

import java.io.Serial;

public class ProcessException extends RuntimeException {

  /** */
  @Serial private static final long serialVersionUID = -6163447071733087905L;

  public ProcessException(String message) {
    super(message);
  }
}
