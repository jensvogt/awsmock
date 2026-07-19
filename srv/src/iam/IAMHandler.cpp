//
// Created by vogje01 on 19/07/2026.
//

#include <awsmock/service/iam/IAMHandler.h>

namespace Awsmock::Service {

    http::response<http::dynamic_body> IAMHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "IAM POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        const std::string body = Core::HttpUtils::GetBodyAsString(request);
        const std::string action = Core::HttpUtils::GetStringParameterFromBody(body, "Action");
        const std::string requestId = Core::AwsUtils::CreateRequestId();
        const std::string accountId = "000000000000";

        try {
            log_info << "IAM action: " << action;

            if (action == "CreateRole") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                std::string xml =
                        "<CreateRoleResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<CreateRoleResult><Role>"
                        "<RoleId>AROAI3KAVITY00000001</RoleId>"
                        "<RoleName>" + roleName + "</RoleName>"
                        "<Arn>arn:aws:iam::" + accountId + ":role/" + roleName + "</Arn>"
                        "<Path>/</Path>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "<AssumeRolePolicyDocument>{}</AssumeRolePolicyDocument>"
                        "</Role></CreateRoleResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</CreateRoleResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "GetRole") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                std::string xml =
                        "<GetRoleResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<GetRoleResult><Role>"
                        "<RoleId>AROAI3KAVITY00000001</RoleId>"
                        "<RoleName>" + roleName + "</RoleName>"
                        "<Arn>arn:aws:iam::" + accountId + ":role/" + roleName + "</Arn>"
                        "<Path>/</Path>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "<AssumeRolePolicyDocument>{}</AssumeRolePolicyDocument>"
                        "<MaxSessionDuration>3600</MaxSessionDuration>"
                        "</Role></GetRoleResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</GetRoleResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "DeleteRole") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                log_info << "DeleteRole, roleName: " << roleName;
                std::string xml =
                        "<DeleteRoleResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</DeleteRoleResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "AttachRolePolicy") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                log_info << "AttachRolePolicy, roleName: " << roleName;
                std::string xml =
                        "<AttachRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</AttachRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "DetachRolePolicy") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                log_info << "DetachRolePolicy, roleName: " << roleName;
                std::string xml =
                        "<DetachRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</DetachRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "ListAttachedRolePolicies") {
                std::string xml =
                        "<ListAttachedRolePoliciesResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ListAttachedRolePoliciesResult>"
                        "<AttachedPolicies/><IsTruncated>false</IsTruncated>"
                        "</ListAttachedRolePoliciesResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</ListAttachedRolePoliciesResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "ListRolePolicies") {
                std::string xml =
                        "<ListRolePoliciesResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ListRolePoliciesResult>"
                        "<PolicyNames/><IsTruncated>false</IsTruncated>"
                        "</ListRolePoliciesResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</ListRolePoliciesResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "TagRole") {
                std::string xml =
                        "<TagRoleResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</TagRoleResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "UntagRole") {
                std::string xml =
                        "<UntagRoleResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</UntagRoleResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "ListRoleTags") {
                std::string xml =
                        "<ListRoleTagsResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ListRoleTagsResult><Tags/><IsTruncated>false</IsTruncated></ListRoleTagsResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</ListRoleTagsResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "CreatePolicy") {
                const std::string policyName = Core::HttpUtils::GetStringParameterFromBody(body, "PolicyName");
                std::string xml =
                        "<CreatePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<CreatePolicyResult><Policy>"
                        "<PolicyId>ANPAI3KAVITY00000001</PolicyId>"
                        "<PolicyName>" + policyName + "</PolicyName>"
                        "<Arn>arn:aws:iam::" + accountId + ":policy/" + policyName + "</Arn>"
                        "<Path>/</Path>"
                        "<DefaultVersionId>v1</DefaultVersionId>"
                        "<AttachmentCount>0</AttachmentCount>"
                        "<IsAttachable>true</IsAttachable>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "<UpdateDate>2026-01-01T00:00:00Z</UpdateDate>"
                        "</Policy></CreatePolicyResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</CreatePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "GetPolicy") {
                const std::string policyArn = Core::HttpUtils::GetStringParameterFromBody(body, "PolicyArn");
                const std::string policyName = policyArn.substr(policyArn.rfind('/') + 1);
                std::string xml =
                        "<GetPolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<GetPolicyResult><Policy>"
                        "<PolicyId>ANPAI3KAVITY00000001</PolicyId>"
                        "<PolicyName>" + policyName + "</PolicyName>"
                        "<Arn>" + policyArn + "</Arn>"
                        "<Path>/</Path>"
                        "<DefaultVersionId>v1</DefaultVersionId>"
                        "<AttachmentCount>0</AttachmentCount>"
                        "<IsAttachable>true</IsAttachable>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "<UpdateDate>2026-01-01T00:00:00Z</UpdateDate>"
                        "</Policy></GetPolicyResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</GetPolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "DeletePolicy") {
                std::string xml =
                        "<DeletePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</DeletePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "GetPolicyVersion") {
                const std::string policyArn = Core::HttpUtils::GetStringParameterFromBody(body, "PolicyArn");
                const std::string versionId = Core::HttpUtils::GetStringParameterFromBody(body, "VersionId");
                std::string xml =
                        "<GetPolicyVersionResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<GetPolicyVersionResult><PolicyVersion>"
                        "<VersionId>" + versionId + "</VersionId>"
                        "<IsDefaultVersion>true</IsDefaultVersion>"
                        "<Document>%7B%7D</Document>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "</PolicyVersion></GetPolicyVersionResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</GetPolicyVersionResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "CreatePolicyVersion") {
                std::string xml =
                        "<CreatePolicyVersionResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<CreatePolicyVersionResult><PolicyVersion>"
                        "<VersionId>v1</VersionId>"
                        "<IsDefaultVersion>true</IsDefaultVersion>"
                        "<CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "</PolicyVersion></CreatePolicyVersionResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</CreatePolicyVersionResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "ListPolicyVersions") {
                std::string xml =
                        "<ListPolicyVersionsResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ListPolicyVersionsResult>"
                        "<Versions><member>"
                        "<VersionId>v1</VersionId><IsDefaultVersion>true</IsDefaultVersion><CreateDate>2026-01-01T00:00:00Z</CreateDate>"
                        "</member></Versions>"
                        "<IsTruncated>false</IsTruncated>"
                        "</ListPolicyVersionsResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</ListPolicyVersionsResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "DeletePolicyVersion") {
                std::string xml =
                        "<DeletePolicyVersionResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</DeletePolicyVersionResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "PutRolePolicy") {
                std::string xml =
                        "<PutRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</PutRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "DeleteRolePolicy") {
                std::string xml =
                        "<DeleteRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</DeleteRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "GetRolePolicy") {
                const std::string roleName = Core::HttpUtils::GetStringParameterFromBody(body, "RoleName");
                const std::string policyName = Core::HttpUtils::GetStringParameterFromBody(body, "PolicyName");
                std::string xml =
                        "<GetRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<GetRolePolicyResult>"
                        "<RoleName>" + roleName + "</RoleName>"
                        "<PolicyName>" + policyName + "</PolicyName>"
                        "<PolicyDocument>%7B%7D</PolicyDocument>"
                        "</GetRolePolicyResult>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</GetRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "UpdateAssumeRolePolicy") {
                std::string xml =
                        "<UpdateAssumeRolePolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</UpdateAssumeRolePolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "ListRoleTags" || action == "ListPolicyTags") {
                const std::string tagType = (action == "ListRoleTags") ? "ListRoleTags" : "ListPolicyTags";
                std::string xml =
                        "<" + tagType + "Response xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<" + tagType + "Result><Tags/><IsTruncated>false</IsTruncated></" + tagType + "Result>"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</" + tagType + "Response>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "TagPolicy") {
                std::string xml =
                        "<TagPolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</TagPolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            if (action == "UntagPolicy") {
                std::string xml =
                        "<UntagPolicyResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                        "<ResponseMetadata><RequestId>" + requestId + "</RequestId></ResponseMetadata>"
                        "</UntagPolicyResponse>";
                std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                return SendResponse(request, http::status::ok, xml, headers);
            }

            log_error << "Unknown IAM action: " << action;
            std::string errXml =
                    "<ErrorResponse xmlns=\"https://iam.amazonaws.com/doc/2010-05-08/\">"
                    "<Error><Code>NotImplemented</Code><Message>Action not implemented: " + action + "</Message></Error>"
                    "<RequestId>" + requestId + "</RequestId>"
                    "</ErrorResponse>";
            std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
            return SendResponse(request, http::status::not_implemented, errXml, headers);

        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

}// namespace Awsmock::Service
