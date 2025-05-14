//
// Created by vogje01 on 4/21/24.
//

#include <awsmock/service/secretsmanager/SecretsManagerMonitoring.h>

namespace AwsMock::Service {

    void SecretsManagerMonitoring::UpdateCounter() {
        long secrets = _secretsManagerDatabase.CountSecrets();
        _metricService.SetGauge(SECRETSMANAGER_SECRETS_COUNT, {}, {}, static_cast<double>(secrets));
        log_trace << "Secrets manager update counter finished";
    }
}// namespace AwsMock::Service