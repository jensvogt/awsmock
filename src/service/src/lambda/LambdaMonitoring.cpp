//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/LambdaMonitoring.h>

namespace AwsMock::Service {

  LambdaMonitoring::LambdaMonitoring(const Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition)
      : _logger(Poco::Logger::get("LambdaMonitoring")), _configuration(configuration), _metricService(metricService), _condition(condition), _running(false) {

    // Update period
    _period = _configuration.getInt("awsmock.monitoring.lambda.period", LAMBDA_MONITORING_DEFAULT_PERIOD);

    // Database connections
    _lambdaDatabase = std::make_unique<Database::LambdaDatabase>(_configuration);
    log_debug_stream(_logger) << "lambda monitoring initialized" << std::endl;
  }

  void LambdaMonitoring::run() {

    log_info_stream(_logger) << "lambda monitoring started" << std::endl;

    _running = true;
    while (_running) {

      _logger.debug() << "lambda monitoring processing started" << std::endl;

      // Update counters
      UpdateCounters();

      // Wait for timeout or condition
      _mutex.lock();
      if (_condition.tryWait(_mutex, _period)) {
        break;
      }
      _mutex.unlock();
    }
  }

  void LambdaMonitoring::UpdateCounters() {
    long lambdas = _lambdaDatabase->LambdaCount();
    _metricService.SetGauge("lambda_count_total", lambdas);
  }

} // namespace AwsMock::Service