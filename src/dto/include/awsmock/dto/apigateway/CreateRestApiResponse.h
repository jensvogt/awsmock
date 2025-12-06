//
// Created by vogje01 on 01/09/2025
//

#ifndef AWSMOCK_DTO_API_GATEWAY_CREATE_REST_API_RESPONSE_H
#define AWSMOCK_DTO_API_GATEWAY_CREATE_REST_API_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/ApiKeySource.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    using std::chrono::system_clock;

    /**
     * @brief Create Rest API response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateRestApiResponse final : Common::BaseCounter<CreateRestApiResponse> {

        /**
         * ID
         */
        std::string id;

        /**
         * The name of the RestApi.
         */
        std::string name;

        /**
         * The source of the API key for metering requests according to a usage plan. Valid values are: HEADER to read the API
         * key from the X-API-Key header of a request. AUTHORIZER to read the API key from the UsageIdentifierKey from a custom authorizer.
         */
        ApiKeySourceType apiKeySource = ApiKeySourceType::UNKNOWN;

        /**
         * The API's root resource ID.
         */
        std::string rootResourceId;

        /**
         * The description of the RestApi.
         */
        std::string description;

        /**
         * Specifies whether clients can invoke your API by using the default execute-api endpoint. By default, clients can invoke your API with
         * the default https://{api_id}.execute-api.{region}.amazonaws.com endpoint. To require that clients use a custom domain name to invoke
         * your API, disable the default endpoint
         */
        bool disableExecuteApiEndpoint{};

        /**
         * A version identifier for the API.
         */
        std::string version;

        /**
         * A nullable integer that is used to enable compression (with non-negative between 0 and 10485760 (10M) bytes, inclusive) or disable
         * compression (with a null value) on an API. When compression is enabled, compression or decompression is not applied on the payload if the
         * payload size is smaller than this value. Setting it to zero allows compression for any payload size.
         */
        long minimumCompressionSize{};

        /**
         * A stringified JSON policy document that applies to this RestApi regardless of the caller and Method configuration.
         */
        std::string policy;

        /**
         * The list of binary media types supported by the RestApi. By default, the RestApi supports only UTF-8-encoded text payloads.
         */
        std::vector<std::string> binaryMediaTypes;

        /**
         * The collection of tags. Each tag element is associated with a given resource.
         */
        std::map<std::string, std::string> tags;

        /**
         * The warning messages reported when 'fail on warnings' is turned on during API import.
         */
        std::vector<std::string> warnings;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

      private:

        friend CreateRestApiResponse tag_invoke(boost::json::value_to_tag<CreateRestApiResponse>, boost::json::value const &v) {
            CreateRestApiResponse r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.apiKeySource = ApiKeySourceTypeFromString(Core::Json::GetStringValue(v, "apiKeySource"));
            r.rootResourceId = Core::Json::GetStringValue(v, "rootResourceId");
            r.description = Core::Json::GetStringValue(v, "description");
            r.disableExecuteApiEndpoint = Core::Json::GetBoolValue(v, "disableExecuteApiEndpoint");
            r.version = Core::Json::GetStringValue(v, "version");
            r.minimumCompressionSize = Core::Json::GetLongValue(v, "minimumCompressionSize");
            r.policy = Core::Json::GetStringValue(v, "policy");
            r.created = Core::Json::GetDatetimeValue(v, "createdDate");
            if (Core::Json::AttributeExists(v, "binaryMediaTypes")) {
                r.binaryMediaTypes = boost::json::value_to<std::vector<std::string>>(v.at("binaryMediaTypes"));
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            if (Core::Json::AttributeExists(v, "warnings")) {
                r.warnings = boost::json::value_to<std::vector<std::string>>(v.at("warnings"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateRestApiResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"apiKeySource", ApiKeySourceTypeToString(obj.apiKeySource)},
                    {"description", obj.description},
                    {"disableExecuteApiEndpoint", obj.disableExecuteApiEndpoint},
                    {"version", obj.version},
                    {"minimumCompressionSize", obj.minimumCompressionSize},
                    {"policy", obj.policy},
                    {"binaryMediaTypes", boost::json::value_from(obj.binaryMediaTypes)},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"warnings", boost::json::value_from(obj.warnings)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_CREATE_REST_API_RESPONSE_H
