# awsmock

![Release](https://img.shields.io/github/v/release/jensvogt/awsmock)
![Docker Pulls](https://img.shields.io/docker/pulls/jensvogt/awsmock)
![License](https://img.shields.io/github/license/jensvogt/awsmock)
![Language](https://img.shields.io/github/languages/top/jensvogt/awsmock)
![CI](https://img.shields.io/github/actions/workflow/status/jensvogt/awsmock/build-release.yml)

> Run AWS services locally — no cloud account, no costs, no latency.  
> A high-performance C++ alternative to LocalStack that starts in under a second.

---

## Why awsmock?

Most local AWS simulators are slow, memory-hungry, or lock core features behind a paywall.
awsmock is different: written in C++, it starts instantly, uses minimal memory, and is completely free.

| Feature      | awsmock          | LocalStack (free)    |
|--------------|------------------|----------------------|
| Language     | C++              | Python / JVM         |
| Startup time | ~1s              | 15–30s               |
| Memory usage | ~50 MB           | 500 MB+              |
| Persistence  | ✅ Free (MongoDB) | 💰 Pro only          |
| Web UI       | ✅ Free           | 💰 Pro only          |
| Qt UI        | ✅ Free           | ❌                    |
| Offline      | ✅                | ✅                    |
| Price        | Free             | Free / $35 per month |

---

## Quick Start (30 seconds)

```bash
docker run -p 4566-4568:4566-4568 jensvogt/awsmock:latest
```

Point your AWS SDK or CLI at `http://localhost:4566`. That's it — no config, no credentials required.

```bash
# Verify it's running
aws --endpoint-url=http://localhost:4566 s3 mb test-bucket
aws --endpoint-url=http://localhost:4566 s3 ls
```

---

## Works with your existing code

awsmock listens on the same port and speaks the same protocol as real AWS.
Change one line — the endpoint URL — and your existing SDK code works as-is.

**Python (boto3)**

```python
import boto3

s3 = boto3.client("s3", endpoint_url="http://localhost:4566")
s3.create_bucket(Bucket="my-bucket")
s3.put_object(Bucket="my-bucket", Key="hello.txt", Body=b"Hello, world!")
```

**Java (AWS SDK v2)**

```java
S3Client s3 = S3Client.builder()
    .endpointOverride(URI.create("http://localhost:4566"))
    .region(Region.US_EAST_1)
    .build();

s3.createBucket(r -> r.bucket("my-bucket"));
```

**AWS CLI**

```bash
aws --endpoint-url=http://localhost:4566 sqs create-queue --queue-name my-queue
aws --endpoint-url=http://localhost:4566 dynamodb list-tables
aws --endpoint-url=http://localhost:4566 s3 cp file.txt s3://my-bucket/
```

---

## Supported Services

| Service             | Status |
|---------------------|--------|
| S3                  | ✅      |
| SQS                 | ✅      |
| SNS                 | ✅      |
| DynamoDB            | ✅      |
| Lambda              | ✅      |
| Cognito             | ✅      |
| API Gateway         | ✅      |
| Secrets Manager     | ✅      |
| KMS                 | ✅      |
| SSM Parameter Store | ✅      |
| Transfer (FTP → S3) | ✅      |

Full API coverage details are in the [docs](https://github.com/jensvogt/awsmock/blob/main/docs/man).

---

## Persistence modes

**In-memory** (default) — fastest, perfect for unit/integration tests, data lost on shutdown.

**Persistent** (MongoDB) — survives restarts, ideal for shared dev environments or realistic local stacks.

```bash
# Run with persistence
docker run \
  -p 4566-4568:4566-4568 \
  -e AWSMOCK_MONGODB_HOST=your-mongo-host \
  jensvogt/awsmock:latest
```

---

## Docker (full options)

```bash
docker run \
  -p 4566-4568:4566-4568 \
  -p 2121:2121 \
  -p 6000-6100:6000-6100 \
  -v /var/run/docker.sock:/var/run/docker.sock \
  jensvogt/awsmock:latest
```

| Port      | Purpose                |
|-----------|------------------------|
| 4566      | Main AWS API endpoint  |
| 4567      | Web UI                 |
| 4568      | Management API         |
| 2121      | FTP (Transfer service) |
| 6000–6100 | Lambda execution       |

---

## Tooling

awsmock ships with three ways to manage your local infrastructure:

- **`awsmockctl`** — CLI tool to start/stop/configure individual services
- **Angular Web UI** — browser dashboard at `http://localhost:4567`
- **Qt Desktop UI** — native app for Windows, macOS, Linux → [awsmock-qt-ui](https://github.com/jensvogt/awsmock-qt-ui)

---

## Installation

### Docker (recommended)

```bash
docker pull jensvogt/awsmock:latest
```

### Debian / Ubuntu

```bash
wget https://jensvogt.github.io/awsmock/awsmock-<version>-amd64.deb
sudo apt install ./awsmock-<version>-amd64.deb
```

### RPM (RHEL / Fedora)

```bash
wget https://jensvogt.github.io/awsmock/awsmock-<version>.rpm
sudo rpm -i awsmock-<version>.rpm
```

### macOS

```bash
wget https://jensvogt.github.io/awsmock/awsmock-<version>-macos.tgz
tar -xzf awsmock-<version>-macos.tgz
```

### Windows

```bash
wget https://jensvogt.github.io/awsmock/awsmock-<version>-amd64.zip
unzip awsmock-<version>-amd64.zip
```

### Build from source

```bash
git clone https://github.com/jensvogt/awsmock.git
cd awsmock
cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path-to-install>
cmake --build .
```

Requires: GCC 4.9+ or Clang 3.3+, CMake, Boost, Beast.
See [build docs](https://github.com/jensvogt/awsmock/blob/main/docs/man) for full dependency list.

---

## Use cases

- **Integration testing** — deterministic, fast, no cloud dependencies
- **CI pipelines** — lightweight container, no AWS credentials needed
- **Offline development** — work on planes, in basements, anywhere
- **Event-driven systems** — wire up S3 → SQS → Lambda locally end-to-end
- **Cost control** — eliminate accidental dev/test AWS charges entirely

---

## Architecture

- Written in **modern C++** using Boost/Beast for HTTP
- Optional **MongoDB** persistence layer
- Modular design — each AWS service is an independent module
- Docker-native, also runs as a system service on Linux/macOS/Windows

---

## Contributing

Contributions are welcome — especially:

- New AWS service support
- Improved API coverage for existing services
- Bug reports with reproduction steps

Open an issue or PR. Real-world usage drives the roadmap.

---

## License

[GPL-3.0](LICENSE)

---

⭐ **If awsmock saves you time or money, a star helps others find it.**
