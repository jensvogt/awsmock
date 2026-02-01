//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Lambda.h>

namespace AwsMock::Database::Entity::Lambda {

    bool Lambda::HasEventSource(const std::string &eventSourceArn) const {
        return std::ranges::find_if(eventSources, [eventSourceArn](const EventSourceMapping &e) {
                   return e.eventSourceArn == eventSourceArn;
               }) != eventSources.end();
    }

    EventSourceMapping Lambda::GetEventSource(const std::string &eventSourceArn) const {
        const auto it = std::ranges::find_if(eventSources, [eventSourceArn](const EventSourceMapping &e) {
            return e.eventSourceArn == eventSourceArn;
        });
        if (it != eventSources.end()) {
            return *it;
        }
        return {};
    }

    bool Lambda::HasTag(const std::string &key) const {
        return std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
                   return t.first == key;
               }) != tags.end();
    }

    std::string Lambda::GetTagValue(const std::string &key) const {
        const auto it = std::ranges::find_if(tags, [key](const std::pair<std::string, std::string> &t) {
            return t.first == key;
        });
        return it->second;
    }

    bool Lambda::HasInstance(const std::string &instanceId) {
        const auto it = std::ranges::find_if(instances, [&](const Instance &obj) { return obj.instanceId == instanceId; });
        return it != instances.end();
    }

    Instance Lambda::GetInstance(const std::string &instanceId) {
        if (const auto &it = std::ranges::find_if(instances, [&instanceId](const Instance &obj) { return obj.instanceId == instanceId; }); it != instances.end()) {
            return *it;
        }
        log_error << "Lambda instance not found, id: " << instanceId;
        return {};
    }

    void Lambda::SetInstanceHostPort(const std::string &instanceId, const std::string &host, const int port) {
        for (auto &instance: instances) {
            if (instanceId == instance.instanceId) {
                instance.hostName = host;
                instance.publicPort = port;
                return;
            }
        }
        log_error << "Lambda instance not found, id: " << instanceId;
    }

    void Lambda::SetInstanceLastInvocation(const std::string &instanceId) {
        for (auto &instance: instances) {
            if (instanceId == instance.instanceId) {
                instance.lastInvocation = system_clock::now();
                return;
            }
        }
        log_error << "Lambda instance not found, id: " << instanceId;
    }

    void Lambda::SetInstanceStatus(const std::string &instanceId, const LambdaInstanceStatus &status) {
        for (auto &instance: instances) {
            if (instanceId == instance.instanceId) {
                instance.status = status;
                return;
            }
        }
        log_error << "Lambda instance not found, id: " << instanceId;
    }

    void Lambda::RemoveInstance(const Instance &instance) {
        RemoveInstance(instance.instanceId);
    }

    void Lambda::RemoveInstance(const std::string &instanceId) {
        instances.erase(std::ranges::remove_if(instances, [instanceId](const Instance &i) { return instanceId == i.instanceId; }).begin(), instances.end());
    }

    void Lambda::RemoveInstanceByContainerId(const std::string &containerId) {
        instances.erase(std::ranges::remove_if(instances, [containerId](const Instance &i) { return containerId == i.containerId; }).begin(), instances.end());
    }

    view_or_value<view, value> Lambda::ToDocument() const {

        // Convert instances
        auto instancesDoc = array{};
        for (const auto &instance: instances) {
            instancesDoc.append(instance.ToDocument());
        }

        // Convert environment to document
        auto variablesDoc = document{};
        for (const auto &[fst, snd]: environment.variables) {
            variablesDoc.append(kvp(fst, snd));
        }
        document varDoc;
        varDoc.append(kvp("variables", variablesDoc));

        // Convert tags to document
        auto tagsDoc = document{};
        for (const auto &[fst, snd]: tags) {
            tagsDoc.append(kvp(fst, snd));
        }

        // Convert event source mappings
        auto eventSourcesDoc = array{};
        for (const auto &e: eventSources) {
            eventSourcesDoc.append(e.ToDocument());
        }

        document ephemeralStorageDoc;
        Core::Bson::BsonUtils::SetLongValue(ephemeralStorageDoc, "size", ephemeralStorage.size);

        document lambdaDoc;
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "region", region);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "arn", arn);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "user", user);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "function", function);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "runtime", runtime);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "role", role);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "handler", handler);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "imageId", imageId);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "memorySize", memorySize);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "codeSize", codeSize);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "imageSize", imageSize);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "containerId", containerId);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "containerSize", containerSize);
        Core::Bson::BsonUtils::SetDocumentValue(lambdaDoc, "tags", tagsDoc);
        Core::Bson::BsonUtils::SetDocumentValue(lambdaDoc, "ephemeralStorage", ephemeralStorageDoc);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "timeout", timeout);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "concurrency", concurrency);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "codeSha256", codeSha256);
        Core::Bson::BsonUtils::SetDocumentValue(lambdaDoc, "environment", varDoc);
        Core::Bson::BsonUtils::SetDocumentValue(lambdaDoc, "code", code.ToDocument());
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "state", LambdaStateToString(state));
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "stateReason", stateReason);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "stateReasonCode", LambdaStateReasonCodeToString(stateReasonCode));
        Core::Bson::BsonUtils::SetArrayValue(lambdaDoc, "instances", instancesDoc);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "invocations", invocations);
        Core::Bson::BsonUtils::SetLongValue(lambdaDoc, "averageRuntime", averageRuntime);
        Core::Bson::BsonUtils::SetStringValue(lambdaDoc, "dockerTag", dockerTag);
        Core::Bson::BsonUtils::SetBoolValue(lambdaDoc, "enabled", enabled);
        Core::Bson::BsonUtils::SetArrayValue(lambdaDoc, "eventSources", eventSourcesDoc);
        Core::Bson::BsonUtils::SetDateValue(lambdaDoc, "lastStarted", lastStarted);
        Core::Bson::BsonUtils::SetDateValue(lambdaDoc, "lastInvocation", lastInvocation);
        Core::Bson::BsonUtils::SetDateValue(lambdaDoc, "created", created);
        Core::Bson::BsonUtils::SetDateValue(lambdaDoc, "modified", modified);

        return lambdaDoc.extract();
    }

    void Lambda::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        user = Core::Bson::BsonUtils::GetStringValue(mResult, "user");
        function = Core::Bson::BsonUtils::GetStringValue(mResult, "function");
        runtime = Core::Bson::BsonUtils::GetStringValue(mResult, "runtime");
        role = Core::Bson::BsonUtils::GetStringValue(mResult, "role");
        handler = Core::Bson::BsonUtils::GetStringValue(mResult, "handler");
        memorySize = Core::Bson::BsonUtils::GetLongValue(mResult, "memorySize");
        ephemeralStorage.FromDocument(mResult.value()["ephemeralStorage"].get_document().value);
        codeSize = Core::Bson::BsonUtils::GetLongValue(mResult, "codeSize");
        imageId = Core::Bson::BsonUtils::GetStringValue(mResult, "imageId");
        imageSize = Core::Bson::BsonUtils::GetLongValue(mResult, "imageSize");
        containerId = Core::Bson::BsonUtils::GetStringValue(mResult, "containerId");
        containerSize = Core::Bson::BsonUtils::GetLongValue(mResult, "containerSize");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        codeSha256 = Core::Bson::BsonUtils::GetStringValue(mResult, "codeSha256");
        timeout = Core::Bson::BsonUtils::GetIntValue(mResult, "timeout");
        concurrency = Core::Bson::BsonUtils::GetIntValue(mResult, "concurrency");
        state = LambdaStateFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "state"));
        stateReason = Core::Bson::BsonUtils::GetStringValue(mResult, "stateReason");
        stateReasonCode = LambdaStateReasonCodeFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "stateReasonCode"));
        lastStarted = Core::Bson::BsonUtils::GetDateValue(mResult.value()["lastStarted"]);
        lastInvocation = Core::Bson::BsonUtils::GetDateValue(mResult.value()["lastInvocation"]);
        invocations = Core::Bson::BsonUtils::GetLongValue(mResult, "invocations");
        averageRuntime = Core::Bson::BsonUtils::GetLongValue(mResult, "averageRuntime");
        dockerTag = Core::Bson::BsonUtils::GetStringValue(mResult, "dockerTag");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Environment
        if (mResult.value().find("environment") != mResult.value().end()) {
            environment.FromDocument(mResult.value()["environment"].get_document().value);
        }

        // Get tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                tags.emplace(key, value);
            }
        }

        // Get code
        if (mResult.value().find("code") != mResult.value().end()) {
            code.FromDocument(mResult.value()["code"].get_document().value);
        }

        // Get instances
        if (mResult.value().find("instances") != mResult.value().end()) {
            instances.clear();
            for (const view instancesView = mResult.value()["instances"].get_array().value; const bsoncxx::document::element &instanceElement: instancesView) {
                Instance instance;
                instance.FromDocument(instanceElement.get_document().view());
                instances.emplace_back(instance);
            }
        }

        // Get event sources
        if (mResult.value().find("eventSources") != mResult.value().end()) {
            eventSources.clear();
            for (const view eventSourcesView = mResult.value()["eventSources"].get_array().value; const bsoncxx::document::element &eventSourceElement: eventSourcesView) {
                EventSourceMapping eventSourceMapping;
                eventSourceMapping.FromDocument(eventSourceElement.get_document().view());
                eventSources.emplace_back(eventSourceMapping);
            }
        }
    }

    std::string Lambda::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Lambda::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Lambda &l) {
        os << "Lambda=" << to_json(l.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Lambda