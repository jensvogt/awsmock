//
// Created by vogje01 on 6/5/24.
//

#pragma once

// C++ includes
#include <map>
#include <string>

namespace Awsmock::Database::Entity::SQS {

    /**
     * @brief Aggregate structure for the waiting time.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageWaitTime {

        /**
         * Map with the average waiting time per queue. Key is the queueUrl, value the average waiting time.
         */
        std::map<std::string, double> waitTime;
    };

}// namespace Awsmock::Database::Entity::SQS
