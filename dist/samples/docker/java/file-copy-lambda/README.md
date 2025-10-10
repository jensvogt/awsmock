# file-copy-lambda

## General description

This lambda copies files from the S3 ```transfer-bucket``` to the ```file-delivery bucket```. The lambda function is 
triggered by a ```CreateObject*``` S3 notification event.

### S3 Notification

The S3 notification event contains all necessary information for the lambda to process the file. The source bucket and 
key are taken from the S3 notification event and the corresponding file is copied to the same S3 key in the destination
bucket. 
