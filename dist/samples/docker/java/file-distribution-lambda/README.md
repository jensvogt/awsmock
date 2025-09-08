# file-distribution-lambda

## General description

This lambda sends the incoming files to different queues, depending on their content. Image files (with endings
```.png```, ```.jog```,```.gif``` and ```.tif```) are send to the ```file-distribution-image-queue```. XML files
are send to the ```file-distribution-xml-queue``` and JSON file are send to the ```file-distribution-json-queue```.

### S3 Notification

The lambda function is triggered by the S3 ```CreateObject*``` notification event.

