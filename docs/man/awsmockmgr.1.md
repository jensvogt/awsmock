---
title: awsmockmgr(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockmgr AwsMock Manager
footer: awsmockmgr $version$
---

## NAME

```awsmockmgr``` AwsMock manager

## SYNOPSIS

```awsmockmgr [OPTIONS] [COMMAND]```

## DESCRIPTION

awsmockmgr is the mein AwsMock module manager. Each AwsMock service has its own module. The manager
starts/stops/restarts this modules. Supported modules are:

&nbsp;&nbsp;&nbsp;&nbsp;```S3```: AWS file store mock

&nbsp;&nbsp;&nbsp;&nbsp;```SQS```: AWS Simple Queueing System mock

&nbsp;&nbsp;&nbsp;&nbsp;```SNS```: AWS Simple Notification System mock

&nbsp;&nbsp;&nbsp;&nbsp;```Lambda```: AWS lambda function mock

&nbsp;&nbsp;&nbsp;&nbsp;```Transfer```: AWS Transfer Manager

&nbsp;&nbsp;&nbsp;&nbsp;```Cognito```: AWS Cognito mock

&nbsp;&nbsp;&nbsp;&nbsp;```DynamoDB```: AWS dynamodb

&nbsp;&nbsp;&nbsp;&nbsp;```Gateway```: AwsMock API gateway

Per default the gateway runs on port 4566, but can be changed in the configuration file. The default configuration file
is
```/etc/aws-mock.properties```. The manager can run as a docker daemon (see http://docker.io/jensvogt/awsmock for more
information), or as a standalone application running as Linux daemon. AwsMock also supports the persistence of the
current configuration in a MongoDB instance. In order to run AwsMock as a host application with a database, switch the
database support on in the properties file.

AwsMock can be connected to a MongoDB database. In this case all created objects and features are saved in several
collections. The database connection parameters can be specified in the properties file.

## OPTIONS

```--daemon```  
&nbsp;&nbsp;&nbsp;&nbsp;Run AwsMock manager as a UNIX daemon.

```--umask```  
&nbsp;&nbsp;&nbsp;&nbsp;Set the daemon's umask (octal, e.g. 027).

``` --pidfile <path>```  
&nbsp;&nbsp;&nbsp;&nbsp;Write the process ID of the application to given file.

```--config <configuration-file-path>```  
&nbsp;&nbsp;&nbsp;&nbsp;Starts the controller with a given configuration file. Per default the configuration file is
```/etc/aws-mock.properties```

``` --loglevel <level>```  
&nbsp;&nbsp;&nbsp;&nbsp;Starts the manager with the given logging level. Valid values are: trace, debug, information,
warning, error, critical, fatal

``` --logfile <path>```  
&nbsp;&nbsp;&nbsp;&nbsp;Writes the logging message to the given file.

``` --version```  
&nbsp;&nbsp;&nbsp;&nbsp;Shows the current version and exists.

```--help```  
&nbsp;&nbsp;&nbsp;&nbsp;shows the usage screen and exists

## EXAMPLES

Start the AwsMock manager as background daemon process with the 'debug' log level.

```
/usr/bin/awsmockmgr --daemon --loglevel debug
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmocksqs(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```,
```awsmockssm(1)```