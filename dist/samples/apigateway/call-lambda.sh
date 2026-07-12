#!/bin/bash

lambdaArn="arn:aws:lambda:eu-central-1:000000000000:function:image-publishing-scaling"
authorizerLambdaArn="arn:aws:lambda:eu-central-1:000000000000:function:image-publishing-authorizer"
stageName="small"
restApiName="test-api"
restApiKeyName="test-api-key"

# 0. Cleanup: delete any existing API key with the same name so the script is reentrant
existingKeyId=$(awslocal apigateway get-api-keys 2>/dev/null \
  | jq -r --arg n "$restApiKeyName" '.items[] | select(.name == $n) | .id' 2>/dev/null)
if [ -n "$existingKeyId" ]; then
    awslocal apigateway delete-api-key --api-key "$existingKeyId"
    echo "Deleted existing API key: $existingKeyId"
fi

# 1. Create the REST API
apiJson=$(awslocal apigateway create-rest-api --name "$restApiName")
restApiId=$(echo "$apiJson" | jq -r .id)
echo "REST API ID: $restApiId"

# 2. Get the root resource ID (path == "/")
rootResourceId=$(awslocal apigateway get-resources --rest-api-id "$restApiId" | jq -r '.items[] | select(.path == "/") | .id')
echo "Root resource ID: $rootResourceId"

# 3. Create a Lambda authorizer
authorizerJson=$(awslocal apigateway create-authorizer --rest-api-id "$restApiId" \
  --name "image-publishing-authorizer" \
  --type TOKEN \
  --authorizer-uri "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/${authorizerLambdaArn}/invocations" \
  --identity-source "method.request.header.Authorization" \
  --authorizer-result-ttl-in-seconds 300)
authorizerId=$(echo "$authorizerJson" | jq -r .id)
echo "Authorizer ID: $authorizerId"

# 4. Attach a GET method to the root resource "/" (API key required, custom authorizer)
awslocal apigateway put-method --rest-api-id "$restApiId" --resource-id "$rootResourceId" --http-method GET \
  --authorization-type CUSTOM --authorizer-id "$authorizerId" --api-key-required

# 5. Point the method at the Lambda function
awslocal apigateway put-integration --rest-api-id "$restApiId" --resource-id "$rootResourceId" --http-method GET \
  --type AWS_PROXY --integration-http-method POST \
  --uri "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/${lambdaArn}/invocations"

# 6. Deploy the API to a stage (required so the usage plan can reference it)
deploymentId=$(awslocal apigateway create-deployment --rest-api-id "$restApiId" --stage-name "$stageName" | jq -r .id)
echo "Deployment ID: $deploymentId"

# 7. Create a usage plan linked to the stage
usagePlanJson=$(awslocal apigateway create-usage-plan --name test-usage-plan \
  --api-stages "apiId=${restApiId},stage=${stageName}" --throttle "burstLimit=100,rateLimit=50" \
  --quota "limit=10000,period=MONTH")
usagePlanId=$(echo "$usagePlanJson" | jq -r .id)
echo "Usage Plan ID: $usagePlanId"

# 8. Create an API key
apiKeyJson=$(awslocal apigateway create-api-key --name "$restApiKeyName" --enabled)
apiKeyId=$(echo "$apiKeyJson" | jq -r .id)
apiKeyValue=$(echo "$apiKeyJson" | jq -r .value)
echo "API Key ID:    $apiKeyId"
echo "API Key Value: $apiKeyValue"

# 9. Associate the API key with the usage plan
awslocal apigateway create-usage-plan-key --usage-plan-id "$usagePlanId" --key-id "$apiKeyId" --key-type API_KEY
echo "API key linked to usage plan"

# call it: /restapis/{restApiId}/{stage}
# stage=small -> event.path='/' -> ScalingParams{width:600, height:600}
# imageName passed as query parameter
#curl -s -H "x-api-key: ${apiKeyValue}" "http://localhost:4566/restapis/${restApiId}/${stageName}?imageName=9783911244381"
