//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUES_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_QUEUES_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queues request
     *
     * Example:
     * @code(.json)
     * {
     *   "MaxResults": 10,
     *   "NextToken": "<string>",
     *   "QueueNamePrefix": "<string>"
     * }"
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueuesRequest final : Common::BaseCounter<ListQueuesRequest> {

        /**
         * Max results
         */
        long maxResults = -1;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Queue name prefix
         */
        std::string queueNamePrefix;

      private:

        friend ListQueuesRequest tag_invoke(boost::json::value_to_tag<ListQueuesRequest>, boost::json::value const &v) {
            ListQueuesRequest r;
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.queueNamePrefix = Core::Json::GetStringValue(v, "QueueNamePrefix");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueuesRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MaxResults", obj.maxResults},
                    {"NextToken", obj.nextToken},
                    {"QueueNamePrefix", obj.queueNamePrefix},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUES_REQUEST_H
