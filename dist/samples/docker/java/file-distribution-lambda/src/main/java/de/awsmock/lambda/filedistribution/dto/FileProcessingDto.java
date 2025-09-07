package de.awsmock.lambda.filedistribution.dto;

import lombok.Builder;

import java.time.LocalDateTime;

@Builder
public record FileProcessingDto(String bucket, String key, String fileType, LocalDateTime created) {
}
