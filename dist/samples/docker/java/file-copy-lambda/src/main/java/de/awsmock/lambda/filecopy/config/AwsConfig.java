package de.awsmock.lambda.filecopy.config;

import org.springframework.beans.factory.annotation.Value;
import static software.amazon.awssdk.transfer.s3.SizeConstant.MB;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import java.net.URI;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;
import software.amazon.awssdk.auth.credentials.AwsCredentialsProvider;
import software.amazon.awssdk.auth.credentials.ProfileCredentialsProvider;
import software.amazon.awssdk.awscore.AwsClient;
import software.amazon.awssdk.http.apache.ApacheHttpClient;
import software.amazon.awssdk.regions.Region;
import software.amazon.awssdk.services.s3.S3AsyncClient;
import software.amazon.awssdk.services.s3.S3Client;
import software.amazon.awssdk.services.s3.S3CrtAsyncClientBuilder;
import software.amazon.awssdk.services.sns.SnsClient;
import software.amazon.awssdk.transfer.s3.S3TransferManager;

@Slf4j
@Configuration
public class AwsConfig {

  @Value("${aws.localstack-endpoint}")
  String localstackEndpoint;

  @Bean
  public ObjectMapper objectMapper() {
    return JsonMapper.builder()
        .configure(MapperFeature.DEFAULT_VIEW_INCLUSION, false)
        .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false)
        .build()
        .registerModule(new JavaTimeModule());
  }

  @Bean
  public S3Client s3Client() {
    return buildClient(
        S3Client.builder()
            .forcePathStyle(true));
  }

  @Bean
  public S3AsyncClient s3AsyncClient() {
    S3CrtAsyncClientBuilder s3CrtBuilder = S3AsyncClient.crtBuilder();
    s3CrtBuilder.region(Region.EU_CENTRAL_1)
            .endpointOverride(URI.create(localstackEndpoint))
            .credentialsProvider(ProfileCredentialsProvider.create());
    return s3CrtBuilder
        .targetThroughputInGbps(20.0)
        .minimumPartSizeInBytes(8 * MB)
        .forcePathStyle(true)
        .build();
  }

  @Bean
  public S3TransferManager s3TransferManager(S3AsyncClient s3AsyncClient) {
    return S3TransferManager.builder().s3Client(s3AsyncClient).build();
  }

  @Bean
  public SnsClient snsClient() {
    return buildClient(SnsClient.builder());
  }

  private <T extends software.amazon.awssdk.awscore.client.builder.AwsClientBuilder<T, S>,S extends AwsClient>S buildClient(T builder) {
    builder.region(Region.EU_CENTRAL_1)
            .endpointOverride(URI.create(localstackEndpoint))
            .credentialsProvider(ProfileCredentialsProvider.create());
    return builder.build();
  }
}
