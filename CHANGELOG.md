# Changelog

## [1.5.4](https://github.com/jensvogt/awsmock/compare/v1.5.3...v1.5.4) (2025-06-30)


### Bug Fixes

* **1289-backup count:** fix import/export issues ([1920c94](https://github.com/jensvogt/awsmock/commit/1920c9461fc34185ffd7b2f92b82ce9d7cbd5a26))
* **1289-backup count:** fix SQS batch delete bug ([8aa6682](https://github.com/jensvogt/awsmock/commit/8aa66824c4cde479df355d9f0e763c12c3f85937))
* **development:** add SSM parameters ([fc28618](https://github.com/jensvogt/awsmock/commit/fc28618fcbb803d3bac67b65505e0b5247ef1590))
* **development:** add SSM parameters details ([af88bdd](https://github.com/jensvogt/awsmock/commit/af88bdd21bb70130f04c5181f223dc59083bcbac))

## [1.5.3](https://github.com/jensvogt/awsmock/compare/v1.5.2...v1.5.3) (2025-06-28)

### Bug Fixes

* **1289-backup count:** add key management key
  details ([691ce78](https://github.com/jensvogt/awsmock/commit/691ce781e67a53f86b16697395dfeca19cfcfa35))
* **1289-backup count:** add key management
  list ([c914425](https://github.com/jensvogt/awsmock/commit/c9144252d162f2c1bb1002ee98da7b077809b06a))
* **1289-backup count:** add SQS queue default message
  attributes ([bc45367](https://github.com/jensvogt/awsmock/commit/bc4536760602561e9b96639ad988a460996b4b32))
* **1289-backup count:** fix backup
  cleanup ([a6fe366](https://github.com/jensvogt/awsmock/commit/a6fe366862e18543b077ef96ed16f30f0629f45e))
* **1289-backup count:** fix bug in SQS message attribute
  list ([504814f](https://github.com/jensvogt/awsmock/commit/504814f72c24b56ef0688d581b3a1f2ac3d50c2e))
* **1289-backup count:** fix bug lambda
  upload ([226a9e8](https://github.com/jensvogt/awsmock/commit/226a9e80edd92855257be7638faf3fc6af307876))
* **1289-backup count:** fix
  tests ([c43cd3c](https://github.com/jensvogt/awsmock/commit/c43cd3ca76f4f2d0f9e364be7a10bdb8e2727ca4))

## [1.5.2](https://github.com/jensvogt/awsmock/compare/v1.5.1...v1.5.2) (2025-06-20)

### Bug Fixes

* **1288-add-lambda-event-source:** add lambda event
  sources ([3199f42](https://github.com/jensvogt/awsmock/commit/3199f42409eae93920cae904239292b4935378be))
* **1288-add-lambda-event-source:** add lambda event
  sources ([85210fc](https://github.com/jensvogt/awsmock/commit/85210fcc1e3da1d79786d2286eb3217c4005b550))
* **1288-add-lambda-event-source:** add lambda event
  sources ([ad93c1e](https://github.com/jensvogt/awsmock/commit/ad93c1e878ee4ffb567a933f14ed84785b29b79a))
* **1288-kms-ui-pages:** add top-x counters for ui
  charts ([40c13ec](https://github.com/jensvogt/awsmock/commit/40c13eca2073db7eeb29fee504bdbbc95d6e0889))
* **1288-kms-ui-pages:** fix bug in SQS message
  redrive ([2bb44e0](https://github.com/jensvogt/awsmock/commit/2bb44e02aff7d81d9d7624d171bc8b4677283441))

## [1.5.1](https://github.com/jensvogt/awsmock/compare/v1.5.0...v1.5.1) (2025-06-14)

### Bug Fixes

* **1275-add-secretmanager-rotation:** backup for all
  modules ([8568ae3](https://github.com/jensvogt/awsmock/commit/8568ae3c8d23c303aab8190e6e8774b4e0bf1fdb))
* **1275-add-secretmanager-rotation:** fix bug in
  backup ([2659282](https://github.com/jensvogt/awsmock/commit/2659282f99dbceeb53e459a5a354a202d4525c51))
* **1275-add-secretmanager-rotation:** fix bug in logging, add backup via
  cron ([8e4775f](https://github.com/jensvogt/awsmock/commit/8e4775fffef3ccfabd04c6cb6de31d7abfbfd230))
* **1275-add-secretmanager-rotation:** remove python 3.8 lambda support, add python 3.12,
  3.13 ([fa8a083](https://github.com/jensvogt/awsmock/commit/fa8a08329afb3ed12d5437d1867d9e85ef0c1260))

## [1.5.0](https://github.com/jensvogt/awsmock/compare/v1.4.0...v1.5.0) (2025-06-09)

### Features

* **1275-add-secretmanager-rotation:** add nodejs22 lambda
  support ([f2f1e5b](https://github.com/jensvogt/awsmock/commit/f2f1e5b107a8bea681db335c425ada89c4c21ba6))

## [1.4.0](https://github.com/jensvogt/awsmock/compare/v1.3.1...v1.4.0) (2025-06-04)

### Features

* **1251-show-lambda-logs:** fix lambda details
  page ([da01080](https://github.com/jensvogt/awsmock/commit/da01080e8a296836f97a92a9aaedf0c92c5b71c4))

## [1.3.1](https://github.com/jensvogt/awsmock/compare/v1.3.0...v1.3.1) (2025-05-25)

### Bug Fixes

* **1219-dynamodb-resolve-host-exception:** new DyanmoDB get/put item JSON
  schema ([17c8d20](https://github.com/jensvogt/awsmock/commit/17c8d206cce3186d96bc95214f9de5333f6d45e2))
* **1219-dynamodb-resolve-host-exception:** streamline dynamodb
  DTOs ([d764196](https://github.com/jensvogt/awsmock/commit/d76419657b50403d90af91b6238f2e394265d2ea))
* **1219-dynamodb-resolve-host-exception:** streamline dynamodb
  DTOs ([8725fad](https://github.com/jensvogt/awsmock/commit/8725fad0507d37cf0bcf920f29fb98421fc90bf0))
* **1219-dynamodb-resolve-host-exception:** streamline dynamodb
  DTOs ([55b934c](https://github.com/jensvogt/awsmock/commit/55b934c7705a091f796858d0056afafa3c7db557))

## [1.3.0](https://github.com/jensvogt/awsmock/compare/v1.2.5...v1.3.0) (2025-05-18)

### Features

* **PIM-1234:** windows
  port ([bc6a494](https://github.com/jensvogt/awsmock/commit/bc6a494a1ba8caa57f73142093a25ccca3fc1b95))

### Bug Fixes

* **1181-dynamodb-tablesitems-missing-in-ui:** fix logging
  backend ([3189583](https://github.com/jensvogt/awsmock/commit/318958371385f5ea7ca97a0319e7959328f15ee0))
* **1181-dynamodb-tablesitems-missing-in-ui:** fix metric
  service ([e6871fe](https://github.com/jensvogt/awsmock/commit/e6871fe16082893d07fe8dd53474130203195718))
* **1219-dynamodb-resolve-host-exception:** fix resolve exception, fix monitoring http counter, fix
  imports. ([3e9ea85](https://github.com/jensvogt/awsmock/commit/3e9ea85217d8fc03ec21543477bc03ef8a583ff8))
* **1219-dynamodb-resolve-host-exception:** fix resolve exception, fix monitoring http counter, fix
  imports. ([f90a22d](https://github.com/jensvogt/awsmock/commit/f90a22dc6167bedb82e1241305cb6a9271f41f60))
* **1219-dynamodb-resolve-host-exception:** new DyanmoDB get/put item JSON
  schema ([808975d](https://github.com/jensvogt/awsmock/commit/808975d0bcffd04111f0a0d19c2254648fc925d6))
* **1219-dynamodb-resolve-host-exception:** new DyanmoDB get/put item JSON
  schema ([3fcbf64](https://github.com/jensvogt/awsmock/commit/3fcbf642c65138205ecbe7f96f3fdd5b38375f63))
* **1219-dynamodb-resolve-host-exception:** new DyanmoDB get/put item JSON
  schema ([68a376f](https://github.com/jensvogt/awsmock/commit/68a376f4e9d134c55fa99958ff579434f015ce0c))

## [1.2.5](https://github.com/jensvogt/awsmock/compare/v1.2.4...v1.2.5) (2025-05-11)

### Bug Fixes

* **1181-dynamodb-tablesitems-missing-in-ui:** add transfer server tags, fix S3 metadata in
  frontend ([1a41367](https://github.com/jensvogt/awsmock/commit/1a413673c669eeab8769cef281578f26c0ed1a71))

## [1.2.4](https://github.com/jensvogt/awsmock/compare/v1.2.3...v1.2.4) (2025-05-10)

### Bug Fixes

* **1181-dynamodb-tablesitems-missing-in-ui:** segfault in SQS
  database ([78bbc62](https://github.com/jensvogt/awsmock/commit/78bbc62251ecbf2674dc7ba47306234753f31435))

## [1.2.3](https://github.com/jensvogt/awsmock/compare/v1.2.2...v1.2.3) (2025-05-04)

### Bug Fixes

* **1164-main-development:** add HTTP to SNS
  subscriptions ([bf84159](https://github.com/jensvogt/awsmock/commit/bf841599858eec85fcc044ac177cbde8600c1af4))
* **1164-main-development:** add HTTP to SNS
  subscriptions ([6f07d7e](https://github.com/jensvogt/awsmock/commit/6f07d7e8d788326ee73e6c9def4d52c55c3071e1))
* **1164-main-development:** add HTTP to SNS
  subscriptions ([3a31699](https://github.com/jensvogt/awsmock/commit/3a316995c3b79cb16cd6be4b55b74a7dfa87162a))
* **1164-main-development:** add local
  network ([2a58aff](https://github.com/jensvogt/awsmock/commit/2a58affc825eb668e6656866429e3938592e01c9))
* **1164-main-development:** fix dynamodb
  setup ([2104b67](https://github.com/jensvogt/awsmock/commit/2104b67a1024798428e6b0779234cb2f729e96ec))
* **1164-main-development:** fix dynamodb
  setup ([cd165e9](https://github.com/jensvogt/awsmock/commit/cd165e9cd4310c0b97d881f3359915adc26ec0f5))
* **1164-main-development:** fix dynamodb
  tables ([d3e3e38](https://github.com/jensvogt/awsmock/commit/d3e3e38d0cbc3823c53658d3c3565993df5510a7))

## [1.2.2](https://github.com/jensvogt/awsmock/compare/v1.2.1...v1.2.2) (2025-05-03)

### Bug Fixes

* **1164-main-development:** fix
  tests ([7a6cd15](https://github.com/jensvogt/awsmock/commit/7a6cd15c4e5e8e7b65b59a8d4c2b2bebc79a8b13))
* **1164-main-development:** fix
  tests ([2f8ecad](https://github.com/jensvogt/awsmock/commit/2f8ecadba9dceb5784c32ac223131fa41e7f1fae))
* **1164-main-development:** SQS sendmessage with new JSON
  format ([ffcaf30](https://github.com/jensvogt/awsmock/commit/ffcaf30cf4af0292e603e3d1564494e2f6f81570))
* **1164-main-development:** using shared memory for monitoring in order to improve
  performance ([47ccb2a](https://github.com/jensvogt/awsmock/commit/47ccb2a0e60a2fc61b68b6552d3a9f9b9bbe5355))
* **1164-sftp-on-windows:** windows absolute
  path ([b30a1b5](https://github.com/jensvogt/awsmock/commit/b30a1b5373a3f0bb648bc2b2ceba1c699ba24d65))

## [1.2.1](https://github.com/jensvogt/awsmock/compare/v1.2.0...v1.2.1) (2025-05-01)

### Bug Fixes

* **1164-sftp-on-windows:** add dir
  permissions ([3f3703d](https://github.com/jensvogt/awsmock/commit/3f3703d7efbc9f6187a6ac79e00c752eeaadd9fd))

## [1.2.0](https://github.com/jensvogt/awsmock/compare/v1.1.0...v1.2.0) (2025-03-05)

### Features

* **123-windows-port:** add some auto
  pointer ([84b1dfa](https://github.com/jensvogt/awsmock/commit/84b1dfaa638b5aedb0c2b2070ac19f5115835eef))
* **801-macos-port:** first
  try ([d23225a](https://github.com/jensvogt/awsmock/commit/d23225a83ed023f9447a504d03d2476c77a96be7))
* **801-macos-port:** first
  try ([3a42343](https://github.com/jensvogt/awsmock/commit/3a423438ffaafcc69e2a77d9eacab88c839d6dd3))
* **801-macos-port:** first
  try ([ae15f05](https://github.com/jensvogt/awsmock/commit/ae15f05bf84c6c21c8d6998f6f748e12c16e15cc))
* **891-macos-port:** add some includes for
  macos ([252cf50](https://github.com/jensvogt/awsmock/commit/252cf50731ea3302ab42516e953679548ce6a359))

### Bug Fixes

* **801-macos-port:** fix ListAllBuckets
  call ([e2268b1](https://github.com/jensvogt/awsmock/commit/e2268b11c87ff4133745ae45f9c7df39f2f010a4))
* **801-macos-port:** fix performance
  counters ([a8c412b](https://github.com/jensvogt/awsmock/commit/a8c412bb2ebde4789f980cd2d5ee8a4365a7c0dd))

## [1.1.0](https://github.com/jensvogt/awsmock/compare/v1.0.1...v1.1.0) (2025-02-01)

### Features

* **903-auto-load-should-load-all-jaon-files:** add all files
  load ([d96cbe2](https://github.com/jensvogt/awsmock/commit/d96cbe2b8bb70f1dd208d78337ce850455defda1))
* **903-auto-load-should-load-all-jaon-files:** add all files
  load ([d77a3b0](https://github.com/jensvogt/awsmock/commit/d77a3b0701be39c0aff0fe3776ed3885afa42cb4))
* **903-auto-load-should-load-all-jaon-files:** add all files
  load ([424c333](https://github.com/jensvogt/awsmock/commit/424c333989beee279e1b735cf3f02769dd36aa25))
* **903-auto-load-should-load-all-jaon-files:** add all files
  load ([6cd3c3f](https://github.com/jensvogt/awsmock/commit/6cd3c3f77c9f1f715088671fd5836a83fe8eaa76))
* **903-auto-load-should-load-all-json-files:** add all files
  load ([eeb8d7b](https://github.com/jensvogt/awsmock/commit/eeb8d7b7155010f690b1de297e204b6ef88c22c2))

### Bug Fixes

* **864-transfer-details-page-missing-in-frontend:** fix FE edit SQS
  attribute ([582d29f](https://github.com/jensvogt/awsmock/commit/582d29f3714f0685986c9936457847faa6507919))
* **864-transfer-details-page-missing-in-frontend:** fix FE edit SQS
  attribute ([9fdc3bc](https://github.com/jensvogt/awsmock/commit/9fdc3bc531485832ad7c919fb87cccc72f2c88d4))
* **864-transfer-details-page-missing-in-frontend:** fix FE edit SQS
  attribute ([669c0ec](https://github.com/jensvogt/awsmock/commit/669c0ec7c5388616337e23b4b719cd01a7f62a6b))
* **864-transfer-details-page-missing-in-frontend:** fix FE edit SQS
  attribute ([bfbf142](https://github.com/jensvogt/awsmock/commit/bfbf142bd821aa2e8759a0cf1cec3e76f8bc5d9b))
