

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
                    return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
                }

                case Dto::Common::LambdaCommandType::WAIT_LAMBDA: {
                    std::string body = Core::HttpUtils::GetBodyAsString(request);

                    std::string functionName = Core::HttpUtils::GetPathParameters(request.target())[2];
                    Dto::Lambda::GetFunctionResponse lambdaResponse = _lambdaService.GetFunction(region, functionName);
                    std::map<std::string, std::string> headers;
                    headers["Content-Length"] = std::to_string(lambdaResponse.ToJson().length());
                    headers["Content-Type"] = "application/xml";
                    return SendResponse(request, http::status::ok, lambdaResponse.ToJson(), headers);
                }

                case Dto::Common::LambdaCommandType::LIST_EVENT_SOURCE_MAPPINGS: {
                    std::string functionName = Core::HttpUtils::GetStringParameter(request.target(), "FunctionName");
                    std::string eventSourceArn = Core::HttpUtils::GetStringParameter(request.target(), "EventSourceArn");
                    std::string marker = Core::HttpUtils::GetStringParameter(request.target(), "Marker");
                    int maxItems = Core::HttpUtils::GetIntParameter(request.target(), "MaxItems", 0, 1000, 1000);

                    Dto::Lambda::ListEventSourceMappingsRequest lambdaRequest;
                    lambdaRequest.functionName = functionName;
                    lambdaRequest.eventSourceArn = eventSourceArn;
                    lambdaRequest.marker = marker;
                    lambdaRequest.maxItems = maxItems;
                    lambdaRequest.region = clientCommand.region;
                    lambdaRequest.user = clientCommand.user;
                    lambdaRequest.requestId = clientCommand.requestId;

                    Dto::Lambda::ListEventSourceMappingsResponse lambdaResponse = _lambdaService.ListEventSourceMappings(lambdaRequest);
                    log_info << "Event source mappings" << lambdaResponse;
                    return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
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
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }
            if (action == "tags") {
                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                Dto::Lambda::ListTagsResponse lambdaResponse = _lambdaService.ListTags(arn);
                log_trace << "Lambda tag list";
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }
            if (action == "account-settings") {
                Dto::Lambda::AccountSettingsResponse lambdaResponse = _lambdaService.GetAccountSettings();
                log_trace << "Lambda account settings";
                return SendOkResponse(request, lambdaResponse.ToJson());
            }
            return SendResponse(request, http::status::bad_request, "Unknown method");
        } catch (Core::ServiceException &exc) {
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::NotFoundException &exc) {
            return SendResponse(request, http::status::internal_server_error, exc.message());
        }
    }

    http::response<http::dynamic_body> LambdaHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Lambda POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::LambdaClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            std::string version, action;
            Core::HttpUtils::GetVersionAction(request.target(), version, action);

            if (action == "functions") {
                std::string body = Core::HttpUtils::GetBodyAsString(request);

                if (Core::HttpUtils::GetPathParameter(request.target(), 3) == "invocations") {
                    auto functionName = Core::HttpUtils::GetPathParameter(request.target(), 2);
                    auto logType = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Log-Type");
                    auto invocationType = Dto::Lambda::LambdaInvocationTypeFromString(Core::HttpUtils::GetHeaderValue(request, "X-Amz-Invocation-Type"));

                    log_info << "Lambda function invoked, name: " << functionName << ", type: " << LambdaInvocationTypeToString(invocationType);
                    Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, functionName, body, invocationType);
                    if (invocationType == Dto::Lambda::LambdaInvocationType::REQUEST_RESPONSE) {
                        return SendResponse(request, http::int_to_status(result.status), result.responseBody);
                    }
                    return SendResponse(request, http::status::ok);
                }
                Dto::Lambda::CreateFunctionRequest lambdaRequest = Dto::Lambda::CreateFunctionRequest::FromJson(body);
                lambdaRequest.region = region;
                lambdaRequest.user = user;

                Dto::Lambda::CreateFunctionResponse lambdaResponse = _lambdaService.CreateFunction(lambdaRequest);
                log_info << "Lambda function created, name: " << lambdaResponse.functionName;

                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (action == "tags") {
                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                std::string body = Core::HttpUtils::GetBodyAsString(request);
                Dto::Lambda::CreateTagRequest lambdaRequest;
                lambdaRequest.FromJson(body);

                _lambdaService.CreateTag(lambdaRequest);
                log_info << "Lambda tag created, name: " << lambdaRequest.arn;

                return SendResponse(request, http::status::no_content);
            }

            if (action == "event-source-mappings") {
                std::string body = Core::HttpUtils::GetBodyAsString(request);
                Dto::Lambda::CreateEventSourceMappingsRequest lambdaRequest = Dto::Lambda::CreateEventSourceMappingsRequest::FromJson(body);
                Dto::Lambda::CreateEventSourceMappingsResponse lambdaResponse = _lambdaService.CreateEventSourceMappings(lambdaRequest);
                log_info << "Lambda event source mapping created, name: " << lambdaRequest.functionName;

                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_LAMBDA_COUNTERS) {
                Dto::Lambda::ListFunctionCountersRequest lambdaRequest = Dto::Lambda::ListFunctionCountersRequest::FromJson(clientCommand.payload);
                Dto::Lambda::ListFunctionCountersResponse lambdaResponse = _lambdaService.ListFunctionCounters(lambdaRequest);
                log_info << "Lambda function counters list, count: " << lambdaResponse.functionCounters.size();
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::GET_FUNCTION_COUNTERS) {
                Dto::Lambda::GetFunctionCountersRequest lambdaRequest = Dto::Lambda::GetFunctionCountersRequest::FromJson(clientCommand);
                Dto::Lambda::GetFunctionCountersResponse lambdaResponse = _lambdaService.GetFunctionCounters(lambdaRequest);
                log_info << "Get lambda function counter, name: " << lambdaResponse.functionName;
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_ENVIRONMENT_COUNTERS) {
                Dto::Lambda::ListLambdaEnvironmentCountersRequest lambdaRequest = Dto::Lambda::ListLambdaEnvironmentCountersRequest::FromJson(clientCommand);
                Dto::Lambda::ListLambdaEnvironmentCountersResponse lambdaResponse = _lambdaService.ListLambdaEnvironmentCounters(lambdaRequest);
                log_trace << "Lambda environment counters list, count: " << lambdaResponse.environmentCounters.size();
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ADD_ENVIRONMENT) {
                Dto::Lambda::AddEnvironmentRequest lambdaRequest = Dto::Lambda::AddEnvironmentRequest::FromJson(clientCommand);
                _lambdaService.AddLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment added, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPDATE_ENVIRONMENT) {
                Dto::Lambda::UpdateFunctionEnvironmentRequest lambdaRequest = Dto::Lambda::UpdateFunctionEnvironmentRequest::FromJson(clientCommand);
                _lambdaService.UpdateLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment updated, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_ENVIRONMENT) {
                Dto::Lambda::DeleteEnvironmentRequest lambdaRequest = Dto::Lambda::DeleteEnvironmentRequest::FromJson(clientCommand);
                _lambdaService.DeleteLambdaEnvironment(lambdaRequest);
                log_trace << "Lambda environment deleted, functionArn: " << lambdaRequest.functionArn << ", key: " << lambdaRequest.environmentKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_TAG_COUNTERS) {
                Dto::Lambda::ListLambdaTagCountersRequest lambdaRequest = Dto::Lambda::ListLambdaTagCountersRequest::FromJson(clientCommand);
                Dto::Lambda::ListLambdaTagCountersResponse lambdaResponse = _lambdaService.ListLambdaTagCounters(lambdaRequest);
                log_trace << "Lambda tag counters list";
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ADD_TAG) {
                Dto::Lambda::AddTagRequest lambdaRequest = Dto::Lambda::AddTagRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] { _lambdaService.AddLambdaTag(lambdaRequest); });
                log_info << "Lambda tag added, functionArn: " << lambdaRequest.functionArn << ", tag: " << lambdaRequest.tagKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPDATE_TAG) {
                Dto::Lambda::UpdateFunctionTagRequest lambdaRequest = Dto::Lambda::UpdateFunctionTagRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] { _lambdaService.UpdateLambdaTag(lambdaRequest); });
                log_info << "Lambda tag updated, lambda: " << lambdaRequest.functionArn << ", tag: " << lambdaRequest.tagKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_TAG) {
                Dto::Lambda::DeleteTagRequest lambdaRequest = Dto::Lambda::DeleteTagRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] { _lambdaService.DeleteLambdaTag(lambdaRequest); });
                log_info << "Lambda tag deleted, lambda: " << lambdaRequest.functionArn << ", tag: " << lambdaRequest.tagKey;
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_INSTANCE_COUNTERS) {
                Dto::Lambda::ListLambdaInstanceCountersRequest lambdaRequest = Dto::Lambda::ListLambdaInstanceCountersRequest::FromJson(clientCommand);
                Dto::Lambda::ListLambdaInstanceCountersResponse lambdaResponse = _lambdaService.ListLambdaInstanceCounters(lambdaRequest);
                log_trace << "Lambda instance counters list, lambdaArn: " << lambdaRequest.lambdaArn;
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::RESET_FUNCTION_COUNTERS) {
                Dto::Lambda::ResetFunctionCountersRequest lambdaRequest = Dto::Lambda::ResetFunctionCountersRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] { _lambdaService.ResetFunctionCounters(lambdaRequest); });
                log_info << "Reset function counters list";
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPLOAD_FUNCTION_CODE) {
                Dto::Lambda::UploadFunctionCodeRequest lambdaRequest = Dto::Lambda::UploadFunctionCodeRequest::FromJson(clientCommand);
                log_info << "Starting upload function code, functionArn: " << lambdaRequest.functionArn;
                _lambdaService.UploadFunctionCode(lambdaRequest);
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::UPDATE_LAMBDA) {
                Dto::Lambda::UpdateLambdaRequest lambdaRequest = Dto::Lambda::UpdateLambdaRequest::FromJson(clientCommand);
                log_info << "Starting update lambda function, functionArn: " << lambdaRequest.functionArn;
                boost::asio::post(_ioc, [this, lambdaRequest] { _lambdaService.UpdateLambda(lambdaRequest); });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_ARNS) {
                Dto::Lambda::ListLambdaArnsResponse lambdaResponse = _lambdaService.ListLambdaArns();
                log_info << "List function ARNs, count: " << lambdaResponse.lambdaArns.size();
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_LAMBDA_RESULT_COUNTERS) {
                Dto::Lambda::ListLambdaResultCountersRequest lambdaRequest = Dto::Lambda::ListLambdaResultCountersRequest::FromJson(clientCommand);
                Dto::Lambda::ListLambdaResultCountersResponse lambdaResponse = _lambdaService.ListLambdaResultCounters(lambdaRequest);
                log_trace << "Lambda result counters list";

                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::GET_LAMBDA_RESULT_COUNTER) {
                Dto::Lambda::GetLambdaResultCounterRequest lambdaRequest = Dto::Lambda::GetLambdaResultCounterRequest::FromJson(clientCommand);
                Dto::Lambda::GetLambdaResultCounterResponse lambdaResponse = _lambdaService.GetLambdaResultCounter(lambdaRequest);
                log_trace << "Get lambda result counter, oid: " << lambdaRequest.oid;
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_LAMBDA_RESULT_COUNTER) {
                Dto::Lambda::DeleteLambdaResultCounterRequest lambdaRequest = Dto::Lambda::DeleteLambdaResultCounterRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    const long count = _lambdaService.DeleteLambdaResultCounter(lambdaRequest);
                    log_trace << "Delete lambda result counter, count: " << count;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_LAMBDA_RESULT_COUNTERS) {
                Dto::Lambda::DeleteLambdaResultCountersRequest lambdaRequest = Dto::Lambda::DeleteLambdaResultCountersRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    const long count = _lambdaService.DeleteLambdaResultCounters(lambdaRequest);
                    log_trace << "Delete lambda result counters, count: " << count;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::LIST_EVENT_SOURCE_COUNTERS) {
                Dto::Lambda::ListLambdaEventSourceCountersRequest lambdaRequest = Dto::Lambda::ListLambdaEventSourceCountersRequest::FromJson(clientCommand);
                Dto::Lambda::ListLambdaEventSourceCountersResponse lambdaResponse = _lambdaService.ListLambdaEventSourceCounters(lambdaRequest);
                log_trace << "Lambda event source counters list, count: " << lambdaResponse.eventSourceCounters.size();
                return SendResponse(request, http::status::ok, lambdaResponse.ToJson());
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ADD_EVENT_SOURCE_COUNTER) {
                Dto::Lambda::AddEventSourceRequest lambdaRequest = Dto::Lambda::AddEventSourceRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.AddEventSource(lambdaRequest);
                    log_trace << "Add event source, functionArn: " << lambdaRequest.functionArn;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_EVENT_SOURCE_COUNTER) {
                Dto::Lambda::DeleteEventSourceRequest lambdaRequest = Dto::Lambda::DeleteEventSourceRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.DeleteEventSource(lambdaRequest);
                    log_trace << "Delete event source, functionArn: " << lambdaRequest.functionArn;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ENABLE_LAMBDA) {
                Dto::Lambda::EnableLambdaRequest lambdaRequest = Dto::Lambda::EnableLambdaRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.EnableLambda(lambdaRequest);
                    log_info << "Lambda enabled, region: " << lambdaRequest.region << ", name: " << lambdaRequest.function.functionName;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::ENABLE_ALL_LAMBDAS) {
                Dto::Lambda::EnableAllLambdasRequest lambdaRequest = Dto::Lambda::EnableAllLambdasRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.EnableAllLambdas(lambdaRequest);
                    log_info << "All lambdas enabled, region: " << lambdaRequest.region;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DISABLE_LAMBDA) {
                Dto::Lambda::DisableLambdaRequest lambdaRequest = Dto::Lambda::DisableLambdaRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.DisableLambda(lambdaRequest);
                    log_info << "Lambda disabled, region: " << lambdaRequest.region << ", name: " << lambdaRequest.function.functionName;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DISABLE_ALL_LAMBDAS) {
                Dto::Lambda::DisableAllLambdasRequest lambdaRequest = Dto::Lambda::DisableAllLambdasRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.DisableAllLambdas(lambdaRequest);
                    log_info << "All lambdas disabled, region: " << lambdaRequest.region;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::START_LAMBDA) {
                Dto::Lambda::StartLambdaRequest lambdaRequest = Dto::Lambda::StartLambdaRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.StartLambda(lambdaRequest);
                    log_trace << "Start lambda function, functionArn: " << lambdaRequest.functionArn;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::START_ALL_LAMBDAS) {
                boost::asio::post(_ioc, [this] {
                    _lambdaService.StartAllLambdas();
                    log_trace << "Started all lambda function";
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::STOP_LAMBDA) {
                Dto::Lambda::StopLambdaRequest lambdaRequest = Dto::Lambda::StopLambdaRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.StopLambda(lambdaRequest);
                    log_trace << "Stop lambda function, functionArn: " << lambdaRequest.functionArn;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::STOP_ALL_LAMBDAS) {
                boost::asio::post(_ioc, [this] {
                    _lambdaService.StopAllLambdas();
                    log_trace << "Stopped all lambda functions";
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::STOP_LAMBDA_INSTANCE) {
                Dto::Lambda::StopLambdaInstanceRequest lambdaRequest = Dto::Lambda::StopLambdaInstanceRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.StopLambdaInstance(lambdaRequest);
                    log_trace << "Stop lambda instance, functionArn: " << lambdaRequest.functionArn << ", instanceId: " << lambdaRequest.instanceId;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_IMAGE) {
                Dto::Lambda::DeleteImageRequest lambdaRequest = Dto::Lambda::DeleteImageRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.DeleteImage(lambdaRequest);
                    log_trace << "Delete image, functionArn: " << lambdaRequest.functionArn;
                });
                return SendResponse(request, http::status::ok);
            }

            if (clientCommand.command == Dto::Common::LambdaCommandType::DELETE_LAMBDA) {
                Dto::Lambda::DeleteFunctionRequest lambdaRequest = Dto::Lambda::DeleteFunctionRequest::FromJson(clientCommand);
                boost::asio::post(_ioc, [this, lambdaRequest] {
                    _lambdaService.DeleteFunction(lambdaRequest);
                    log_trace << "Delete function, functionName: " << lambdaRequest.functionName;
                });
                return SendResponse(request, http::status::ok);
            }

            log_error << "Unknown method";
            return SendResponse(request, http::status::bad_request, "Unknown method");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

    http::response<http::dynamic_body> LambdaHandler::HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Lambda DELETE request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::LambdaClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            std::string version, action;
            Core::HttpUtils::GetVersionAction(request.target(), version, action);

            if (action == "functions") {
                std::string functionName = Core::HttpUtils::GetPathParameter(request.target(), 2);
                std::string qualifier = Core::HttpUtils::GetPathParameter(request.target(), 3);
                log_debug << "Found lambda name, name: " << functionName << " qualifier: " << qualifier;

                Dto::Lambda::DeleteFunctionRequest lambdaRequest;
                lambdaRequest.region = region;
                lambdaRequest.functionName = functionName;
                lambdaRequest.qualifier = qualifier;
                _lambdaService.DeleteFunction(lambdaRequest);
                return SendResponse(request, http::status::no_content);
            }
            if (action == "tags") {
                std::string arn = Core::HttpUtils::GetPathParameter(request.target(), 2);
                log_debug << "Found lambda arn, arn: " << arn;

                std::vector<std::string> tagKeys = Core::HttpUtils::GetQueryParametersByPrefix(request.target(),
                                                                                               "tagKeys");
                log_trace << "Got tags count: " << tagKeys.size();

                Dto::Lambda::DeleteTagsRequest lambdaRequest(arn, tagKeys);
                _lambdaService.DeleteTags(lambdaRequest);
                return SendResponse(request, http::status::no_content);
            }
            log_error << "Unknown method";
            return SendResponse(request, http::status::bad_request, "Unknown method");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }
}// namespace AwsMock::Service
