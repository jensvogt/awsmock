//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_MONITORING_MAPPER_H
#define AWSMOCK_DTO_MONITORING_MAPPER_H

// AwsMock includes
#include <awsmock/dto/monitoring/Counter.h>
#include <awsmock/entity/monitoring/Counter.h>

namespace AwsMock::Dto::Monitoring {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a monitoring counter-entity to a counter DTO
         *
         * @param counterEntity monitoring counter-entity
         * @return monitoring counter DTO
         */
        static Counter map(const Database::Entity::Monitoring::Counter &counterEntity);

        /**
         * @brief Maps a monitoring counter-entity list to a counter DTO list
         *
         * @param counterEntity monitoring counter-entity
         * @return monitoring counter DTO
         */
        static std::vector<Counter> map(const std::vector<Database::Entity::Monitoring::Counter> &counterEntity);

        /**
         * @brief Maps a monitoring counter DTO list to a counter-entity
         *
         * @param counterDto monitoring counter-entity
         * @return monitoring counter-entity
         */
        static Database::Entity::Monitoring::Counter map(const Counter &counterDto);

        /**
         * @brief Maps a monitoring counter DTO list to a counter-entity
         *
         * @param counterDto monitoring counter-entity
         * @return monitoring counter-entity
         */
        static std::vector<Database::Entity::Monitoring::Counter> map(const std::vector<Counter> &counterDto);
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_MAPPER_H
