//
// Created by vogje01 on 1/20/24.
//

#ifndef AWSMOCK_DTO_DOCKER_FILTERS_H
#define AWSMOCK_DTO_DOCKER_FILTERS_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Dto::Docker {

    /**
     * Docker filter
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Filter {

        /**
         * Docker image/container name
         */
        std::string name;

        /**
         * Docker image/container tag
         */
        std::string value;
    };

    /**
     * Docker filter list
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Filters {

        /**
         * Vector of filters
         */
        std::vector<Filter> filters;

        /**
         * @brief Convert to a JSON string
         *
         * @return object JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Filters &c);
    };

}// namespace AwsMock::Dto::Docker

#endif//AWSMOCK_DTO_DOCKER_FILTERS_H
