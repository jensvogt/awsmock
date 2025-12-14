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
        applicationDto.runType = AppsRunTypeFromString(applicationEntity.type);
        applicationDto.privatePort = applicationEntity.privatePort;
        applicationDto.publicPort = applicationEntity.publicPort;
        applicationDto.archive = applicationEntity.archive;
        applicationDto.version = applicationEntity.version;
        applicationDto.imageId = applicationEntity.imageId;
        applicationDto.imageName = applicationEntity.imageName;
        applicationDto.containerId = applicationEntity.containerId;
        applicationDto.containerName = applicationEntity.containerName;
        applicationDto.status = AppsStatusTypeFromString(applicationEntity.status);
        applicationDto.enabled = applicationEntity.enabled;
        applicationDto.description = applicationEntity.description;
        applicationDto.lastStarted = applicationEntity.lastStarted;
        applicationDto.created = applicationEntity.created;
        applicationDto.modified = applicationEntity.modified;
        applicationDto.environment = applicationEntity.environment;
        applicationDto.tags = applicationEntity.tags;
        applicationDto.dependencies = applicationEntity.dependencies;
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
        applicationEntity.type = AppsRunTypeToString(applicationDto.runType);
        applicationEntity.privatePort = applicationDto.privatePort;
        applicationEntity.publicPort = applicationDto.publicPort;
        applicationEntity.archive = applicationDto.archive;
        applicationEntity.version = applicationDto.version;
        applicationEntity.imageId = applicationDto.imageId;
        applicationEntity.containerId = applicationDto.containerId;
        applicationEntity.containerName = applicationDto.containerName;
        applicationEntity.status = AppsStatusTypeToString(applicationDto.status);
        applicationEntity.enabled = applicationDto.enabled;
        applicationEntity.description = applicationDto.description;
        applicationEntity.lastStarted = applicationDto.lastStarted;
        applicationEntity.created = applicationDto.created;
        applicationEntity.modified = applicationDto.modified;
        applicationEntity.environment = applicationDto.environment;
        applicationEntity.tags = applicationDto.tags;
        applicationEntity.dependencies = applicationDto.dependencies;
        return applicationEntity;
    }

    std::vector<Database::Entity::Apps::Application> Mapper::map(const std::vector<Application> &applicationDtos) {
        std::vector<Database::Entity::Apps::Application> applicationEntities;
        for (const auto &applicationDto: applicationDtos) {
            applicationEntities.emplace_back(map(applicationDto));
        }
        return applicationEntities;
    }
} // namespace AwsMock::Dto::Apps
