//
// Created by vogje01 on 26/08/2022.
//

#ifndef AWSMOCK_CORE_METRIC_DEFINITION_H
#define AWSMOCK_CORE_METRIC_DEFINITION_H

// System counter
#define CPU_USAGE_AWSMOCK "cpu_usage_awsmock"
#define CPU_USAGE_TOTAL "cpu_usage_total"
#define MEMORY_USAGE_AWSMOCK "memory_usage_awsmock"
#define MEMORY_USAGE_TOTAL "memory_usage_total"
#define TOTAL_THREADS "total_threads"

// HTTP timer
#define GATEWAY_HTTP_TIMER "gateway_http_timer"
#define GATEWAY_HTTP_COUNTER "gateway_http_counter"
#define GATEWAY_HTTP_QUEUE_LENGTH "gateway_http_queue_length"

// Modules
#define MODULE_HTTP_TIMER "manager_http_timer"
#define MODULE_HTTP_COUNTER "manager_http_counter"
#define MODULE_UPDOWN_GAUGE "manager_updown_gauge"

// SNS counter, timer
#define SNS_TOPIC_COUNT "sns_topic_counter"
#define SNS_TOPIC_SIZE "sns_topic_size"
#define SNS_MESSAGE_COUNT "sns_message_counter"
#define SNS_MESSAGE_BY_TOPIC_COUNT "sns_message_by_topic_counter"
#define SNS_SERVICE_TIMER "sns_service_timer"
#define SNS_SERVICE_COUNTER "sns_service_counter"

// SQS counter, timer
#define SQS_QUEUE_COUNT "sqs_queue_counter"
#define SQS_QUEUE_SIZE "sqs_queue_size"
#define SQS_MESSAGE_COUNT "sqs_message_counter"
#define SQS_MESSAGE_BY_QUEUE_COUNT "sqs_message_by_queue_counter"
#define SQS_SERVICE_TIMER "sqs_service_timer"
#define SQS_SERVICE_COUNTER "sqs_service_counter"
#define SQS_MESSAGE_WAIT_TIME "sqs_message_wait_time"

// S3 counter, timer
#define S3_BUCKET_COUNT "s3_bucket_counter"
#define S3_OBJECT_COUNT "s3_object_counter"
#define S3_OBJECT_BY_BUCKET_COUNT "s3_object_by_bucket_counter"
#define S3_SERVICE_TIMER "s3_service_timer"
#define S3_SERVICE_COUNTER "s3_service_counter"

// Lambda counter, timer
#define LAMBDA_FUNCTION_COUNT "lambda_function_counter"
#define LAMBDA_SERVICE_TIMER "lambda_service_timer"
#define LAMBDA_SERVICE_COUNTER "lambda_service_counter"
#define LAMBDA_INVOCATION_TIMER "lambda_invocation_timer"
#define LAMBDA_INVOCATION_COUNT "lambda_invocation_counter"

// Dynamodb
#define DYNAMODB_TABLE_COUNT "dynamodb_table_counter"
#define DYNAMODB_ITEM_COUNT "dynamodb_item_counter"
#define DYNAMODB_SERVICE_TIMER "dynamodb_service_timer"
#define DYNAMODB_SERVICE_COUNTER "dynamodb_service_counter"

// Secrets manager
#define SECRETSMANAGER_SECRETS_COUNT "secretsmanager_secret_counter"
#define SECRETSMANAGER_SERVICE_TIMER "secretsmanager_service_timer"
#define SECRETSMANAGER_SERVICE_COUNTER "secretsmanager_service_counter"

// Cognito
#define COGNITO_USER_COUNT "cognito_user_counter"
#define COGNITO_USERPOOL_COUNT "cognito_userpool_counter"
#define COGNITO_USER_BY_USERPOOL_COUNT "cognito_user_by_userpool_counter"
#define COGNITO_SERVICE_TIMER "cognito_service_timer"
#define COGNITO_SERVICE_COUNTER "cognito_service_counter"

// Transfer server
#define TRANSFER_SERVER_COUNT "transfer_server_counter"
#define TRANSFER_SERVER_UPLOAD_COUNT "transfer_upload_counter"
#define TRANSFER_SERVER_DOWNLOAD_COUNT "transfer_download_counter"
#define TRANSFER_SERVICE_TIMER "transfer_service_timer"
#define TRANSFER_SERVICE_COUNTER "transfer_service_counter"
#define TRANSFER_SERVER_FILESIZE_UPLAOD "transfer_upload_filesize"
#define TRANSFER_SERVER_FILESIZE_DOWNLOAD "transfer_download_filesize"

// KMS counter, timer
#define KMS_KEY_COUNT "kms_key_counter"
#define KMS_KEYACCESS_COUNT "kms_key_access_counter"
#define KMS_ENCRYPTION_COUNT "kms_encryption_counter"
#define KMS_DECRYPTION_COUNT "kms_decryption_counter"
#define KMS_SERVICE_TIMER "kms_service_timer"
#define KMS_SERVICE_COUNTER "kms_service_counter"

// Systems manager
#define SSM_PARAMETER_COUNT "ssm_parameter_counter"
#define SSM_SERVICE_TIMER "ssm_service_timer"
#define SSM_SERVICE_COUNTER "ssm_service_counter"

#endif// AWSMOCK_CORE_METRIC_DEFINITION_H
