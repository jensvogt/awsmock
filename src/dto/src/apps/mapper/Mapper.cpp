//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/apps/mapper/Mapper.h>

namespace AwsMock::Dto::Apps {

    Application Mapper::map(const Database::Entity::Apps::Application &applicationEntity) {
        Application applicationDto;
        applicationDto.region = applicationEntity.region;
        applicationDto.name = applicationEntity.name;
        applicationDto.runtime = AppsRuntimeTypeFromString(applicationEntity.runtime);
        applicationDto.archive = applicationEntity.archive;
        applicationDto.version = applicationEntity.version;
        applicationDto.containerId = applicationEntity.containerId;
        applicationDto.status = AppsStatusTypeFromString(applicationEntity.status);
        applicationDto.enabled = applicationEntity.enabled;
        applicationDto.created = applicationEntity.created;
        applicationDto.modified = applicationEntity.modified;
        return applicationDto;
    }

    std::vector<Application> Mapper::map(const std::vector<Database::Entity::Apps::Application> &applicationEntities) {
        std::vector<Application> applicationDtos;
        for (const auto &applicationEntity: applicationEntities) {
            applicationDtos.emplace_back(map(applicationEntity));
        }
        return applicationDtos;
    }

    Database::Entity::Apps::Application Mapper::map(const Application &applicationDto) {
        Database::Entity::Apps::Application applicationEntity;
        applicationEntity.region = applicationDto.region;
        applicationEntity.name = applicationDto.name;
        applicationEntity.runtime = AppsRuntimeTypeToString(applicationDto.runtime);
        applicationEntity.archive = applicationDto.archive;
        applicationEntity.version = applicationDto.version;
        applicationEntity.containerId = applicationDto.containerId;
        applicationEntity.status = AppsStatusTypeToString(applicationDto.status);
        applicationEntity.enabled = applicationDto.enabled;
        applicationEntity.created = applicationDto.created;
        applicationEntity.modified = applicationDto.modified;
        return applicationEntity;
    }

    std::vector<Database::Entity::Apps::Application> Mapper::map(const std::vector<Application> &applicationDtos) {
        std::vector<Database::Entity::Apps::Application> applicationEntities;
        for (const auto &applicationDto: applicationDtos) {
            applicationEntities.emplace_back(map(applicationDto));
        }
        return applicationEntities;
    }

}// namespace AwsMock::Dto::Apps