---
title: awsmockcognito(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockcognito AwsMock Cognito module
footer: awsmockcognito $version$
---

## NAME

```awsmockcognito``` AwsMock Cognito module

## DESCRIPTION

Cognito is a managed identity service provided by AWS that is used for securing user authentication, authorization, and
managing user identities in web and mobile applications. Cognito enables developers to add user sign-up, sign-in, and
access control functionalities to their applications. Cognito supports various authentication methods, including social
identity providers, SAML-based identity providers, and custom authentication flows.

AwsMock supports Cognito, allowing you to use the Cognito APIs in your local environment to manage authentication and
access control for your local application and resources. The supported APIs are available on our Cognito Identity
coverage page and Cognito User Pools coverage page, which provides information on the extent of Cognito’s integration
with AwsMock.

The Cognito module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## COMMANDS

```awslocal cognito-idp create-user-pool --pool-name <user-pool-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new Cognito user pool.

```awslocal cognito-idp list-user-pools```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all available Cognito user pools.

```awslocal cognito-idp delete-user-pool --user-pool-userPoolId <user-pool-id>```  
&nbsp;&nbsp;&nbsp;&nbsp;delete a Cognito user pool.

```awslocal cognito-idp admin-create-user --user-pool-id <user-pool-id> --username <user-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a Cognito user.

```awslocal cognito-idp create-group --user-pool-id <user-pool-id> --group-name <group-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a Cognito user group.

```awslocal cognito-idp list-groups --user-pool-id <user-pool-id>```   
&nbsp;&nbsp;&nbsp;&nbsp;list groups in a user pool

```awslocal cognito-idp list-groups --user-pool-id <user-pool-id>```   
&nbsp;&nbsp;&nbsp;&nbsp;list groups in a user pool

```awslocal cognito-idp list-users-in-group --user-pool-id <user-pool-id> --group-name <group-name>```  
&nbsp;&nbsp;&nbsp;&nbsp;lists all users in a group

```awslocal cognito-idp create-user-pool-client --user-pool-id <user-pool-id> --client-name <client-name> --generate-secret```   
&nbsp;&nbsp;&nbsp;&nbsp;creates a Cognito user pool client

```awslocal cognito-idp delete-user-pool-client --user-pool-id <user-pool-id> --client-id <client-id>```   
&nbsp;&nbsp;&nbsp;&nbsp;deletes a Cognito user pool client

```awslocal cognito-idp create-user-pool-domain --user-pool-id <user-pool-id> --domain <domain-name>```   
&nbsp;&nbsp;&nbsp;&nbsp;creates a Cognito user pool client

## EXAMPLES

To create a new Cognito user pools:

```
awslocal cognito-idp create-function --create-user-pool --pool-name test-user-pool
```

To get a list of all currently available Cognito user pools:

```
awslocal cognito-idp list-user-pools --max-results 10
{
    "UserPools": [
        {
            "Id": "eu-central-1_6g67LGPzz",
            "Name": "test-user-pool",
            "LastModifiedDate": "2023-12-22T17:40:26+01:00",
            "CreationDate": "2023-12-22T17:40:26+01:00"
        }
    ]
}
```

To delete a Cognito user pool:

```
awslocal cognito-idp delete-user-pool --user-pool-userPoolId eu-central-1_6g67LGPzz
```

## Authentication in a Web App

In order to use Cognito Identity provider authentication, a browser extension has be used in order to redirect the
requests from AWS production servers
(*.amazonaws.com) to AwsMock. The samples directory contains such a Chrome browser extension in the
```/usr/local/share/awsmock/cognito/resources/redirect```. Head over
to chrome://extensions/ in Chrome, then select "Load unpacked" and point to the directory where the source code files
are stored on the disk.

In order to start the authentication process a user pool, users and a user pool client has be created. Create the
AwsMock object using the CLI or a application. The
authentication flow should be initiated like in a NodeJs web application like this:

```
import {Injectable} from '@angular/core';
import {Router} from "@angular/router";
import {AuthenticationDetails, CognitoUser, CognitoUserPool,} from "amazon-cognito-identity-js";
import {environment} from "../environments/environment";

@Injectable({
  providedIn: 'root'
})
export class CognitoService {
  userPool: any;
  cognitoUser: any;
  username: string = "";

  constructor(private router: Router) {
  }

  // Login
  login(userName: any, password: any) {
    let authenticationDetails = new AuthenticationDetails({
      Username: userName,
      Password: password,
    });

    let poolData = {
      UserPoolId: environment.cognito.userPoolId,
      ClientId: environment.cognito.userPoolWebClientId,
    };

    this.username = userName;
    this.userPool = new CognitoUserPool(poolData);
    let userData = {Username: userName, Pool: this.userPool};
    this.cognitoUser = new CognitoUser(userData);

    this.cognitoUser.authenticateUser(authenticationDetails, {
      onSuccess: (result: any) => {
        this.router.navigate(["/home"]);
        console.log("Success Results : ", result);
      },
      // First time login attempt
      newPasswordRequired: () => {
        this.router.navigate(["/newPasswordRequire"]);
      },
      onFailure: (error: any) => {
        console.log("Login failed, error", error);
      },
    });
  }

  // Logout
  logOut() {
    let poolData = {
      UserPoolId: environment.cognito.userPoolId,
      ClientId: environment.cognito.userPoolWebClientId,
    };
    this.userPool = new CognitoUserPool(poolData);
    this.cognitoUser = this.userPool.getCurrentUser();
    console.log("UserPool: ", this.userPool)
    console.log("User: ", this.cognitoUser)
    if (this.cognitoUser) {
      this.cognitoUser.signOut();
      this.router.navigate(["/login"]);
    }
  }
}
```

The general environment should look like this:

```
export const environment = {
  production: false,
  apiUrl: 'http://localhost:4566',
  cognito: {
    userPoolId: 'eu-central-1_9e5afEQXm',
    userPoolWebClientId: 'cOXwdSQHZNV2KxhLBshvepz5Fd',
    endpoint: 'http://localhost:4566'
  },
};
``` 

with ```userPoolId``` and ```userPoolCLientId``` from your AwsMock environment.

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```,
```awsmocksecretsmanager(1)```, ```awsmocksqs(1)```, ```awsmockssm(1)```, ```awsmockapi(1)```