
#include "awsmock/dto/ssm/internal/GetParameterCounterRequest.h"
#include "awsmock/dto/ssm/internal/GetParameterCounterResponse.h"


#include <awsmock/service/ssm/SSMHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> SSMHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "SSM POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::SSMClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {

                case Dto::Common::SSMCommandType::PUT_PARAMETER: {

                    Dto::SSM::PutParameterRequest ssmRequest = Dto::SSM::PutParameterRequest::FromJson(clientCommand);
                    Dto::SSM::PutParameterResponse ssmResponse = _ssmService.PutParameter(ssmRequest);
                    log_info << "Parameter created, name: " << ssmRequest.name << " version: " << ssmResponse.version;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::GET_PARAMETER: {

                    Dto::SSM::GetParameterRequest ssmRequest = Dto::SSM::GetParameterRequest::FromJson(clientCommand);
                    Dto::SSM::GetParameterResponse ssmResponse = _ssmService.GetParameter(ssmRequest);
                    log_info << "Parameter found, name: " << ssmRequest.name;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::GET_PARAMETER_COUNTER: {

                    Dto::SSM::GetParameterCounterRequest ssmRequest = Dto::SSM::GetParameterCounterRequest::FromJson(clientCommand);
                    Dto::SSM::GetParameterCounterResponse ssmResponse = _ssmService.GetParameterCounter(ssmRequest);
                    log_info << "Parameter found, name: " << ssmRequest.name << ", body: " << ssmResponse.ToJson();
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::DESCRIBE_PARAMETERS: {

                    Dto::SSM::DescribeParametersRequest ssmRequest = Dto::SSM::DescribeParametersRequest::FromJson(clientCommand);
                    Dto::SSM::DescribeParametersResponse ssmResponse = _ssmService.DescribeParameters(ssmRequest);
                    log_info << "Describe parameters, region: " << ssmRequest.region;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::LIST_PARAMETER_COUNTERS: {

                    Dto::SSM::ListParameterCountersRequest ssmRequest = Dto::SSM::ListParameterCountersRequest::FromJson(clientCommand);
                    Dto::SSM::ListParameterCountersResponse ssmResponse = _ssmService.ListParameterCounters(ssmRequest);
                    log_info << "List parameter counters, region: " << ssmRequest.region;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::CREATE_PARAMETER_COUNTER: {

                    Dto::SSM::CreateParameterCounterRequest ssmRequest = Dto::SSM::CreateParameterCounterRequest::FromJson(clientCommand);
                    Dto::SSM::ListParameterCountersResponse ssmResponse = _ssmService.CreateParameter(ssmRequest);
                    log_info << "Parameter created, name: " << ssmRequest.name;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::UPDATE_PARAMETER_COUNTER: {

                    Dto::SSM::UpdateParameterCounterRequest ssmRequest = Dto::SSM::UpdateParameterCounterRequest::FromJson(clientCommand);
                    Dto::SSM::ListParameterCountersResponse ssmResponse = _ssmService.UpdateParameter(ssmRequest);
                    log_info << "Parameter updated, name: " << ssmRequest.name;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                case Dto::Common::SSMCommandType::DELETE_PARAMETER: {

                    Dto::SSM::DeleteParameterRequest ssmRequest = Dto::SSM::DeleteParameterRequest::FromJson(clientCommand);
                    _ssmService.DeleteParameter(ssmRequest);
                    log_info << "Parameter deleted, name: " << ssmRequest.name;
                    return SendOkResponse(request);
                }

                case Dto::Common::SSMCommandType::DELETE_PARAMETER_COUNTER: {

                    Dto::SSM::DeleteParameterCounterRequest ssmRequest = Dto::SSM::DeleteParameterCounterRequest::FromJson(clientCommand);
                    Dto::SSM::ListParameterCountersResponse ssmResponse = _ssmService.DeleteParameterCounter(ssmRequest);
                    log_info << "Parameter deleted, name: " << ssmRequest.name;
                    return SendOkResponse(request, ssmResponse.ToJson());
                }

                default:
                case Dto::Common::SSMCommandType::UNKNOWN: {
                    log_error << "Unimplemented command called";
                    throw Core::ServiceException("Unimplemented command called");
                }
            }
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }
}// namespace AwsMock::Service
