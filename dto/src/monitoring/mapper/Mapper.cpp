//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/monitoring/mapper/Mapper.h>

namespace AwsMock::Dto::Monitoring {

    Counter Mapper::map(const Database::Entity::Monitoring::Counter &counterEntity) {
        Counter counter;
        counter.timestamp = counterEntity.timestamp;
        counter.performanceValue = counterEntity.performanceValue;
        return counter;
    }

    std::vector<Counter> Mapper::map(const std::vector<Database::Entity::Monitoring::Counter> &counterEntity) {
        std::vector<Counter> counters;
        for (const auto &counter: counterEntity) {
            counters.push_back(map(counter));
        }
        return counters;
    }

    Database::Entity::Monitoring::Counter Mapper::map(const Counter &counterDto) {
        Database::Entity::Monitoring::Counter counter;
        counter.timestamp = counterDto.timestamp;
        counter.performanceValue = counterDto.performanceValue;
        return counter;
    }

    std::vector<Database::Entity::Monitoring::Counter> Mapper::map(const std::vector<Counter> &counterEntity) {
        std::vector<Database::Entity::Monitoring::Counter> counters;
        for (const auto &counter: counterEntity) {
            counters.push_back(map(counter));
        }
        return counters;
    }

}// namespace AwsMock::Dto::Monitoring