//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_WAIT_TIME_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_WAIT_TIME_H

namespace AwsMock::Database::Entity::SQS {

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

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_WAIT_TIME_H
