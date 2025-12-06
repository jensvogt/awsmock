//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/module/model/Infrastructure.h>

namespace AwsMock::Dto::Module {

    document Infrastructure::ToDocument() const {
        document document;

        // S3
        Core::Bson::ToBsonArray(document, "s3-buckets", s3Buckets);
        Core::Bson::ToBsonArray(document, "s3-objects", s3Objects);

        // SQS
        Core::Bson::ToBsonArray(document, "sqs-queues", sqsQueues);
        Core::Bson::ToBsonArray(document, "sqs-messages", sqsMessages);

        // SNS
        Core::Bson::ToBsonArray(document, "sns-topics", snsTopics);
        Core::Bson::ToBsonArray(document, "sns-messages", snsMessages);

        // Lambdas
        Core::Bson::ToBsonArray(document, "lambda-functions", lambdas);

        // Transfer servers
        Core::Bson::ToBsonArray(document, "transfer-servers", transferServers);

        // Cognito
        Core::Bson::ToBsonArray(document, "cognito-user-pools", cognitoUserPools);
        Core::Bson::ToBsonArray(document, "cognito-users", cognitoUsers);

        // DynamoDb
        Core::Bson::ToBsonArray(document, "dynamodb-tables", dynamoDbTables);
        Core::Bson::ToBsonArray(document, "dynamodb-items", dynamoDbItems);

        // Secrets manager
        Core::Bson::ToBsonArray(document, "secretsmanager-secrets", secrets);

        // KMS
        Core::Bson::ToBsonArray(document, "kms-keys", kmsKeys);

        // SSM
        Core::Bson::ToBsonArray(document, "ssm-parameters", ssmParameters);

        // Applications
        Core::Bson::ToBsonArray(document, "applications", applications);

        // API gateway keys
        Core::Bson::ToBsonArray(document, "api-gateway-keys", apiKeys);

        return document;
    }

    void Infrastructure::FromJson(const std::string &jsonString) {

        if (const value documentValue = bsoncxx::from_json(jsonString); documentValue.find("infrastructure") != documentValue.end()) {
            FromDocument(documentValue["infrastructure"].get_document().view());
        }
    }

    void Infrastructure::FromDocument(const view &document) {

        // S3
        Core::Bson::FromBsonArray(document, "s3-buckets", &s3Buckets);
        Core::Bson::FromBsonArray(document, "s3-objects", &s3Objects);

        // SQS
        Core::Bson::FromBsonArray(document, "sqs-queues", &sqsQueues);
        Core::Bson::FromBsonArray(document, "sqs-messages", &sqsMessages);

        // SNS
        Core::Bson::FromBsonArray(document, "sns-topics", &snsTopics);
        Core::Bson::FromBsonArray(document, "sns-messages", &snsMessages);

        // Cognito
        Core::Bson::FromBsonArray(document, "cognito-user-pools", &cognitoUserPools);
        Core::Bson::FromBsonArray(document, "cognito-users", &cognitoUsers);

        // DynamoDB
        Core::Bson::FromBsonArray(document, "dynamodb-tables", &dynamoDbTables);
        Core::Bson::FromBsonArray(document, "dynamodb-items", &dynamoDbItems);

        // Secrets manager
        Core::Bson::FromBsonArray(document, "secretsmanager-secrets", &secrets);

        // Lambdas
        Core::Bson::FromBsonArray(document, "lambda-functions", &lambdas);

        // Transfer servers
        Core::Bson::FromBsonArray(document, "transfer-servers", &transferServers);

        // KMS
        Core::Bson::FromBsonArray(document, "kms-keys", &kmsKeys);

        // SSM
        Core::Bson::FromBsonArray(document, "ssm-parameters", &ssmParameters);

        // Application
        Core::Bson::FromBsonArray(document, "applications", &applications);

        // API gateway keys
        Core::Bson::FromBsonArray(document, "api-gateway-keys", &apiKeys);
    }

}// namespace AwsMock::Dto::Module