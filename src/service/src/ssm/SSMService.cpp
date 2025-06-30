//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/dto/common/mapper/Mapper.h"


#include <awsmock/service/ssm/SSMService.h>

namespace AwsMock::Service {

    SSMService::SSMService() : _ssmDatabase(Database::SSMDatabase::instance()) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::SSM::PutParameterResponse SSMService::PutParameter(const Dto::SSM::PutParameterRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "put_parameter");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "put_parameter");
        log_trace << "Put parameter request: " << request.ToString();

        if (_ssmDatabase.ParameterExists(request.name)) {
            log_error << "Parameter exists already, name: " << request.name;
            throw Core::ServiceException("Parameter exists already, name: " + request.name);
        }

        try {
            // Update database
            const std::string arn = Core::AwsUtils::CreateSSMParameterArn(request.region, _accountId, request.name);
            Database::Entity::SSM::Parameter parameterEntity = {
                    .region = request.region,
                    .parameterName = request.name,
                    .parameterValue = request.parameterValue,
                    .description = request.description,
                    .type = Dto::SSM::ParameterTypeToString(request.type),
                    .version = 1,
                    .arn = arn,
                    .tags = request.tags,
                    .created = system_clock::now(),
                    .modified = system_clock::now(),
            };

            // Store in the database
            parameterEntity = _ssmDatabase.CreateParameter(parameterEntity);
            log_trace << "SSM parameter created: " << parameterEntity.ToString();

            Dto::SSM::PutParameterResponse response;
            response.tier = parameterEntity.tier;
            response.version = parameterEntity.version;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM put parameter failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SSM::GetParameterResponse SSMService::GetParameter(const Dto::SSM::GetParameterRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "get_parameter");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "get_parameter");
        log_trace << "Get parameter request: " << request.ToString();

        if (!_ssmDatabase.ParameterExists(request.name)) {
            log_error << "Parameter does not exist, name: " << request.name;
            throw Core::ServiceException("Parameter does not exist, name: " + request.name);
        }

        try {
            // Get from the database
            const Database::Entity::SSM::Parameter parameterEntity = _ssmDatabase.GetParameterByName(request.name);
            log_trace << "SSM parameter found: " << parameterEntity.ToString();

            return Dto::SSM::Mapper::map(request, parameterEntity);

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM get parameter failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SSM::DescribeParametersResponse SSMService::DescribeParameters(const Dto::SSM::DescribeParametersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "describe_parameters");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "describe_parameters");
        log_trace << "Describe parameters request: " << request.ToString();

        try {
            // Get from the database
            const Database::Entity::SSM::ParameterList parameterEntities = _ssmDatabase.ListParameters(request.region);
            log_trace << "SSM parameters found: " << parameterEntities.size();

            return Dto::SSM::Mapper::map(request, parameterEntities);

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM describe parameters failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SSM::ListParameterCountersResponse SSMService::ListParameterCounters(const Dto::SSM::ListParameterCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "list_parameter_counters");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "list_parameter_counters");
        log_trace << "List parameter counters request: " << request.ToString();

        try {
            Dto::SSM::ListParameterCountersResponse response;

            // Get from the database
            const Database::Entity::SSM::ParameterList parameterEntities = _ssmDatabase.ListParameters(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            response.total = _ssmDatabase.CountParameters(request.region, request.prefix);
            response.parameterCounters = Dto::SSM::Mapper::map(parameterEntities);
            log_trace << "SSM parameters found: " << parameterEntities.size();

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM list parameter counters failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void SSMService::DeleteParameter(const Dto::SSM::DeleteParameterRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "delete_parameter");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "delete_parameter");
        log_trace << "Delete parameter request: " << request.ToString();

        if (!_ssmDatabase.ParameterExists(request.name)) {
            log_error << "Parameter does not exist, name: " << request.name;
            throw Core::ServiceException("Parameter does not exist, name: " + request.name);
        }

        try {
            // Get from the database
            const Database::Entity::SSM::Parameter parameterEntity = _ssmDatabase.GetParameterByName(request.name);
            log_trace << "SSM parameter found: " << parameterEntity.ToString();

            _ssmDatabase.DeleteParameter(parameterEntity);

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM delete parameter failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SSM::ListParameterCountersResponse SSMService::DeleteParameterCounter(const Dto::SSM::DeleteParameterCounterRequest &request) const {
        Monitoring::MetricServiceTimer measure(SSM_SERVICE_TIMER, "action", "delete_parameter");
        Monitoring::MetricService::instance().IncrementCounter(SSM_SERVICE_TIMER, "action", "delete_parameter");
        log_trace << "Delete parameter request: " << request.ToString();

        if (!_ssmDatabase.ParameterExists(request.name)) {
            log_error << "Parameter does not exist, name: " << request.name;
            throw Core::ServiceException("Parameter does not exist, name: " + request.name);
        }

        try {
            // Get from the database
            const Database::Entity::SSM::Parameter parameterEntity = _ssmDatabase.GetParameterByName(request.name);
            log_trace << "SSM parameter found: " << parameterEntity.ToString();

            _ssmDatabase.DeleteParameter(parameterEntity);

            // Get the list from the database
            const Database::Entity::SSM::ParameterList parameterEntities = _ssmDatabase.ListParameters(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            log_trace << "SSM parameters found: " << parameterEntities.size();
            Dto::SSM::ListParameterCountersResponse response;
            response.total = static_cast<long>(parameterEntities.size());
            response.parameterCounters = Dto::SSM::Mapper::map(parameterEntities);
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SSM delete parameter failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

}// namespace AwsMock::Service