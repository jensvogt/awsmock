#!/bin/bash

lambdaArn="arn:aws:lambda:eu-central-1:000000000000:function:image-publishing-scaling"

# 1. Create the REST API
apiJson=$(awslocal apigateway create-rest-api --name test-api)
restApiId=$(echo "$apiJson" | jq -r .id)
echo "REST API ID: $restApiId"

# 2. Get the root resource ID (path == "/")
rootResourceId=$(awslocal apigateway get-resources --rest-api-id "$restApiId" | jq -r '.items[] | select(.path == "/") | .id')
echo "Root resource ID: $rootResourceId"

# 3. Attach a GET method to the root resource "/"
awslocal apigateway put-method --rest-api-id "$restApiId" --resource-id "$rootResourceId" --http-method GET \
  --authorization-type NONE

# 4. Point the method at the Lambda function
awslocal apigateway put-integration --rest-api-id "$restApiId" --resource-id "$rootResourceId" --http-method GET \
  --type AWS_PROXY --integration-http-method POST \
  --uri "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/${lambdaArn}/invocations"

# call it: /restapis/{restApiId}/{stage}
# stage=small -> event.path='/' -> ScalingParams{width:600, height:600}
# imageName passed as query parameter
#curl -s "http://localhost:4566/restapis/${restApiId}/small?imageName=9783911244381"
