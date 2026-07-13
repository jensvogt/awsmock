---
title: awsmockapi(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockapi AwsMock API Gateway module
footer: awsmockapi $version$
---

## NAME

```awsmockapi``` — AwsMock API Gateway module

## DESCRIPTION

Amazon API Gateway is an AWS service for creating, publishing, maintaining, monitoring, and securing REST, HTTP, and
WebSocket APIs at any scale. API developers can create APIs that access AWS or other web services, as well as data
stored in the AWS Cloud.

AwsMock implements the API Gateway management plane (REST APIs, resources, methods, integrations, deployments, usage
plans, and API keys) and the execution plane (invoking Lambda-backed endpoints via `GET`/`POST`/`PUT`/`DELETE`/`PATCH`
with optional API-key and Lambda-authorizer validation).

## COMMANDS

### API Keys

```awslocal apigateway create-api-key --name <name> [--description <desc>] [--customer-id <id>] [--enabled | --no-enabled] [--generate-distinct-id | --no-generate-distinct-id] [--tags <key=value,...>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Creates a new API key. The generated key value is returned in the `value` field.

```awslocal apigateway get-api-keys [--name-query <prefix>] [--customer-id <id>] [--include-values | --no-include-values] [--limit <n>] [--position <token>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Returns a (paginated) list of API keys. Pass `--include-values` to include the secret key value in the response.

```awslocal apigateway delete-api-key --api-key <id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Deletes the API key with the given ID.

### REST APIs

```awslocal apigateway create-rest-api --name <name> [--description <desc>] [--version <ver>] [--api-key-source HEADER|AUTHORIZER] [--clone-from <rest-api-id>] [--disable-execute-api-endpoint | --no-disable-execute-api-endpoint] [--minimum-compression-size <bytes>] [--policy <json>] [--binary-media-types <type,...>] [--tags <key=value,...>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Creates a new REST API. Returns the new REST API object including its root resource ID.

```awslocal apigateway get-rest-apis [--limit <n>] [--position <token>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Returns a (paginated) list of all REST APIs.

```awslocal apigateway delete-rest-api --rest-api-id <id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Deletes the REST API and all of its resources, methods, integrations, and deployments.

### Resources

```awslocal apigateway create-resource --rest-api-id <id> --parent-id <parent-resource-id> --path-part <part>```  
&nbsp;&nbsp;&nbsp;&nbsp;Creates a child resource under `parent-id`. Use `{param}` syntax in `path-part` for path-parameter segments (e.g. `{isbn}`).

```awslocal apigateway get-resources --rest-api-id <id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Returns all resources defined for the given REST API.

```awslocal apigateway get-resource --rest-api-id <id> --resource-id <resource-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Returns the resource with the given ID.

```awslocal apigateway delete-resource --rest-api-id <id> --resource-id <resource-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Deletes the resource and its methods and integrations.

### Methods and Integrations

```awslocal apigateway put-method --rest-api-id <id> --resource-id <resource-id> --http-method GET|POST|PUT|DELETE|PATCH|ANY --authorization-type NONE|AWS_IAM|CUSTOM [--api-key-required | --no-api-key-required]```  
&nbsp;&nbsp;&nbsp;&nbsp;Creates or replaces the method configuration on a resource. Set `--api-key-required` to enforce API-key validation on every call.

```awslocal apigateway put-integration --rest-api-id <id> --resource-id <resource-id> --http-method <method> --type AWS_PROXY|AWS|HTTP|HTTP_PROXY|MOCK --uri <uri> [--integration-http-method POST]```  
&nbsp;&nbsp;&nbsp;&nbsp;Attaches a backend integration to the method. For Lambda integrations, `--type AWS_PROXY` and `--integration-http-method POST` are required; `--uri` must be the Lambda invocation ARN in the form `arn:aws:apigateway:<region>:lambda:path/2015-03-31/functions/<function-arn>/invocations`.

```awslocal apigateway get-integration --rest-api-id <id> --resource-id <resource-id> --http-method <method>```  
&nbsp;&nbsp;&nbsp;&nbsp;Returns the integration configuration for the given method.

```awslocal apigateway delete-integration --rest-api-id <id> --resource-id <resource-id> --http-method <method>```  
&nbsp;&nbsp;&nbsp;&nbsp;Removes the integration from the method.

### Deployments

```awslocal apigateway create-deployment --rest-api-id <id> --stage-name <name> [--description <desc>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Deploys the REST API to the named stage. Returns a deployment object with the generated deployment ID.

```awslocal apigateway update-deployment --rest-api-id <id> --deployment-id <deployment-id> [--patch-operations op=replace,path=/description,value=<text>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Updates a deployment (e.g. changes its description).

```awslocal apigateway delete-deployment --rest-api-id <id> --deployment-id <deployment-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Deletes the deployment.

### Usage Plans

```awslocal apigateway create-usage-plan --name <name> [--description <desc>] [--api-stages apiId=<id>,stage=<name>] [--throttle burstLimit=<n>,rateLimit=<n>] [--quota limit=<n>,period=DAY|WEEK|MONTH] [--tags <key=value,...>]```  
&nbsp;&nbsp;&nbsp;&nbsp;Creates a usage plan. Link it to a stage via `--api-stages` and set rate limits with `--throttle` and `--quota`.

```awslocal apigateway create-usage-plan-key --usage-plan-id <plan-id> --key-id <api-key-id> --key-type API_KEY```  
&nbsp;&nbsp;&nbsp;&nbsp;Associates an existing API key with a usage plan. The key must already exist (create it first with `create-api-key`).

```awslocal apigateway delete-usage-plan --usage-plan-id <plan-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;Deletes the usage plan.

## EXAMPLES

### Full REST API setup with Lambda integration, usage plan, and API key

```bash
lambdaArn="arn:aws:lambda:eu-central-1:000000000000:function:my-function"
stageName="prod"

# 1. Create the REST API
apiJson=$$(awslocal apigateway create-rest-api --name my-api)
restApiId=$$(echo "$$apiJson" | jq -r .id)

# 2. Get the root resource ID
rootResourceId=$$(awslocal apigateway get-resources --rest-api-id "$$restApiId" \
  | jq -r '.items[] | select(.path == "/") | .id')

# 3. Attach a GET method (API key required)
awslocal apigateway put-method \
  --rest-api-id "$$restApiId" --resource-id "$$rootResourceId" \
  --http-method GET --authorization-type NONE --api-key-required

# 4. Point the method at the Lambda function
awslocal apigateway put-integration \
  --rest-api-id "$$restApiId" --resource-id "$$rootResourceId" \
  --http-method GET --type AWS_PROXY --integration-http-method POST \
  --uri "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/$${lambdaArn}/invocations"

# 5. Deploy to a stage
deploymentId=$$(awslocal apigateway create-deployment \
  --rest-api-id "$$restApiId" --stage-name "$$stageName" | jq -r .id)

# 6. Create a usage plan linked to the stage
usagePlanId=$$(awslocal apigateway create-usage-plan \
  --name my-plan \
  --api-stages "apiId=$${restApiId},stage=$${stageName}" \
  --throttle "burstLimit=100,rateLimit=50" \
  --quota "limit=10000,period=MONTH" | jq -r .id)

# 7. Create an API key
apiKeyJson=$$(awslocal apigateway create-api-key --name my-key --enabled)
apiKeyId=$$(echo "$$apiKeyJson" | jq -r .id)
apiKeyValue=$$(echo "$$apiKeyJson" | jq -r .value)

# 8. Associate the key with the usage plan
awslocal apigateway create-usage-plan-key \
  --usage-plan-id "$$usagePlanId" --key-id "$$apiKeyId" --key-type API_KEY

# 9. Invoke the endpoint
curl -s -H "x-api-key: $${apiKeyValue}" \
  "http://localhost:4566/restapis/$${restApiId}/$${stageName}/"
```

### Create a REST API with path parameters

```bash
# Create REST API
restApiId=$$(awslocal apigateway create-rest-api --name books-api | jq -r .id)
rootId=$$(awslocal apigateway get-resources --rest-api-id "$$restApiId" \
  | jq -r '.items[] | select(.path == "/") | .id')

# Create /books resource
booksId=$$(awslocal apigateway create-resource \
  --rest-api-id "$$restApiId" --parent-id "$$rootId" \
  --path-part books | jq -r .id)

# Create /books/{isbn} resource
isbnId=$$(awslocal apigateway create-resource \
  --rest-api-id "$$restApiId" --parent-id "$$booksId" \
  --path-part '{isbn}' | jq -r .id)

# Attach GET method and Lambda integration
awslocal apigateway put-method \
  --rest-api-id "$$restApiId" --resource-id "$$isbnId" \
  --http-method GET --authorization-type NONE

awslocal apigateway put-integration \
  --rest-api-id "$$restApiId" --resource-id "$$isbnId" \
  --http-method GET --type AWS_PROXY --integration-http-method POST \
  --uri "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/arn:aws:lambda:eu-central-1:000000000000:function:get-book/invocations"

awslocal apigateway create-deployment \
  --rest-api-id "$$restApiId" --stage-name v1

# Call it — {isbn} is passed as pathParameters.isbn in the Lambda event
curl -s "http://localhost:4566/restapis/$${restApiId}/v1/books/9783911244381"
```

### List all API keys (with values)

```
awslocal apigateway get-api-keys --include-values
{
    "items": [
        {
            "id": "2gscwmeicszl",
            "value": "vmJ6Wq2ZgSqwqbKwaaNdKpkGlu6aFjlhzvTgt7v5E1",
            "name": "my-key",
            "customerId": "",
            "description": "",
            "enabled": true,
            "tags": {}
        }
    ]
}
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksecretsmanager(1)```, ```awsmocksqs(1)```
