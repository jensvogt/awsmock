//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_DTO_COMMON_COMMON_H
#define AWSMOCK_DTO_COMMON_COMMON_H

// C++ standard includes
#include <string>
#include <utility>

namespace AwsMock::Dto {

    /**
     * @brief Common DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CommonDto {

        /**
         * @brief Constructor
         *
         * @param resource AWS resource
         * @param requestId AWS request ID
         */
        CommonDto(std::string resource, std::string requestId) : resource(std::move(resource)), requestId(std::move(requestId)) {}

        /**
         * Resource
         */
        std::string resource = "Unknown resource";

        /**
         * Resource
         */
        std::string requestId = Core::StringUtils::CreateRandomUuid();
    };

}// namespace AwsMock::Dto

#endif// AWSMOCK_DTO_COMMON_COMMON_H
