---
title: awsmocks3(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmocks3 AwsMock S3 module
footer: awsmocks3 $version$
---

## NAME

```awsmocks3``` AwsMock S3 module

## DESCRIPTION

S3 (Simple Storage Service) is an object storage service that provides a highly scalable and durable solution for
storing and retrieving data. In S3, a bucket represents a directory, while an object corresponds to a file. Each object
or file within S3 encompasses essential attributes such as a unique key denoting its name, the actual content it holds,
a version ID for versioning support, and accompanying metadata. S3 can store unlimited objects, allowing you to store,
retrieve, and manage your data in a highly adaptable and reliable manner.

AwsMock supports S3, allowing you to use the S3 APIs in your local environment to create new buckets, manage your S3
objects, and test your S3 configurations locally. The supported APIs are available in the example section, which
provides information on the extent of S3’s integration with AwsMock.

AwsMock s3 module supports encryption for bucket. Currently only SSE-KMS is supported. The encryption key must be stored
in the KMS module. See ```awsmockmks(1)``` for more details on how to store encryption key in KMS.

The S3 module can be configured using the ```awslocal``` command. For details of the ```awslocal``` command see the
corresponding man page ```awslocal(1)```.

## VIRTUAL HOSTING OF BUCKETS

The S3 client should be configured to use the path style bucket name. It's easier to use and more reliable. Path style
is deprecated and should be removed in a future release. Nevertheless, path-style deprecation was first announced 2020,
but still exists as it is easier to use in AWS simulations.

Following are examples for synchronous and asynchronous Java client configurations in case you want to use path-style
requests:

```   
  s3Client = S3Client.builder()
         .credentialsProvider(ProfileCredentialsProvider.create())
         .region(Region.US_EAST_1)
         .endpointOverride(new URI(endpoint))
         .forcePathStyle(true)
         .build();
```   

and for the async client:

```   
  s3AsyncClient = S3CrtAsyncClient.builder()
        .credentialsProvider(ProfileCredentialsProvider.create())
        .region(Region.US_EAST_1)
        .endpointOverride(new URI(endpoint))
        .forcePathStyle(true)
        .build();
```   

If you want ot use host-style location requests, you need to se tup the clients differently:

```
    public S3Client s3Client(AwsCredentialsProvider awsCredentialsProvider) {            
        return S3Client.builder()
                .forcePathStyle(false)
                .endpointOverride(URI.create("http://s3.eu-central-1.awsmock.de:4566"))
                .credentialsProvider(awsCredentialsProvider)
                .build();
    }

  s3AsyncClient = S3CrtAsyncClient.builder()
        .credentialsProvider(ProfileCredentialsProvider.create())
        .region(Region.US_EAST_1)
        .endpointOverride(URI.create("http://s3.eu-central-1.awsmock.de:4566"))
        .forcePathStyle(true)
        .build();
```

Here the endpoint needs to have a ```s3``` prefix and the region should be included in the URL. Additionally, you need
to
provide a DNS settings, which routes all S3 request to awsmock. This is most easily done using a ```dnsmasq```
implementation:

```
sudo apt install dnsmasq 
```

The ```dnsmasq``` configuration should be as follows:

```
port 53
bogus-priv
strict-order
address=/s3.eu-central-1.awsmock.de/172.17.0.1
address=/awsmock.de/127.0.0.1
expand-hosts
domain=awsmock.de
listen-address=127.0.0.1
listen-address=172.17.0.1 // docker host
```

For more information about ```dnsmasq``` see: https://thekelleys.org.uk/dnsmasq/doc.html

## COMMANDS

```awslocal s3 mb s3://test-bucket```  
&nbsp;&nbsp;&nbsp;&nbsp;creates a new bucket

```awslocal s3 rm s3://test-bucket```  
&nbsp;&nbsp;&nbsp;&nbsp;removes an existing bucket

```awslocal s3 ls```  
&nbsp;&nbsp;&nbsp;&nbsp;list all available buckets

```awslocal s3 ls s3://test-bucket --recursive```  
&nbsp;&nbsp;&nbsp;&nbsp;recursively lists all objects in the bucket

## EXAMPLES

To get a list of all currently available buckets:

```
awslocal s3 ls
2023-12-11 17:37:08 test-bucket-1
2023-12-11 17:37:08 test-bucket-2
2023-12-11 17:37:08 test-result-bucket-1
2023-12-11 17:37:08 test-result-bucket-2
```

To upload a local file to a S3 bucket ```s3://test-bucket-1```

```
awslocal s3 cp README.md s3://test-bucket-1/test/README.md
```

To get a list of all S3 objects in the ```s3://test-bucket-1```

```
awslocal s3 ls s3://test-bucket-1 --recursive
2023-12-16 16:01:41       6248 test/README.md
```

To download a S3 object from a S3 bucket ```s3://test-bucket-1```

```
awslocal s3 cp s3://test-bucket-1/test/README.md test.md
```

Switch encryption on for bucket:

```
awslocal s3api put-bucket-encryption --bucket file-delivery --server-side-encryption-configuration file://encryption.json
```

encryption.json contains:

```
{
  "Rules": [
    {
      "ApplyServerSideEncryptionByDefault": {
        "SSEAlgorithm": "AES256",
        "KMSMasterKeyID": "123456"
      }
    }
  ]
}
```

## JAVA EXAMPLES

The following section are Java example using the synchronous S3 client, as well as the transfer manager for bigger
files.

```
// Create a new bucket 
CreateBucketResponse response = s3Client.createBucket(CreateBucketRequest.builder().bucket(name).build());

// List buckets 
ListBucketsResponse response = s3Client.listBuckets();

// Delete a bucket 
DeleteBucketResponse response = s3Client.deleteBucket(DeleteBucketRequest.builder().bucket(name).build());

// Put small object 
PutObjectResponse response = s3Client.putObject(PutObjectRequest.builder().bucket(bucket).key(key).build(), RequestBody.fromString(body));

// Get object 
ResponseInputStream<GetObjectResponse> response = s3Client.getObject(GetObjectRequest.builder().bucket(bucket).key(key).build());
try (OutputStream ofs = new FileOutputStream(filename)) {
  IOUtils.copy(response, ofs);
}

// Copy object 
CopyObjectRequest copyRequest = CopyObjectRequest.builder()
    .sourceBucket(sourceBucket)
    .sourceKey(sourceKey)
    .destinationBucket(destinationBucket)
    .destinationKey(destinationKey)
    .build();
CopyObjectResponse response = s3Client.copyObject(copyRequest);

// Download using transfer manager
DownloadFileRequest downloadFileRequest =
    DownloadFileRequest.builder()
            .getObjectRequest(b -> b.bucket(bucket).key(key))
            .destination(destination)
            .build();
FileDownload downloadFile = s3TransferManager.downloadFile(downloadFileRequest);
CompletedFileDownload downloadResult = downloadFile.completionFuture().join();

// Upload using transfer manager
UploadFileRequest uploadFileRequest =
    UploadFileRequest.builder()
            .putObjectRequest(r -> r.bucket(bucket).key(key))
            .source(destination)
            .build();
FileUpload uploadFile = s3TransferManager.uploadFile(uploadFileRequest);
CompletedFileUpload uploadResult = uploadFile.completionFuture().join();
```

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocksqs(1)```, ```awsmocksns(1)```,
```awsmocklambda(1)```,
```awsmockdynamodb(1)```, ```awsmockcognito(1)```, ```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```,
```awsmocksqs(1)```,
```awsmocksqs(1)```