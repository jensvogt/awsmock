//
// Created by vogje01 on 26/08/2022.
//

#ifndef AWSMOCK_CORE_METRIC_DEFINITION_H
#define AWSMOCK_CORE_METRIC_DEFINITION_H

// System counter
#define VIRTUAL_MEMORY "virtual_memory_used"
#define REAL_MEMORY "real_memory_used"
#define TOTAL_THREADS "total_threads"
#define TOTAL_CPU "total_cpu"
#define USER_CPU "user_cpu"
#define SYSTEM_CPU "system_cpu"

// HTTP timer
#define GATEWAY_HTTP_TIMER "gateway_http_timer"
#define GATEWAY_HTTP_COUNTER "gateway_http_counter"

#define MODULE_HTTP_TIMER "manager_http_timer"
#define MODULE_HTTP_COUNTER "manager_http_counter"
#define MODULE_UPDOWN_GAUGE "manager_updown_gauge"

// SNS counter, timer
#define SNS_TOPIC_COUNT "sns_topic_counter"
#define SNS_MESSAGE_COUNT "sns_message_counter"
#define SNS_MESSAGE_BY_TOPIC_COUNT "sns_message_by_topic_counter"
#define SNS_SERVICE_TIMER "sns_service_timer"

// SQS counter, timer
#define SQS_QUEUE_COUNT "sqs_queue_counter"
#define SQS_MESSAGE_COUNT "sqs_message_counter"
#define SQS_MESSAGE_BY_QUEUE_COUNT "sqs_message_by_queue_counter"
#define SQS_SERVICE_TIMER "sqs_service_timer"
#define SQS_MESSAGE_WAIT_TIME "sqs_message_wait_time"

// S3 counter, timer
#define S3_BUCKET_COUNT "s3_bucket_counter"
#define S3_OBJECT_COUNT "s3_object_counter"
#define S3_OBJECT_BY_BUCKET_COUNT "s3_object_by_bucket_counter"
#define S3_SERVICE_TIMER "s3_service_timer"

// Lambda counter, timer
#define LAMBDA_FUNCTION_COUNT "lambda_function_counter"
#define LAMBDA_INVOCATION_TIMER "lambda_invocation_timer"
#define LAMBDA_INVOCATION_COUNT "lambda_invocation_counter"
#define LAMBDA_SERVICE_TIMER "lambda_service_timer"

#define DYNAMODB_TABLE_COUNT "dynamodb_table_counter"
#define DYNAMODB_ITEM_COUNT "dynamodb_item_counter"
#define DYNAMODB_SERVICE_TIMER "dynamodb_service_timer"

#define SECRETSMANAGER_SECRETS_COUNT "secretsmanager_secret_counter"

#define COGNITO_USER_COUNT "cognito_user_counter"
#define COGNITO_USERPOOL_COUNT "cognito_userpool_counter"
#define COGNITO_USER_BY_USERPOOL_COUNT "cognito_user_by_userpool_counter"
#define COGNITO_SERVICE_TIMER "cognito_service_timer"

#define TRANSFER_SERVER_COUNT "transfer_server_counter"
#define TRANSFER_SERVER_UPLOAD_COUNT "transfer_upload_counter"
#define TRANSFER_SERVER_DOWNLOAD_COUNT "transfer_download_counter"
#define TRANSFER_SERVICE_TIMER "transfer_service_timer"
#define TRANSFER_SERVER_FILESIZE_UPLAOD "transfer_upload_filesize"
#define TRANSFER_SERVER_FILESIZE_DOWNLOAD "transfer_download_filesize"

// KMS counter, timer
#define KMS_KEY_COUNT "kms_key_counter"
#define KMS_KEYACCESS_COUNT "kms_key_access_counter"
#define KMS_ENCRYPTION_COUNT "kms_encryption_counter"
#define KMS_DECRYPTION_COUNT "kms_decryption_counter"
#define KMS_SERVICE_TIMER "kms_service_timer"

#endif// AWSMOCK_CORE_METRIC_DEFINITION_H
