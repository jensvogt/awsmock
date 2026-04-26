# awsmock

> ⚡ High-performance AWS mock server for fast local development

awsmock is a **lightweight, persistent AWS simulation** built in C++ for developers who want fast, reliable local
testing without real AWS infrastructure.

---

## 🚀 Why awsmock?

Working with AWS locally is often:

* 🐢 Slow (startup time, network latency)
* 💸 Expensive (even in dev/test)
* 🧱 Complex (IAM, infra setup, cloud dependencies)

**awsmock solves this.**

✔ Instant startup
✔ Low memory footprint
✔ Works offline
✔ Persistent or in-memory modes
✔ Designed for local dev & CI

---

## ⚡ Quick Start (30 seconds)

```bash
docker run -p 4566-4568:4566-4568 jensvogt/awsmock:latest
```

Then point your AWS SDK or CLI to:

```text
http://localhost:4566
```

Done.

---

## 🧪 Example

### S3

```bash
aws --endpoint-url=http://localhost:4566 s3 mb s3://test-bucket
aws --endpoint-url=http://localhost:4566 s3 cp file.txt s3://test-bucket
```

### SQS

```bash
aws --endpoint-url=http://localhost:4566 sqs create-queue --queue-name test-queue
```

---

## 🧩 Supported Services

* S3
* SQS
* SNS *(SQS endpoint support)*
* Lambda
* DynamoDB
* Cognito
* API Gateway
* Secrets Manager
* KMS
* SSM (Parameter Store)
* Transfer (FTP → S3)
* Application management

👉 See detailed API coverage in the [docs](docs/man).

---

## 🆚 Why awsmock?

| Feature         | awsmock                |
|-----------------|------------------------|
| Language        | C++ (high performance) |
| Startup time    | ⚡ Fast                 |
| Memory usage    | 🟢 Low                 |
| Persistence     | Optional (MongoDB)     |
| Offline support | ✅                      |
| CI-friendly     | ✅                      |

awsmock is built for **speed and simplicity**, not full AWS replication.

---

## 🧠 When to use awsmock

Use awsmock if you want:

* Fast local AWS simulation
* Deterministic integration tests
* Offline development
* Lightweight CI environments
* Persistent mock infrastructure (via MongoDB)

---

## 💾 Persistence vs In-Memory

awsmock supports two modes:

### In-Memory (default)

* Fastest
* Perfect for tests
* Data is lost on shutdown

### Persistent (MongoDB)

* Stores buckets, queues, metadata
* Survives restarts
* Ideal for realistic environments

---

## 🐳 Docker (recommended)

```bash
docker pull jensvogt/awsmock:latest

docker run \
  -p 4566-4568:4566-4568 \
  -p 2121:2121 \
  -p 6000-6100:6000-6100 \
  -v /var/run/docker.sock:/var/run/docker.sock \
  jensvogt/awsmock:latest
```

---

## 🧪 Use Cases

* Local AWS development
* Integration testing
* Event-driven systems (S3 → SQS → Lambda)
* Debugging without cloud latency
* CI pipelines without AWS dependencies

---

## 🛠 Tooling

awsmock includes:

* `awsmockctl` → control & configure services
* Angular web UI → manage infrastructure (`http://localhost:4567`)
* Qt desktop UI → [awsmock-qt-ui](https://github.com/jensvogt/awsmock-qt-ui)

---

## 🏗 Architecture Highlights

* Written in **modern C++**
* Uses **Boost / Beast**
* Optional **MongoDB persistence**
* Docker-native execution
* Modular service design

---

## 📦 Installation

### Docker (recommended)

```bash
docker pull jensvogt/awsmock:latest
```

---

## 🤝 Contributing

Contributions are welcome:

* Add AWS service support
* Improve API coverage
* Fix edge cases

---

## ⭐ If this helps you

Give the repo a star — it helps others discover it.

---

## 📬 Feedback

Open an issue or discussion — real-world usage drives the roadmap.
