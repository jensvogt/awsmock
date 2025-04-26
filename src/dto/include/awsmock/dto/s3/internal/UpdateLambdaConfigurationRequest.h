//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H
#define AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>

namespace AwsMock::Dto::S3 {

    struct UpdateLambdaConfigurationRequest final : Common::BaseCounter<UpdateLambdaConfigurationRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Lambda configuration
         */
        LambdaConfiguration lambdaConfiguration;

      private:

        friend UpdateLambdaConfigurationRequest tag_invoke(boost::json::value_to_tag<UpdateLambdaConfigurationRequest>, boost::json::value const &v) {

            UpdateLambdaConfigurationRequest r;
            r.region = v.at("region").as_string();
            r.bucket = v.at("bucket").as_string();

            // Lambda configuration
            r.lambdaConfiguration = boost::json::value_to<LambdaConfiguration>(v.at("lambdaConfiguration"));

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateLambdaConfigurationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"bucket", obj.bucket},
                    {"lambdaConfiguration", boost::json::value_from(obj.lambdaConfiguration)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H
