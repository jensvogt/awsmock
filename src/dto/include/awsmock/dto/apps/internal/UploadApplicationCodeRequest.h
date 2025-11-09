//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPLOAD_APPLICATION_CODE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPLOAD_APPLICATION_CODE_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Upload application code
     *
     * Example:
     * @code{.json}
     * {
     *   "applicationName": "string",
     *   "applicationCode": string,
     *   "version": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UploadApplicationCodeRequest final : Common::BaseCounter<UploadApplicationCodeRequest> {

        /**
         * Application name
         */
        std::string applicationName;

        /**
         * Base64 encoded application code string
         */
        std::string applicationCode;

        /**
         * Version
         */
        std::string version;

        /**
         * Archive
         */
        std::string archive;

    private:

        friend UploadApplicationCodeRequest tag_invoke(boost::json::value_to_tag<UploadApplicationCodeRequest>, boost::json::value const &v) {
            UploadApplicationCodeRequest r;
            r.applicationName = Core::Json::GetStringValue(v, "applicationName");
            r.applicationCode = Core::Json::GetStringValue(v, "applicationCode");
            r.version = Core::Json::GetStringValue(v, "version");
            r.archive = Core::Json::GetStringValue(v, "archive");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UploadApplicationCodeRequest const &obj) {
            jv = {
                    {"applicationName", obj.applicationName},
                    {"applicationCode", obj.applicationCode},
                    {"version", obj.version},
                    {"archive", obj.archive},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_LAMBDA_UPLOAD_APPLICATION_CODE_REQUEST_H
