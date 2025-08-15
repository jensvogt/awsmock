//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_IMAGE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_IMAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function docker image.
     *
     * @par
     * First stops all containers and remove containers from docker. If all containers are stopped, the image is removed from the
     * docker daemon.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteImageRequest final : Common::BaseCounter<DeleteImageRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

      private:

        friend DeleteImageRequest tag_invoke(boost::json::value_to_tag<DeleteImageRequest>, boost::json::value const &v) {
            DeleteImageRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteImageRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
