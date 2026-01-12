# S3 Examples

This directory contains examples for the use of the AwsMock AWS simulation package. Most of the features of AwsMock are
covered in the different samples.

All example need a running awsmock manager listening on port 4566. If you prefer another port, edit the sample bash
script accordingly. To set a port of ```9000``` change the line ```--endpoint http://localhost:4566``` to
```--endpoint http://localhost:9000```.

All examples are cleaned up after execution so that the next example runs without errors.

This directory container the following SQS examples:

- ```create-bucket.sh```: creates a new bucket ```test-bucket```
- ```delete-bucket.sh```: deletes the bucket ```test-bucket```
- ```get-object.sh```: gets an objects from a bucket ```test-bucket```
- ```list-bucket.sh```: lists buckets
- ```put-big-object.sh```: puts a big object (several GB) to bucket ```test-bucket```
- ```put-lifecycle-configuration.sh```: puts a lifecycle to bucket ```test-bucket```
- ```get-lifecycle-configuration.sh```: gets the lifecycle configuration for the bucket ```test-bucket```
- ```delete-lifecycle-configuration.sh```: deletes the lifecycle configuration for a bucket ```test-bucket```
- ```put-object.sh```: puts a object to bucket ```test-bucket```
- ```cleanup.sh```: deletes all created buckets and objects

