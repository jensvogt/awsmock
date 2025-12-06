//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/core/AwsUtils.h>

namespace AwsMock::Core {
    std::string AwsUtils::CreateS3BucketArn(const std::string &region, const std::string &accountId, const std::string &bucket) {
        return CreateArn("s3", region, accountId, bucket);
    }

    std::string AwsUtils::CreateS3ObjectArn(const std::string &region, const std::string &accountId, const std::string &bucket, const std::string &key) {
        return CreateArn("s3", region, accountId, bucket + "/" + key);
    }

    std::string AwsUtils::CreateLambdaArn(const std::string &region, const std::string &accountId, const std::string &function) {
        return CreateArn("lambda", region, accountId, "function:" + function);
    }

    std::string AwsUtils::CreateApiKeyId() {
        return StringUtils::GenerateRandomAlphanumericString(12);
    }

    std::string AwsUtils::CreateRestApiId() {
        return StringUtils::GenerateRandomAlphanumericString(12);
    }

    std::string AwsUtils::CreateApiGatewayApiKey() {
        return StringUtils::GenerateRandomString(42);
    }

    std::string AwsUtils::ConvertSQSQueueArnToUrl(const std::string &queueArn) {
        std::string endpoint = GetEndpoint();
        std::vector<std::string> parts = StringUtils::Split(queueArn, ":");
        if (parts.size() < 6) {
            log_error << "Could not convert SQS arn to url, arn: " << queueArn;
            return {};
        }

        const std::string accountId = parts[4];
        const std::string queueName = parts[5];
        parts.clear();

        const auto region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const auto port = Configuration::instance().GetValue<std::string>("awsmock.gateway.http.port");
        const auto hostname = SystemUtils::GetHostName();

        return "http://sqs." + region + "." + hostname + ":" + port + "/" + accountId + "/" + queueName;
    }

    std::string AwsUtils::ConvertSQSQueueUrlToArn(const std::string &region, const std::string &queueUrl) {
        const auto queueName = queueUrl.substr(queueUrl.rfind('/') + 1);
        const auto accountId = Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        log_trace << "Region: " << region << " accountId: " << accountId;
        return CreateArn("sqs", region, accountId, queueName);
    }

    std::string AwsUtils::ConvertSQSQueueArnToName(const std::string &queueArn) {
        std::vector<std::string> parts = StringUtils::Split(queueArn, ":");
        if (parts.size() < 6) {
            log_error << "Could not convert SQS arn to name, arn: " << queueArn;
            return {};
        }
        return parts[5];
    }

    std::string AwsUtils::ConvertSQSQueueUrlToName(const std::string &queueUrl) {
        return queueUrl.substr(queueUrl.find_last_of("/") + 1);
    }

    std::string AwsUtils::CreateSQSQueueArn(const std::string &region, const std::string &accountId, const std::string &queueName) {
        return CreateArn("sqs", region, accountId, queueName);
    }

    std::string AwsUtils::CreateSQSSenderId() {
        return StringUtils::GenerateRandomString(21) + ":none@example.com";
    }

    std::string AwsUtils::CreateSNSTopicArn(const std::string &region, const std::string &accountId, const std::string &topicName) {
        return CreateArn("sns", region, accountId, topicName);
    }

    std::string AwsUtils::CreateSNSSubscriptionArn(const std::string &region, const std::string &accountId, const std::string &topicName) {
        return CreateArn("sns", region, accountId, topicName + ":" + StringUtils::CreateRandomUuid());
    }

    std::string AwsUtils::CreateSSMParameterArn(const std::string &region, const std::string &accountId, const std::string &name) {
        return CreateArn("ssm", region, accountId, name);
    }

    std::string AwsUtils::CreateTransferArn(const std::string &region, const std::string &accountId, const std::string &serverId) {
        return CreateArn("transfer", region, accountId, "manager/" + serverId);
    }

    std::string AwsUtils::CreateTransferUserArn(const std::string &region, const std::string &accountId, const std::string &serverId, const std::string &userName) {
        return CreateArn("transfer", region, accountId, serverId + "/" + userName);
    }

    std::string AwsUtils::CreateSecretArn(const std::string &region, const std::string &accountId, const std::string &secretId) {
        return CreateArn("secretsmanager", region, accountId, "secret:" + secretId);
    }

    std::string AwsUtils::CreateKMSKeyArn(const std::string &region, const std::string &accountId, const std::string &kmsId) {
        return CreateArn("kms", region, accountId, "key/" + kmsId);
    }

    std::string AwsUtils::CreateDynamoDbTableArn(const std::string &accountId, const std::string &tableName) {
        return CreateArn("dynamodb", "ddblocal", accountId, "table/" + tableName);
    }

    std::string AwsUtils::CreateCognitoUserPoolId(const std::string &region) {
        return region + "_" + StringUtils::GenerateRandomString(9);
    }

    std::string AwsUtils::CreateCognitoUserPoolArn(const std::string &region, const std::string &accountId, const std::string &userPoolId) {
        return CreateArn("cognito-idp", region, accountId, userPoolId);
    }

    std::string AwsUtils::CreateRequestId() {
        std::stringstream stringstream;
        stringstream << boost::uuids::random_generator()();
        return stringstream.str();
    }

    std::string AwsUtils::CreateMessageId() {
        std::stringstream stringstream;
        stringstream << boost::uuids::random_generator()();
        return stringstream.str();
    }

    std::string AwsUtils::GetS3BucketName(const http::request<http::dynamic_body> &request) {
        if (IsS3HostStyle(request)) {
            return GetS3HostStyleBucket(request);
        }
        return GetS3PathStyleBucket(request);
    }

    std::string AwsUtils::GetS3ObjectKey(const http::request<http::dynamic_body> &request) {
        if (IsS3HostStyle(request)) {
            return GetS3HostStyleObjectKey(request);
        }
        return GetS3PathStyleObjectKey(request.target());
    }

    bool AwsUtils::IsS3HostStyle(const http::request<http::dynamic_body> &request) {
        if (request.base().find(http::field::host) != request.end()) {
            const std::string host = request.base()[http::field::host];
            return StringUtils::Contains(host, ".s3.");
        }
        return false;
    }

    std::string AwsUtils::GetS3HostStyleBucket(const http::request<http::dynamic_body> &request) {
        if (request.base().find(http::field::host) != request.end()) {
            const std::string host = request.base()[http::field::host];
            return StringUtils::SubStringUntil(host, ".");
        }
        return {};
    }

    std::string AwsUtils::GetS3PathStyleBucket(const http::request<http::dynamic_body> &request) {
        return HttpUtils::GetPathParameter(request.target(), 0);
    }

    std::string AwsUtils::GetS3HostStyleObjectKey(const http::request<http::dynamic_body> &request) {
        return HttpUtils::GetPathParameter(request.target(), 0);
    }

    std::string AwsUtils::GetS3PathStyleObjectKey(const std::string &url) {
        std::vector<std::string> segments = HttpUtils::GetPathParameters(url);
        if (segments.empty()) {
            return {};
        }
        segments.erase(segments.begin());
        std::string key = StringUtils::Join(segments, "/");
        return key;
    }

    void AwsUtils::AddAuthorizationHeader(http::request<http::dynamic_body> &request, const std::string &module, const std::string &contentType, const std::string &signedHeaders, const std::string &payload) {
        const std::string region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const std::string accessKeyId = Configuration::instance().GetValue<std::string>("awsmock.access.key-id");
        const std::string secretAccessKey = Configuration::instance().GetValue<std::string>("awsmock.access.secret-access-key");

        // Mandatory headers
        request.set(http::field::host, Core::SystemUtils::GetHostName());
        request.set(http::field::content_type, contentType);
        request.set("x-amz-content-sha256", Core::Crypto::GetSha256FromString(payload));
        request.set("x-amz-date", GetISODateString());
        request.set("x-amz-security-token", secretAccessKey);

        const std::string dateString = GetDateString();
        const std::string requestVersion = "aws4_request";
        const std::string scope = dateString + "/" + region + "/" + module + "/" + requestVersion;

        const AuthorizationHeaderKeys authorizationHeaderKeys = {
                .signingVersion = "AWS4-HMAC-SHA256",
                .secretAccessKey = secretAccessKey,
                .dateTime = dateString,
                .isoDateTime = GetISODateString(),
                .scope = scope,
                .region = region,
                .module = module,
                .requestVersion = requestVersion,
                .signedHeaders = signedHeaders};
        const std::string canonicalRequest = GetCanonicalRequest(request, authorizationHeaderKeys);
        const std::string stringToSign = GetStringToSign(canonicalRequest, authorizationHeaderKeys);
        const std::string signature = GetSignature(authorizationHeaderKeys, stringToSign);

        const std::string authorization = "AWS4-HMAC-SHA256 Credential=" + accessKeyId + "/" + dateString + "/" + region + "/" + module + "/" + requestVersion + ",SignedHeaders=" + signedHeaders + ",Signature=" + signature;
        request.set(http::field::authorization, authorization);
    }

    void AwsUtils::AddAuthorizationHeader(const std::string &method, const std::string &path, std::map<std::string, std::string> &headers, const std::string &module, const std::string &contentType, const std::string &signedHeaders, const std::string &payload) {
        const std::string region = Configuration::instance().GetValue<std::string>("awsmock.region");
        const std::string accessKeyId = Configuration::instance().GetValue<std::string>("awsmock.access.key-id");
        const std::string secretAccessKey = Configuration::instance().GetValue<std::string>("awsmock.access.secret-access-key");

        // Mandatory headers
        if (!headers.contains("Host")) {
            headers["Host"] = SystemUtils::GetHostName();
        }
        headers["Content-Type"] = contentType;
        headers["X-Amz-Content-Sha256"] = Crypto::GetSha256FromString(payload);
        if (!headers.contains("X-Amz-Date")) {
            headers["X-Amz-Date"] = GetISODateString();
        }
        headers["X-Amz-Security-Token"] = secretAccessKey;

        const std::string dateString = GetDateString();
        const std::string requestVersion = "aws4_request";
        const std::string scope = dateString + "/" + region + "/" + module + "/" + requestVersion;

        const AuthorizationHeaderKeys authorizationHeaderKeys = {
                .signingVersion = "AWS4-HMAC-SHA256",
                .secretAccessKey = secretAccessKey,
                .dateTime = dateString,
                .isoDateTime = GetISODateString(),
                .scope = scope,
                .region = region,
                .module = module,
                .requestVersion = requestVersion,
                .signedHeaders = signedHeaders};
        const std::string canonicalRequest = GetCanonicalRequest(method, path, headers, payload, authorizationHeaderKeys);
        const std::string stringToSign = GetStringToSign(canonicalRequest, authorizationHeaderKeys);
        const std::string signature = GetSignature(authorizationHeaderKeys, stringToSign);

        const std::string authorization = "AWS4-HMAC-SHA256 Credential=" + accessKeyId + "/" + dateString + "/" + region + "/" + module + "/" + requestVersion + ",SignedHeaders=" + signedHeaders + ",Signature=" + signature;
        headers["Authorization"] = authorization;
    }

    bool AwsUtils::VerifySignature(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey) {

        const AuthorizationHeaderKeys authorizationHeaderKeys = GetAuthorizationKeys(request, secretAccessKey);

        const std::string canonicalRequest = GetCanonicalRequest(request, authorizationHeaderKeys);
        const std::string stringToSign = GetStringToSign(canonicalRequest, authorizationHeaderKeys);

        if (const std::string signature = GetSignature(authorizationHeaderKeys, stringToSign); !StringUtils::Equals(signature, authorizationHeaderKeys.signature)) {
            log_error << "Calculated: " << signature << " provided: " << authorizationHeaderKeys.signature;
            return false;
        }
        return true;
    }

    bool AwsUtils::VerifySignature(const http::request<request_body_t, http::basic_fields<alloc_t> > &request, const std::string &secretAccessKey) {

        const AuthorizationHeaderKeys authorizationHeaderKeys = GetAuthorizationKeys(request, secretAccessKey);

        const std::string canonicalRequest = GetCanonicalRequest(request, authorizationHeaderKeys);
        const std::string stringToSign = GetStringToSign(canonicalRequest, authorizationHeaderKeys);

        if (const std::string signature = GetSignature(authorizationHeaderKeys, stringToSign); !StringUtils::Equals(signature, authorizationHeaderKeys.signature)) {
            log_error << "Calculated: " << signature << " provided: " << authorizationHeaderKeys.signature;
            return false;
        }
        return true;
    }

    std::string AwsUtils::GetCanonicalRequest(const http::request<http::dynamic_body> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalRequest;
        canonicalRequest << request.method() << '\n';
        canonicalRequest << StringUtils::UrlEncode(request.target()) << '\n';
        canonicalRequest << GetCanonicalQueryParameters(request.target()) << '\n';
        canonicalRequest << GetCanonicalHeaders(request, authorizationHeaderKeys) << '\n';
        canonicalRequest << authorizationHeaderKeys.signedHeaders << '\n';
        canonicalRequest << GetHashedPayload(HttpUtils::GetBodyAsString(request));
        return canonicalRequest.str();
    }

    std::string AwsUtils::GetCanonicalRequest(const http::request<request_body_t, http::basic_fields<alloc_t> > &request, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalRequest;
        canonicalRequest << request.method() << '\n';
        canonicalRequest << StringUtils::UrlEncode(request.target()) << '\n';
        canonicalRequest << GetCanonicalQueryParameters(request.target()) << '\n';
        canonicalRequest << GetCanonicalHeaders(request, authorizationHeaderKeys) << '\n';
        canonicalRequest << authorizationHeaderKeys.signedHeaders << '\n';
        canonicalRequest << GetHashedPayload(HttpUtils::GetBodyAsString(request));
        return canonicalRequest.str();
    }

    std::string AwsUtils::GetCanonicalRequest(const std::string &method, const std::string &path, std::map<std::string, std::string> &headers, const std::string &payload, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalRequest;
        canonicalRequest << method << '\n';
        canonicalRequest << StringUtils::UrlEncode(path) << '\n';
        canonicalRequest << GetCanonicalQueryParameters(path) << '\n';
        canonicalRequest << GetCanonicalHeaders(headers, authorizationHeaderKeys) << '\n';
        canonicalRequest << authorizationHeaderKeys.signedHeaders << '\n';
        canonicalRequest << GetHashedPayload(payload);
        return canonicalRequest.str();
    }

    std::string AwsUtils::GetStringToSign(const std::string &canonicalRequest, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream stringToSign;
        stringToSign << authorizationHeaderKeys.signingVersion << '\n';
        stringToSign << authorizationHeaderKeys.isoDateTime << '\n';
        stringToSign << authorizationHeaderKeys.scope << '\n';
        stringToSign << Crypto::GetSha256FromString(canonicalRequest);
        return stringToSign.str();
    }

    std::string AwsUtils::GetCanonicalQueryParameters(const std::string &path) {
        std::stringstream canonicalParameters;

        // Get query parameter
        for (std::map<std::string, std::string> queryParameters = Core::HttpUtils::GetQueryParameters(path); const auto &[fst, snd]: queryParameters) {
            canonicalParameters << fst << "=" << snd + "&";
        }

        std::string canonicalParameterStr = canonicalParameters.str();

        // Remove last character
        if (StringUtils::Contains(canonicalParameterStr, "&")) {
            canonicalParameterStr.pop_back();
        }
        return canonicalParameterStr;
    }

    std::string AwsUtils::GetCanonicalHeaders(const http::request<http::dynamic_body> &request, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalHeaders;

        // Get header
        for (const auto &header: StringUtils::Split(authorizationHeaderKeys.signedHeaders, ";")) {
            if (HttpUtils::HasHeader(request, header)) {
                canonicalHeaders << StringUtils::ToLower(header) << ":" << StringUtils::Trim(HttpUtils::GetHeaderValue(request, header)) + '\n';
            }
        }
        return canonicalHeaders.str();
    }

    std::string AwsUtils::GetCanonicalHeaders(const http::request<request_body_t, http::basic_fields<alloc_t> > &request, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalHeaders;

        // Get header
        for (const auto &header: StringUtils::Split(authorizationHeaderKeys.signedHeaders, ";")) {
            if (HttpUtils::HasHeader(request, header)) {
                canonicalHeaders << StringUtils::ToLower(header) << ":" << StringUtils::Trim(HttpUtils::GetHeaderValue(request, header)) + '\n';
            }
        }
        return canonicalHeaders.str();
    }

    std::string AwsUtils::GetCanonicalHeaders(std::map<std::string, std::string> &headers, const AuthorizationHeaderKeys &authorizationHeaderKeys) {
        std::stringstream canonicalHeaders;

        // Get header
        for (const auto &header: StringUtils::Split(authorizationHeaderKeys.signedHeaders, ";")) {
            if (headers.contains(header)) {
                canonicalHeaders << StringUtils::ToLower(header) << ":" << StringUtils::Trim(headers[header]) + '\n';
            }
        }
        return canonicalHeaders.str();
    }

    std::string AwsUtils::GetHashedPayload(const std::string &payload) {
        return Crypto::GetSha256FromString(payload);
    }

    AuthorizationHeaderKeys AwsUtils::GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey) {
        const std::string authorizationHeader = request["Authorization"];

        // Get signing version
        AuthorizationHeaderKeys authKeys;
        authKeys.signingVersion = StringUtils::Split(authorizationHeader, " ")[0];

        try {
            const boost::regex expr(R"(Credential=([a-zA-Z0-9]+)\/([0-9]{8})\/([a-zA-Z0-9\-]+)\/([a-zA-Z0-9\-]+)\/(aws4_request),\ ?SignedHeaders=(.*),\ ?Signature=(.*)$)");
            boost::smatch what;
            boost::regex_search(authorizationHeader, what, expr);

            authKeys.secretAccessKey = secretAccessKey.empty() ? "none" : secretAccessKey;
            authKeys.dateTime = what[2];
            authKeys.region = what[3];
            authKeys.module = what[4];
            authKeys.requestVersion = what[5];
            authKeys.signedHeaders = what[6];
            authKeys.signature = what[7];
            authKeys.scope = authKeys.dateTime + "/" + authKeys.region + "/" + authKeys.module + "/" + authKeys.requestVersion;
            authKeys.isoDateTime = HttpUtils::HasHeader(request, "x-amz-date") ? HttpUtils::GetHeaderValue(request, "x-amz-date") : GetISODateString();
            return authKeys;

        } catch (std::exception &e) {
            log_error << e.what();
        }
        return {};
    }

    AuthorizationHeaderKeys AwsUtils::GetAuthorizationKeys(const http::request<request_body_t, http::basic_fields<alloc_t> > &request, const std::string &secretAccessKey) {
        const std::string authorizationHeader = request["Authorization"];

        // Get signing version
        AuthorizationHeaderKeys authKeys;
        authKeys.signingVersion = StringUtils::Split(authorizationHeader, " ")[0];

        try {
            const boost::regex expr(R"(Credential=([a-zA-Z0-9]+)\/([0-9]{8})\/([a-zA-Z0-9\-]+)\/([a-zA-Z0-9\-]+)\/(aws4_request),\ ?SignedHeaders=(.*),\ ?Signature=(.*)$)");
            boost::smatch what;
            boost::regex_search(authorizationHeader, what, expr);

            authKeys.secretAccessKey = secretAccessKey.empty() ? "none" : secretAccessKey;
            authKeys.dateTime = what[2];
            authKeys.region = what[3];
            authKeys.module = what[4];
            authKeys.requestVersion = what[5];
            authKeys.signedHeaders = what[6];
            authKeys.signature = what[7];
            authKeys.scope = authKeys.dateTime + "/" + authKeys.region + "/" + authKeys.module + "/" + authKeys.requestVersion;
            authKeys.isoDateTime = HttpUtils::HasHeader(request, "x-amz-date") ? HttpUtils::GetHeaderValue(request, "x-amz-date") : GetISODateString();
            return authKeys;

        } catch (std::exception &e) {
            log_error << e.what();
        }
        return {};
    }

    std::string AwsUtils::GetSignature(const AuthorizationHeaderKeys &authorizationHeaderKeys, const std::string &stringToSign) {
        std::vector<unsigned char> digest = Crypto::GetHmacSha256FromStringRaw("AWS4" + authorizationHeaderKeys.secretAccessKey, authorizationHeaderKeys.dateTime);
        digest = Crypto::GetHmacSha256FromStringRaw(digest, authorizationHeaderKeys.region);
        digest = Crypto::GetHmacSha256FromStringRaw(digest, authorizationHeaderKeys.module);
        digest = Crypto::GetHmacSha256FromStringRaw(digest, authorizationHeaderKeys.requestVersion);
        digest = Crypto::GetHmacSha256FromStringRaw(digest, stringToSign);
        return Crypto::HexEncode(digest.data(), static_cast<int>(digest.size()));
    }

    std::string AwsUtils::CreateCognitoConfirmationCode() {
        return StringUtils::GenerateRandomString(40);
    }

    std::string AwsUtils::GetDateString() {
        const auto t = std::time(nullptr);
        const auto tm = *std::gmtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d");
        return oss.str();
    }

    std::string AwsUtils::GetISODateString() {
        const auto t = std::time(nullptr);
        const auto tm = *std::gmtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%FT%TZ");
        return oss.str();
    }
}// namespace AwsMock::Core
