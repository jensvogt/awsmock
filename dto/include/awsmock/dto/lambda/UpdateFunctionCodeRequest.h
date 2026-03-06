//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_CODE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_CODE_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Update the function code
     *
     * Example:
     * @code{.json}
     * {
     *    "Architectures": [ "string" ],
     *    "DryRun": boolean,
     *    "ImageUri": "string",
     *    "Publish": boolean,
     *    "PublishTo": "string",
     *    "RevisionId": "string",
     *    "S3Bucket": "string",
     *    "S3Key": "string",
     *    "S3ObjectVersion": "string",
     *    "SourceKMSKeyArn": "string",
     *    "ZipFile": blob
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateFunctionCodeRequest final : Common::BaseCounter<UpdateFunctionCodeRequest> {

        /**
         * Lambda function name
         */
        std::string functionName;

        /**
         * Architectures
         */
        std::vector<std::string> architectures;

        /**
         * Base64 encoded function code string
         */
        std::string zipFile;

        /**
         * Revision
         */
        std::string revisionId;

        /**
         * Dry run
         */
        bool dryRun = false;

      private:

        friend UpdateFunctionCodeRequest tag_invoke(boost::json::value_to_tag<UpdateFunctionCodeRequest>, boost::json::value const &v) {
            UpdateFunctionCodeRequest r;
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.zipFile = Core::Json::GetStringValue(v, "ZipFile");
            r.revisionId = Core::Json::GetStringValue(v, "RevisionId");
            r.dryRun = Core::Json::GetBoolValue(v, "DryRun");
            if (Core::Json::AttributeExists(v, "Architectures")) {
                r.architectures = boost::json::value_to<std::vector<std::string>>(v.at("Architectures"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateFunctionCodeRequest const &obj) {
            jv = {
                    {"FunctionName", obj.functionName},
                    {"ZipFile", obj.zipFile},
                    {"RevisionId", obj.revisionId},
                    {"DryRun", obj.dryRun},
                    {"Architectures", boost::json::value_from(obj.revisionId)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_CODE_REQUEST_H
