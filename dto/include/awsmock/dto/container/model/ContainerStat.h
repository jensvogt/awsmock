//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CONTAINER_STAT_H
#define AWSMOCK_DTO_DOCKER_CONTAINER_STAT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/container/model/CpuStat.h>
#include <awsmock/dto/container/model/MemoryStat.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ContainerStat final : Common::BaseCounter<ContainerStat> {

        /**
         * ContainerId
         */
        std::string containerId;

        /**
         * Name
         */
        std::string name;

        /**
         * OS type
         */
        std::string osType;

        /**
         * The state of this container (e.g. Exited)
         */
        State state;

        /**
         * Number of processes
         */
        int numProcs;

        /**
         * Read timestamp
         */
        system_clock::time_point read;

        /**
         * Pre read timestamp
         */
        system_clock::time_point preRead;

        /**
         * Container memory statistics
         */
        MemoryStat memoryStats;

        /**
         * Container CPU statistics
         */
        CpuStat cpuStats;

        /**
         * Previous container CPU statistics
         */
        CpuStat preCpuStats;

      private:

        friend ContainerStat tag_invoke(boost::json::value_to_tag<ContainerStat>, boost::json::value const &v) {
            ContainerStat r;
            r.containerId = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.osType = Core::Json::GetStringValue(v, "os_type");
            r.numProcs = Core::Json::GetIntValue(v, "num_procs");
            r.memoryStats = boost::json::value_to<MemoryStat>(v.at("memory_stats"));
            r.cpuStats = boost::json::value_to<CpuStat>(v.at("cpu_stats"));
            r.preCpuStats = boost::json::value_to<CpuStat>(v.at("precpu_stats"));
            r.read = Core::Json::GetDatetimeValue(v, "read");
            r.preRead = Core::Json::GetDatetimeValue(v, "preread");
            if (Core::Json::AttributeExists(v, "state")) {
                r.state = boost::json::value_to<State>(v.at("state"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ContainerStat const &obj) {
            jv = {
                    {"id", obj.containerId},
                    {"name", obj.name},
                    {"os_type", obj.osType},
                    {"num_procs", obj.numProcs},
                    {"memory_stats", boost::json::value_from(obj.memoryStats)},
                    {"cpu_stats", boost::json::value_from(obj.cpuStats)},
                    {"precpu_stats", boost::json::value_from(obj.preCpuStats)},
                    {"read", Core::DateTimeUtils::ToISO8601(obj.read)},
                    {"preread", Core::DateTimeUtils::ToISO8601(obj.preRead)},
                    {"state", boost::json::value_from(obj.state)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
