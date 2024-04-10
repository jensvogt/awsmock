
#include "awsmock/service/sns/SNSCmdHandler.h"

namespace AwsMock::Service {

  SNSCmdHandler::SNSCmdHandler(Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition)
    : AbstractHandler(), _logger(Poco::Logger::get("SNSCliHandler")), _configuration(configuration), _metricService(metricService), _snsService(configuration, condition) {
  }

  void SNSCmdHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::SNSClientCommand &snsClientCommand) {
    log_trace_stream(_logger) << "SNS POST request, URI: " << request.getURI() << " region: " << snsClientCommand.region << " user: " << snsClientCommand.user << " command: " << Dto::Common::SNSCommandTypeToString(snsClientCommand.command) << std::endl;

    std::string requestId = GetHeaderValue(request, "RequestId", Poco::UUIDGenerator().createRandom().toString());

    switch (snsClientCommand.command) {

      case Dto::Common::SNSCommandType::CREATE_TOPIC: {

        std::string name = GetStringParameter(snsClientCommand.payload, "Name");
        log_debug_stream(_logger) << "Topic name: " << name << std::endl;

        Dto::SNS::CreateTopicRequest snsRequest = {.region=snsClientCommand.region, .topicName = name, .owner=snsClientCommand.user};
        Dto::SNS::CreateTopicResponse snsResponse = _snsService.CreateTopic(snsRequest);
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Topic created, name: " << name << std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::LIST_TOPICS:{

        Dto::SNS::ListTopicsResponse snsResponse = _snsService.ListTopics(snsClientCommand.region);
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "List topics" << std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::PUBLISH:{

        std::string topicArn = GetStringParameter(snsClientCommand.payload, "TopicArn");
        std::string targetArn = GetStringParameter(snsClientCommand.payload, "TargetArn");
        std::string message = GetStringParameter(snsClientCommand.payload, "Message");

        Dto::SNS::PublishResponse snsResponse = _snsService.Publish({.region=snsClientCommand.region, .topicArn=topicArn, .targetArn=targetArn, .message=message});
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Message published, topic: " << topicArn <<std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::SUBSCRIBE: {

        std::string topicArn = GetStringParameter(snsClientCommand.payload, "TopicArn");
        std::string protocol = GetStringParameter(snsClientCommand.payload, "Protocol");
        std::string endpoint = GetStringParameter(snsClientCommand.payload, "Endpoint");

        Dto::SNS::SubscribeResponse snsResponse = _snsService.Subscribe({.region=snsClientCommand.region, .topicArn=topicArn, .protocol=protocol, .endpoint=endpoint, .owner=snsClientCommand.user});
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Subscribed to topic, topicArn: " << topicArn <<std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::UNSUBSCRIBE: {

        std::string subscriptionArn = GetStringParameter(snsClientCommand.payload, "SubscriptionArn");

        Dto::SNS::UnsubscribeResponse snsResponse = _snsService.Unsubscribe({.region=snsClientCommand.region, .subscriptionArn=subscriptionArn});
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Unsubscribed from topic, subscriptionArn: " << subscriptionArn <<std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::TAG_RESOURCE: {

        std::string resourceArn = GetStringParameter(snsClientCommand.payload, "ResourceArn");
        log_debug_stream(_logger) << "Resource ARN: " << resourceArn << std::endl;

        int count = Core::HttpUtils::CountQueryParametersByPrefix(snsClientCommand.payload, "Tags.member") / 2;
        log_trace_stream(_logger) << "Got tags count, count: " << count << std::endl;

        std::map<std::string, std::string> tags;
        for (int i = 1; i <= count; i++) {
          std::string tagKey = Core::HttpUtils::GetQueryParameterValueByName(snsClientCommand.payload, "Tags.member." + std::to_string(i) + ".Key");
          std::string tagValue = Core::HttpUtils::GetQueryParameterValueByName(snsClientCommand.payload, "Tags.member." + std::to_string(i) + ".Value");
          tags[tagKey] = tagValue;
        }
        Dto::SNS::TagResourceRequest snsRequest = {.region=snsClientCommand.region, .resourceArn= resourceArn, .tags=tags};
        Dto::SNS::TagResourceResponse snsResponse = _snsService.TagResource(snsRequest);
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Topic tagged, resourceArn: " << resourceArn <<std::endl;

        break;
      }
      case Dto::Common::SNSCommandType::DELETE_TOPIC: {

        std::string topicArn = GetStringParameter(snsClientCommand.payload, "TopicArn");
        log_debug_stream(_logger) << "Topic ARN: " << topicArn << std::endl;

        Dto::SNS::DeleteTopicResponse snsResponse = _snsService.DeleteTopic(snsClientCommand.region, topicArn);
        SendOkResponse(response, snsResponse.ToXml());
        log_info_stream(_logger) << "Topic deleted, topicArn: " << topicArn <<std::endl;

        break;
      }

      case Dto::Common::SNSCommandType::UNKNOWN: {
        log_error_stream(_logger) << "Bad request, method: POST clientCommand: " << Dto::Common::SNSCommandTypeToString(snsClientCommand.command) << std::endl;
        throw Core::ServiceException("Bad request, method: POST clientCommand: " + Dto::Common::SNSCommandTypeToString(snsClientCommand.command));
      }
    }
  }
}