//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_REBUILD_LAMBDA_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_REBUILD_LAMBDA_REQUEST_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {
    struct RebuildLambdaRequest final : Common::BaseCounter<RebuildLambdaRequest> {
        /**
         * Function name
         */
        std::string name;

        /**
         * Version
         */
        std::string version;

    private:
        friend RebuildLambdaRequest tag_invoke(boost::json::value_to_tag<RebuildLambdaRequest>, boost::json::value const &v) {
            RebuildLambdaRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.version = Core::Json::GetStringValue(v, "version");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RebuildLambdaRequest const &obj) {
            jv = {
                {"name", obj.name},
                {"version", obj.version},
            };
        }
    };
} // namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_REBUILD_LAMBDA_REQUEST_H
