
#include <awsmock/service/LambdaHandler.h>

namespace AwsMock::Service {

  LambdaHandler::LambdaHandler(Core::Configuration &configuration,
                               Core::MetricService &metricService,
                               Poco::NotificationQueue &createQueue,
                               Poco::NotificationQueue &invokeQueue)
    : AbstractHandler(), _logger(Poco::Logger::get("LambdaServiceHandler")), _configuration(configuration), _metricService(metricService),
      _lambdaService(configuration, metricService, createQueue, invokeQueue) {
  }

  void LambdaHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, [[maybe_unused]]const std::string &user) {
    _metricService.IncrementCounter("gateway_get_counter");
    log_trace_stream(_logger) << "lambda GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {

      std::string version, action;
      Core::HttpUtils::GetVersionAction(request.getURI(), version, action);

      if (action == "functions") {

        Dto::Lambda::ListFunctionResponse lambdaResponse = _lambdaService.ListFunctions(region);
        SendOkResponse(response, lambdaResponse.ToJson());

      } else if (action == "tags") {

        std::string arn = Core::HttpUtils::GetPathParameter(request.getURI(), 2);
        log_debug_stream(_logger) << "Found lambda arn, arn: " << arn << std::endl;

        Dto::Lambda::ListTagsResponse lambdaResponse = _lambdaService.ListTags(arn);
        SendOkResponse(response, lambdaResponse.ToJson());
      }

    } catch (Core::ServiceException &exc) {
      SendErrorResponse("lambda", response, exc);
    } catch (Core::ResourceNotFoundException &exc) {
      SendErrorResponse("lambda", response, exc);
    }
  }

  void LambdaHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
    _metricService.IncrementCounter("gateway_put_counter");
    log_trace_stream(_logger) << "lambda PUT request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;

    try {
      std::string version, action;
      Core::HttpUtils::GetVersionAction(request.getURI(), version, action);

    } catch (Poco::Exception &exc) {
      SendErrorResponse("lambda", response, exc);
    }
  }

  void LambdaHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    _metricService.IncrementCounter("gateway_post_counter");
    log_trace_stream(_logger) << "lambda POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {
      std::string tmp = request.getURI();
      std::string version, action;
      Core::HttpUtils::GetVersionAction(request.getURI(), version, action);

      if (action == "functions") {

        std::string body = Core::HttpUtils::GetBodyAsString(request);

        if (Core::HttpUtils::GetPathParameter(request.getURI(), 3) == "invocations") {

          std::string functionName = Core::HttpUtils::GetPathParameter(request.getURI(), 2);
          log_debug_stream(_logger) << "lambda function invocation, name: " << functionName << std::endl;

          _lambdaService.InvokeLambdaFunction(functionName, body, region, user);
          log_debug_stream(_logger) << "lambda function invoked, name: " << functionName << std::endl;
          SendOkResponse(response);

        } else {

          Dto::Lambda::CreateFunctionRequest lambdaRequest;
          lambdaRequest.FromJson(body);
          lambdaRequest.region = region;
          lambdaRequest.user = user;

          Dto::Lambda::CreateFunctionResponse lambdaResponse = _lambdaService.CreateFunction(lambdaRequest);
          SendOkResponse(response, lambdaResponse.ToJson());
        }

      } else if (action == "tags") {

        std::string arn = Core::HttpUtils::GetPathParameter(request.getURI(), 2);
        log_debug_stream(_logger) << "Found lambda arn, arn: " << arn << std::endl;

        std::string body = Core::HttpUtils::GetBodyAsString(request);
        Dto::Lambda::CreateTagRequest lambdaRequest(arn, body);

        _lambdaService.CreateTag(lambdaRequest);
        SendOkResponse(response, {}, Poco::Net::HTTPResponse::HTTP_NO_CONTENT);

      }

    } catch (Poco::Exception &exc) {
      SendErrorResponse("lambda", response, exc);
    }
  }

  void LambdaHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    _metricService.IncrementCounter("gateway_delete_counter");
    log_trace_stream(_logger) << "lambda DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {
      std::string version, action;
      Core::HttpUtils::GetVersionAction(request.getURI(), version, action);
      std::string body = GetBodyAsString(request);

      if (action == "functions") {

        std::string functionName = Core::HttpUtils::GetPathParameter(request.getURI(), 2);
        std::string qualifier = Core::HttpUtils::GetPathParameter(request.getURI(), 3);
        log_debug_stream(_logger) << "Found lambda name, name: " << functionName << " qualifier: " << qualifier << std::endl;

        Dto::Lambda::DeleteFunctionRequest lambdaRequest = {.functionName=functionName, .qualifier=qualifier};
        _lambdaService.DeleteFunction(lambdaRequest);
        SendOkResponse(response);

      } else if (action == "tags") {

        std::string arn = Core::HttpUtils::GetPathParameter(request.getURI(), 2);
        log_debug_stream(_logger) << "Found lambda arn, arn: " << arn << std::endl;

        std::vector<std::string> tagKeys = Core::HttpUtils::GetQueryParametersByPrefix(request.getURI(), "tagKeys");
        log_trace_stream(_logger) << "Got tags count: " << tagKeys.size() << std::endl;

        Dto::Lambda::DeleteTagsRequest lambdaRequest(arn, tagKeys);
        _lambdaService.DeleteTags(lambdaRequest);
        SendNoContentResponse(response);

      }

    } catch (Core::ServiceException &exc) {
      SendErrorResponse("lambda", response, exc);
    }
  }

  void LambdaHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
    _metricService.IncrementCounter("gateway_options_counter");
    log_trace_stream(_logger) << "lambda OPTIONS request" << std::endl;

    response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
    response.setContentType("text/plain; charset=utf-8");

    handleHttpStatusCode(response, 200);
    std::ostream &outputStream = response.send();
    outputStream.flush();
  }

  void LambdaHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    _metricService.IncrementCounter("gateway_head_counter");
    log_trace_stream(_logger) << "lambda HEAD request, address: " << request.clientAddress().toString() << std::endl;

    try {

      std::string version, action;
      GetVersionActionFromUri(request.getURI(), version, action);

      HeaderMap headerMap;
      headerMap["Connection"] = "closed";
      headerMap["Server"] = "AmazonS3";

      SendOkResponse(response, {}, headerMap);

    } catch (Poco::Exception &exc) {
      SendErrorResponse("lambda", response, exc);
    }
  }
}