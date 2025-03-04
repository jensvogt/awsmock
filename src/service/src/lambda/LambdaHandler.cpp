
#include <awsmock/service/lambda/LambdaHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> LambdaHandler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Lambda GET request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::LambdaClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {

                case Dto::Common::LambdaCommandType::LIST_LAMBDAS: {

                    Dto::Lambda::ListFunctionResponse lambdaResponse = _lambdaService.ListFunctions(region);
                    log_trace << "Lambda function list";
                    return SendOkResponse(request, lambdaResponse.ToJson());
                }

                case Dto::Common::LambdaCommandType::WAIT_LAMBDA: {

                    std::string body = Core::HttpUtils::GetBodyAsString(request);

                    std::string functionName = Core::HttpUtils::GetPathParameters(request.target())[2];
                    Dto::Lambda::GetFunctionResponse lambdaResponse = _lambdaService.GetFunction(region, functionName);
                    std::map<std::string, std::string> headers;
                    headers["Content-Length"] = std::to_string(lambdaResponse.ToXml().length());
                    headers["Content-Type"] = "application/xml";
                    return SendOkResponse(request, lambdaResponse.ToJson(), headers);
                }

                case Dto::Common::LambdaCommandType::LIST_EVENT_SOURCE_MAPPINGS: {

                    std::string functionName = Core::HttpUtils::GetQueryParameterValueByName(request.target(), "FunctionName");
                    std::string eventSourceArn = Core::HttpUtils::GetQueryParameterValueByName(request.target(), "EventSourceArn");
                    std::string marker = Core::HttpUtils::GetQueryParameterValueByName(request.target(), "Marker");
                    int maxItems = Core::NumberUtils::ToInt(Core::HttpUtils::GetQueryParameterValueByName(request.target(), "MaxItems"));

                    Dto::Lambda::ListEventSourceMappingsRequest lambdaRequest = {.functionName = functionName, .eventSourceArn = eventSourceArn, .marker = marker, .maxItems = maxItems};
                    lambdaRequest.region = clientCommand.region;
                    lambdaRequest.user = clientCommand.user;
                    lambdaRequest.requestId = clientCommand.requestId;

                    Dto::Lambda::ListEventSourceMappingsResponse lambdaResponse = _lambdaService.ListEventSourceMappings(lambdaRequest);

                    return SendOkResponse(request, lambdaResponse.ToJson());
                }

                default:
                    break;
            }
            std::map<std::string, std::string> headers = Core::HttpUtils::GetHeaders(request);

            std::string version, action;
            Core::HttpUtils::GetVersionAction(request.target(), version, action);

            if (action == "functions") {


                if (Core::HttpUtils::HasPathParameters(request.target(), 2)) {

                    std::string functionName = Core::HttpUtils::GetPathParameters(request.target())[2];

                    Dto::Lambda::GetFunctionResponse lambdaResponse = _lambdaService.GetFunction(region, functionName);
                    log_trace << "Lambda function region: " << region << " name: " << functionName;
                    return SendOkResponse(request, lambdaResponse.ToJson());
                }
                Dto::Lambda::ListFunctionResponse lambdaResponse = _lambdaService.ListFunctions(region);
                log_trace << "Lambda function list: " << lambdaResponse.ToJson();
                return SendOkResponse(request, lambdaResponse.ToJson());
            }
            if (action == "tags") {

                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                Dto::Lambda::ListTagsResponse lambdaResponse = _lambdaService.ListTags(arn);
                log_trace << "Lambda tag list";
                return SendOkResponse(request, lambdaResponse.ToJson());
            }
            if (action == "account-settings") {

                Dto::Lambda::AccountSettingsResponse lambdaResponse = _lambdaService.GetAccountSettings();
                log_trace << "Lambda account settings";
                return SendOkResponse(request, lambdaResponse.ToJson());
            }
            return SendBadRequestError(request, "Unknown method");

        } catch (Core::ServiceException &exc) {
            return Core::HttpUtils::InternalServerError(request, exc.message());
        } catch (Core::NotFoundException &exc) {
            return SendInternalServerError(request, exc.message());
        }
    }

    http::response<http::dynamic_body> LambdaHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Lambda POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::LambdaClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            std::map<std::string, std::string> headers = Core::HttpUtils::GetHeaders(request);

            std::string version, action;
            Core::HttpUtils::GetVersionAction(request.target(), version, action);

            if (action == "functions") {

                std::string body = Core::HttpUtils::GetBodyAsString(request);

                if (Core::HttpUtils::GetPathParameter(request.target(), 3) == "invocations") {

                    std::string logType = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Log-Type");

                    std::string functionName = Core::HttpUtils::GetPathParameter(request.target(), 2);
                    log_debug << "Lambda function invocation, name: " << functionName;

                    _lambdaService.InvokeLambdaFunction(region, functionName, body);
                    log_info << "Lambda function invoked, name: " << functionName;
                    return SendOkResponse(request);
                }
                Dto::Lambda::CreateFunctionRequest lambdaRequest;
                lambdaRequest.FromJson(body);
                lambdaRequest.region = region;
                lambdaRequest.user = user;

                Dto::Lambda::CreateFunctionResponse lambdaResponse = _lambdaService.CreateFunction(lambdaRequest);
                log_info << "Lambda function created, name: " << lambdaResponse.functionName;

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (action == "tags") {

                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                std::string body = Core::HttpUtils::GetBodyAsString(request);
                Dto::Lambda::CreateTagRequest lambdaRequest;
                lambdaRequest.FromJson(body);

                _lambdaService.CreateTag(lambdaRequest);
                log_info << "Lambda tag created, name: " << lambdaRequest.arn;

                return SendNoContentResponse(request);
            }

            if (action == "event-source-mappings") {

                std::string body = Core::HttpUtils::GetBodyAsString(request);
                Dto::Lambda::CreateEventSourceMappingsRequest lambdaRequest;
                lambdaRequest.FromJson(body);
                lambdaRequest.region = region;
                lambdaRequest.user = user;

                Dto::Lambda::CreateEventSourceMappingsResponse lambdaResponse = _lambdaService.CreateEventSourceMappings(lambdaRequest);
                log_info << "Lambda event source mapping created, name: " << lambdaRequest.functionName;

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_LAMBDA_COUNTERS) {

                Dto::Lambda::ListFunctionCountersRequest lambdaRequest = Dto::Lambda::ListFunctionCountersRequest::FromJson(clientCommand.payload);

                Dto::Lambda::ListFunctionCountersResponse lambdaResponse = _lambdaService.ListFunctionCounters(lambdaRequest);
                log_trace << "Lambda function counters list, count: " << lambdaResponse.functionCounters.size();

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::GET_FUNCTION_COUNTERS) {

                Dto::Lambda::GetFunctionCountersRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                Dto::Lambda::GetFunctionCountersResponse lambdaResponse = _lambdaService.GetFunctionCounters(lambdaRequest);
                log_trace << "Lambda function counters list";

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_ENVIRONMENT_COUNTERS) {

                Dto::Lambda::ListLambdaEnvironmentCountersRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                Dto::Lambda::ListLambdaEnvironmentCountersResponse lambdaResponse = _lambdaService.ListLambdaEnvironmentCounters(lambdaRequest);
                log_trace << "Lambda environment counters list, count: " << lambdaResponse.environmentCounters.size();

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ADD_ENVIRONMENT) {

                Dto::Lambda::AddFunctionEnvironmentRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.AddLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment added, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPDATE_ENVIRONMENT) {

                Dto::Lambda::UpdateFunctionEnvironmentRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.UpdateLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment updated, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_ENVIRONMENT) {

                Dto::Lambda::DeleteFunctionEnvironmentRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.DeleteLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment deleted, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_TAG_COUNTERS) {

                Dto::Lambda::ListLambdaTagCountersRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                Dto::Lambda::ListLambdaTagCountersResponse lambdaResponse = _lambdaService.ListLambdaTagCounters(lambdaRequest);
                log_trace << "Lambda tag counters list";

                return SendOkResponse(request, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ADD_TAG) {

                Dto::Lambda::AddFunctionTagRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.AddLambdaTag(lambdaRequest);
                log_trace << "Lambda tag added";

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPDATE_TAG) {

                Dto::Lambda::UpdateFunctionTagRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.UpdateLambdaTag(lambdaRequest);
                log_trace << "Lambda tag added";

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_TAG) {

                Dto::Lambda::DeleteFunctionTagRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.DeleteLambdaTag(lambdaRequest);
                log_trace << "Lambda tag deleted";

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::RESET_FUNCTION_COUNTERS) {

                Dto::Lambda::ResetFunctionCountersRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.ResetFunctionCounters(lambdaRequest);
                log_trace << "Reset function counters list";

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPLOAD_FUNCTION_CODE) {

                Dto::Lambda::UploadFunctionCodeRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.UploadFunctionCode(lambdaRequest);
                log_trace << "Upload function code, functionArn: " << lambdaRequest.functionArn;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::START_FUNCTION) {

                Dto::Lambda::StartFunctionRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.StartFunction(lambdaRequest);
                log_trace << "Start lambda function, functionArn: " << lambdaRequest.functionArn;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::STOP_FUNCTION) {

                Dto::Lambda::StopFunctionRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.StopFunction(lambdaRequest);
                log_trace << "Stop lambda function, functionArn: " << lambdaRequest.functionArn;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_IMAGE) {

                Dto::Lambda::DeleteImageRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.DeleteImage(lambdaRequest);
                log_trace << "Delete image, functionArn: " << lambdaRequest.functionArn;

                return SendOkResponse(request);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_LAMBDA) {

                Dto::Lambda::DeleteFunctionRequest lambdaRequest;
                lambdaRequest.FromJson(clientCommand.payload);

                _lambdaService.DeleteFunction(lambdaRequest);
                log_trace << "Delete function, functionName: " << lambdaRequest.functionName;

                return SendOkResponse(request);
            }

            log_error << "Unknown method";
            return SendBadRequestError(request, "Unknown method");

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }

    http::response<http::dynamic_body> LambdaHandler::HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Lambda DELETE request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::LambdaClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            std::string version, action;
            Core::HttpUtils::GetVersionAction(request.target(), version, action);
            std::string body = Core::HttpUtils::GetBodyAsString(request);

            if (action == "functions") {

                std::string functionName = Core::HttpUtils::GetPathParameter(request.target(), 2);
                std::string qualifier = Core::HttpUtils::GetPathParameter(request.target(), 3);
                log_debug << "Found lambda name, name: " << functionName << " qualifier: " << qualifier;

                Dto::Lambda::DeleteFunctionRequest lambdaRequest = {.region = region, .functionName = functionName, .qualifier = qualifier};
                _lambdaService.DeleteFunction(lambdaRequest);
                return SendNoContentResponse(request);
            }
            if (action == "tags") {

                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                std::vector<std::string> tagKeys = Core::HttpUtils::GetQueryParametersByPrefix(request.target(),
                                                                                               "tagKeys");
                log_trace << "Got tags count: " << tagKeys.size();

                Dto::Lambda::DeleteTagsRequest lambdaRequest(arn, tagKeys);
                _lambdaService.DeleteTags(lambdaRequest);
                return SendNoContentResponse(request);
            }
            log_error << "Unknown method";
            return SendBadRequestError(request, "Unknown method");

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }

}// namespace AwsMock::Service
