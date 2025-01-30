//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/core/CryptoUtils.h>

namespace AwsMock::Core {

    unsigned int Crypto::_salt[] = {1214370622, 264849915};

    std::string Crypto::GetMd5FromString(const std::string &content) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;

        EVP_DigestInit_ex(context, md, nullptr);
        EVP_DigestUpdate(context, content.data(), content.length());
        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetMd5FromString(const unsigned char *content) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;

        EVP_DigestInit_ex(context, md, nullptr);
        EVP_DigestUpdate(context, content, sizeof(content));
        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetMd5FromFile(const std::string &fileName) {

        const auto buffer = new char[AWSMOCK_BUFFER_SIZE];

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        std::string output;

        EVP_DigestInit_ex(context, md, nullptr);

        std::ifstream ifs(fileName, std::ios::binary);
        while (ifs.good()) {
            ifs.read(buffer, AWSMOCK_BUFFER_SIZE);
            EVP_DigestUpdate(context, buffer, ifs.gcount());
        }
        ifs.close();

        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] buffer;

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetSha1FromString(const std::string &content) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_sha1();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        std::string output;

        EVP_DigestInit_ex(context, md, nullptr);
        EVP_DigestUpdate(context, content.c_str(), content.length());
        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetSha1FromFile(const std::string &fileName) {

        const auto buffer = new char[AWSMOCK_BUFFER_SIZE];

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_sha1();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        std::string output;

        EVP_DigestInit_ex(context, md, nullptr);

        std::ifstream is;
        is.open(fileName.c_str(), std::ios::binary);
        while (is.good()) {
            is.read(buffer, AWSMOCK_BUFFER_SIZE);
            EVP_DigestUpdate(context, buffer, is.gcount());
        }
        is.close();

        // Finalize
        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] buffer;

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetSha256FromString(const std::string &content) {

        EVP_MD_CTX *context = EVP_MD_CTX_create();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        std::string output;

        EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
        EVP_DigestUpdate(context, content.c_str(), content.size());
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetHmacSha224FromString(const std::string &key, const std::string &msg) {

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
        unsigned int hashLen;

        HMAC(EVP_sha224(),
             msg.data(),
             static_cast<int>(msg.size()),
             reinterpret_cast<unsigned char const *>(key.data()),
             static_cast<int>(key.size()),
             hash.data(),
             &hashLen);

        return HexEncode(hash.data(), static_cast<int>(hashLen));
    }

    std::string Crypto::GetHmacSha384FromString(const std::string &key, const std::string &msg, unsigned int *hashLen) {

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};

        HMAC(EVP_sha384(),
             msg.data(),
             static_cast<int>(msg.size()),
             reinterpret_cast<unsigned char const *>(key.data()),
             static_cast<int>(key.size()),
             hash.data(),
             hashLen);

        return HexEncode(hash.data(), static_cast<int>(*hashLen));
    }

    std::string Crypto::GetHmacSha512FromString(const std::string &key, const std::string &msg, unsigned int *hashLen) {

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};

        HMAC(EVP_sha512(),
             msg.data(),
             static_cast<int>(msg.size()),
             reinterpret_cast<unsigned char const *>(key.data()),
             static_cast<int>(key.size()),
             hash.data(),
             hashLen);

        return HexEncode(hash.data(), static_cast<int>(*hashLen));
    }

    std::string Crypto::GetHmacSha256FromString(const std::string &key, const std::string &msg) {

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
        unsigned int hashLen;

        HMAC(EVP_sha256(),
             msg.data(),
             static_cast<int>(msg.size()),
             reinterpret_cast<unsigned char const *>(key.data()),
             static_cast<int>(key.size()),
             hash.data(),
             &hashLen);

        return HexEncode(hash.data(), static_cast<int>(hashLen));
    }

    std::string Crypto::GetSha256FromFile(const std::string &fileName) {

        const auto buffer = new char[AWSMOCK_BUFFER_SIZE];

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_sha256();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        std::string output;

        EVP_DigestInit_ex(context, md, nullptr);

        std::ifstream is;
        is.open(fileName.c_str(), std::ios::binary);
        while (is.good()) {
            is.read(buffer, AWSMOCK_BUFFER_SIZE);
            EVP_DigestUpdate(context, buffer, is.gcount());
        }
        is.close();

        // Finalize
        EVP_DigestFinal_ex(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] buffer;

        return HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string Crypto::GetHmacSha256FromString(const std::array<unsigned char, CRYPTO_HMAC256_BLOCK_SIZE> &key, const std::string &msg) {

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
        unsigned int hashLen;

        HMAC(EVP_sha256(),
             msg.data(),
             static_cast<int>(msg.size()),
             key.data(),
             key.size(),
             hash.data(),
             &hashLen);
        return HexEncode(hash.data(), hash.size());
    }

    std::vector<unsigned char> Crypto::GetHmacSha256FromStringRaw(const std::string &key, const std::string &msg) {

        auto *hash = static_cast<unsigned char *>(malloc(CRYPTO_HMAC256_BLOCK_SIZE));
        unsigned int hashLen;

        HMAC(EVP_sha256(),
             key.data(),
             static_cast<int>(key.size()),
             reinterpret_cast<unsigned char const *>(msg.data()),
             static_cast<int>(msg.size()),
             hash,
             &hashLen);
        std::vector<unsigned char> result = {hash, hash + hashLen};
        free(hash);
        return result;
    }

    std::vector<unsigned char> Crypto::GetHmacSha256FromStringRaw(const std::vector<unsigned char> &key, const std::string &msg) {

        auto *hash = static_cast<unsigned char *>(malloc(CRYPTO_HMAC256_BLOCK_SIZE));
        unsigned int hashLen;

        HMAC(EVP_sha256(),
             key.data(),
             static_cast<int>(key.size()),
             reinterpret_cast<unsigned char const *>(msg.data()),
             static_cast<int>(msg.size()),
             hash,
             &hashLen);
        std::vector<unsigned char> result = {hash, hash + hashLen};
        free(hash);
        return result;
    }

    void Crypto::CreateAes256Key(unsigned char *key, unsigned char *iv) {

        if (RAND_bytes(key, CRYPTO_AES256_KEY_SIZE) < 0) {
            log_error << "Failed to generate 256bit random key";
        }
        if (RAND_bytes(iv, CRYPTO_AES256_BLOCK_SIZE) < 0) {
            log_error << "Failed to generate 128bit random iv";
        }
    }

    unsigned char *Crypto::Aes256EncryptString(unsigned char *plaintext, int *len, unsigned char *key) {

        // "opaque" encryption, decryption ctx structures that libcrypto uses to record status of enc/dec operations
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

        auto *key_data = key;
        int key_data_len = (int) strlen(reinterpret_cast<const char *>(key_data));

        if (Aes256EncryptionInit(key_data, key_data_len, (unsigned char *) &_salt, ctx)) {
            log_error << "Couldn't initialize AES256 cipher";
            return {};
        }

        /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
        int c_len = *len + CRYPTO_AES256_BLOCK_SIZE, f_len = 0;
        auto *ciphertext = static_cast<unsigned char *>(malloc(c_len));

        // Allows reusing of 'ctx' for multiple encryption cycles
        EVP_EncryptInit_ex(ctx, nullptr, nullptr, nullptr, nullptr);
        EVP_EncryptUpdate(ctx, ciphertext, &c_len, plaintext, *len);
        EVP_EncryptFinal_ex(ctx, ciphertext + c_len, &f_len);

        EVP_CIPHER_CTX_free(ctx);

        *len = c_len + f_len;
        return ciphertext;
    }

    unsigned char *Crypto::Aes256DecryptString(const unsigned char *ciphertext, int *len, unsigned char *key) {

        // Opaque encryption, decryption ctx structures that libcrypto uses to record status of enc/dec operations
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

        auto *key_data = key;

        if (const int key_data_len = static_cast<int>(strlen(reinterpret_cast<const char *>(key_data))); Aes256DecryptionInit(key_data, key_data_len, reinterpret_cast<unsigned char *>(&_salt), ctx)) {
            log_error << "Couldn't initialize AES cipher";
            return {};
        }

        const int c_len = *len;
        // Max ciphertext len for n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes
        int f_len = 0, p_len = 0;
        auto *plaintext = static_cast<unsigned char *>(malloc(c_len));

        EVP_DecryptInit_ex(ctx, nullptr, nullptr, nullptr, nullptr);
        EVP_DecryptUpdate(ctx, plaintext, &p_len, ciphertext, c_len);
        p_len = *len;
        EVP_DecryptFinal(ctx, plaintext + c_len, len);
        p_len += *len;

        EVP_CIPHER_CTX_free(ctx);

        plaintext[p_len] = '\0';
        return plaintext;
    }

    void Crypto::Aes256EncryptFile(const std::string &filename, unsigned char *key) {

        // "opaque" encryption, decryption ctx structures that libcrypto uses to record status of enc/dec operations
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

        auto *key_data = key;

        if (int key_data_len = static_cast<int>(strlen(reinterpret_cast<const char *>(key_data))); Aes256EncryptionInit(key_data, key_data_len, reinterpret_cast<unsigned char *>(&_salt), ctx)) {
            log_error << "Couldn't initialize AES256 cipher";
        }

        std::string outFilename = filename + ".aes256";
        std::ifstream input_file(filename, std::ios::binary);
        std::ofstream output_file(outFilename, std::ios::binary);
        int outFileLen = 0;

        // max ciphertext len for n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes
        auto in_buf = new char[AWSMOCK_BUFFER_SIZE];
        auto out_buf = new char[AWSMOCK_BUFFER_SIZE + AES_BLOCK_SIZE];

        // Allows reusing of 'ctx' for multiple encryption cycles
        EVP_EncryptInit_ex(ctx, nullptr, nullptr, nullptr, nullptr);

        while (!input_file.eof()) {
            input_file.read(in_buf, AWSMOCK_BUFFER_SIZE);
            if (std::streamsize dataSize = input_file.gcount(); 1 == EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(out_buf), &outFileLen, reinterpret_cast<const unsigned char *>(in_buf), (int) dataSize)) {
                output_file.write(out_buf, outFileLen);
                log_trace << "AES256 encryption of file, written: " << outFileLen;
            } else {
                int count = 0;
                log_error << "Encryption failed after " << count << " chunk";
                break;
            }
        }
        EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(out_buf), &outFileLen);
        EVP_CIPHER_CTX_free(ctx);
        input_file.close();
        output_file.close();
        free(in_buf);
        free(out_buf);

        FileUtils::MoveTo(outFilename, filename);
    }

    void Crypto::Aes256DecryptFile(const std::string &filename, std::string &outFilename, unsigned char *key) {

        // "opaque" encryption, decryption ctx structures that libcrypto uses to record status of enc/dec operations
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

        auto *key_data = key;

        if (int key_data_len = static_cast<int>(strlen(reinterpret_cast<const char *>(key_data))); Aes256DecryptionInit(key_data, key_data_len, reinterpret_cast<unsigned char *>(&_salt), ctx)) {
            log_error << "Couldn't initialize AES256 cipher";
        }

        outFilename = filename + ".plain";
        std::ifstream input_file(filename, std::ios::binary);
        std::ofstream output_file(outFilename, std::ios::binary);
        int outFileLen = 0;

        // max ciphertext len for n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes
        auto in_buf = new char[AWSMOCK_BUFFER_SIZE];
        auto out_buf = new char[AWSMOCK_BUFFER_SIZE + AES_BLOCK_SIZE];

        // Allows reusing of 'ctx' for multiple encryption cycles
        EVP_DecryptInit_ex(ctx, nullptr, nullptr, nullptr, nullptr);

        while (!input_file.eof()) {
            input_file.read(in_buf, AWSMOCK_BUFFER_SIZE);
            if (std::streamsize dataSize = input_file.gcount(); 1 == EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(out_buf), &outFileLen, reinterpret_cast<const unsigned char *>(in_buf), static_cast<int>(dataSize))) {
                output_file.write(out_buf, outFileLen);
                log_trace << "AES256 decryption of file, written: " << outFileLen;
            } else {
                int count = 0;
                log_error << "Decryption failed after " << count << " chunk";
                break;
            }
        }
        EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(out_buf), &outFileLen);
        EVP_CIPHER_CTX_free(ctx);
        input_file.close();
        output_file.close();
        free(in_buf);
        free(out_buf);
    }

    int Crypto::Aes256EncryptionInit(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *ctx) {

        int nrounds = 5;
        unsigned char key[CRYPTO_AES256_KEY_SIZE], iv[CRYPTO_AES256_BLOCK_SIZE];

        // Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material. nrounds is the number of times
        // we hash the material. More rounds are more secure but slower.
        if (const int i = EVP_BytesToKey(EVP_aes_256_gcm(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv); i != 32) {
            log_error << "Key size is " << i << " bits - should be 256 bits";
            return -1;
        }

        EVP_CIPHER_CTX_init(ctx);
        EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv);

        return 0;
    }

    int Crypto::Aes256DecryptionInit(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *ctx) {

        int nrounds = 5;
        unsigned char key[CRYPTO_AES256_KEY_SIZE], iv[CRYPTO_AES256_BLOCK_SIZE];

        // Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material. nrounds is the number of times,
        // we hash the material. More rounds are more secure but slower.
        if (const int i = EVP_BytesToKey(EVP_aes_256_gcm(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv); i != 32) {
            log_error << "Key size is " << i << " bits - should be 256 bits";
            return -1;
        }

        EVP_CIPHER_CTX_init(ctx);
        EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv);

        return 0;
    }

    void Crypto::CreateHmacKey(unsigned char *key, int length) {

        if (RAND_bytes(key, length) < 0) {
            log_error << "Failed to generate HMAC random key, length: " << length;
        }
    }

    std::string Crypto::Base64Encode(const std::string &inputString) {
        std::stringstream os;
        typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<const char *, 6, 8>> base64_enc;
        const std::string base64_padding[] = {"", "==", "="};
        std::copy(base64_enc(inputString.c_str()), base64_enc(inputString.c_str() + inputString.size()), std::ostream_iterator<char>(os));
        os << base64_padding[inputString.size() % 3];
        return os.str();
    }

    std::string Crypto::Base64Decode(const std::string &encodedString) {
        std::string output;
        std::string input = encodedString;
        using namespace boost::archive::iterators;
        typedef remove_whitespace<std::string::const_iterator> StripIt;
        typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> ItBinaryT;
        try {
            /// Trailing whitespace makes remove_whitespace barf because the iterator never == end().
            while (!input.empty() && std::isspace(input.back())) { input.pop_back(); }
            //inputString.swap(StripIt(inputString.begin()), StripIt(inputString.end()));
            /// If the input isn't a multiple of 4, pad with =
            input.append((4 - input.size() % 4) % 4, '=');
            const size_t pad_chars(std::count(input.end() - 4, input.end(), '='));
            std::replace(input.end() - 4, input.end(), '=', 'A');
            output.clear();
            output.reserve(input.size() * 1.3334);
            output.assign(ItBinaryT(input.begin()), ItBinaryT(input.end()));
            output.erase(output.end() - (pad_chars < 2 ? pad_chars : 2), output.end());
        } catch (std::exception const &) {
            output.clear();
        }
        return output;
    }

    bool Crypto::IsBase64(const std::string &inputString) {
        if (inputString.length() % 4 == 0 && std::ranges::all_of(inputString,
                                                                 [](const char c) { return ((c >= 'a' && c <= 'z') ||
                                                                                            (c >= 'A' && c <= 'Z') ||
                                                                                            (c >= '0' && c <= '9') ||
                                                                                            c == '/' ||
                                                                                            c == '+' ||
                                                                                            c == '='); })) {
            // filter by the location of '=' sign.
            if (const auto pos = inputString.find("==="); pos != std::string_view::npos) {
                if (pos < inputString.length() - 3) return false;
            } else if (const auto pos1 = inputString.find("=="); pos1 != std::string_view::npos) {
                if (pos1 < inputString.length() - 2) return false;
            } else if (const auto pos2 = inputString.find("="); pos2 != std::string_view::npos) {
                if (pos2 < inputString.length() - 1) return false;
            }
            return true;
        }
        return false;
    }

    std::string Crypto::HexEncode(const std::string &input) {
        return boost::algorithm::hex(input);
    }

    std::string Crypto::HexEncode(const unsigned char *hash, const int size) {
        std::stringstream ss;
        for (int i = 0; i < size; ++i)
            ss << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(hash[i]);
        return ss.str();
    }

    std::string Crypto::HexEncode(const std::vector<unsigned char> &digest) {
        std::string sResult;
        sResult.reserve(digest.size() * 2 + 1);
        for (const unsigned char it: digest) {
            static constexpr char dec2hex[16 + 1] = "0123456789abcdef";
            sResult += dec2hex[(it >> 4) & 15];
            sResult += dec2hex[it & 15];
        }
        return sResult;
    }

    unsigned char *Crypto::HexDecode(const std::string &hex) {
        long len;
        unsigned char *out = OPENSSL_hexstr2buf(hex.data(), &len);
        out[len] = '\0';
        return out;
    }

    EVP_PKEY *Crypto::GenerateRsaKeys(unsigned int keyLength) {
        EVP_PKEY *pRSA = EVP_RSA_gen(keyLength);
        if (!pRSA) {
            log_error << "Could not generate RSA key, length: " << keyLength;
            return nullptr;
        }
        return pRSA;
    }

    std::string Crypto::GetRsaPublicKey(EVP_PKEY *pRSA) {

        BIO *bp = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bp, pRSA);

        int size;
        const auto buf = static_cast<char *>(malloc(CRYPTO_RSA_KEY_LINE_LENGTH));
        std::ostringstream sstream;
        do {
            size = BIO_gets(bp, buf, CRYPTO_RSA_KEY_LINE_LENGTH);
            sstream << buf;
        } while (size > 0);
        free(buf);

        return sstream.str();
    }

    std::string Crypto::GetRsaPrivateKey(EVP_PKEY *pRSA) {

        BIO *bp = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bp, pRSA);

        int size;
        const auto buf = static_cast<char *>(malloc(CRYPTO_RSA_KEY_LINE_LENGTH));
        std::ostringstream sstream;
        PEM_write_bio_PrivateKey(bp, pRSA, nullptr, nullptr, 0, nullptr, nullptr);
        do {
            size = BIO_gets(bp, buf, CRYPTO_RSA_KEY_LINE_LENGTH);
            sstream << buf;
        } while (size > 0);
        free(buf);

        return sstream.str();
    }

    EVP_PKEY *Crypto::ReadRsaPrivateKey(const std::string &inKey) {

        BIO *bo = BIO_new(BIO_s_mem());
        BIO_write(bo, inKey.c_str(), static_cast<int>(inKey.length()));

        EVP_PKEY *pKey = nullptr;
        PEM_read_bio_PrivateKey(bo, &pKey, nullptr, nullptr);

        BIO_free(bo);
        return pKey;
    }

    EVP_PKEY *Crypto::ReadRsaPublicKey(const std::string &publicKey) {

        EVP_PKEY *pKey = nullptr;
        BIO *bo2 = BIO_new(BIO_s_mem());
        BIO_write(bo2, publicKey.c_str(), static_cast<int>(publicKey.length()));
        PEM_read_bio_PUBKEY(bo2, &pKey, nullptr, nullptr);
        BIO_free(bo2);

        return pKey;
    }

    std::string Crypto::RsaEncrypt(EVP_PKEY *keyPair, const std::string &in) {

        size_t outLen;
        const auto inLen = static_cast<size_t>(in.length());
        const auto bytes = reinterpret_cast<const unsigned char *>(in.c_str());

        EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(keyPair, nullptr);
        EVP_PKEY_encrypt_init(ctx);
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);

        // Get output length
        if (EVP_PKEY_encrypt(ctx, nullptr, &outLen, bytes, inLen) <= 0) {
            log_error << "Could not get length of encrypted string, error: " << ERR_error_string(ERR_get_error(), nullptr);
        };

        auto *encrypt = static_cast<unsigned char *>(OPENSSL_malloc(outLen));
        if (EVP_PKEY_encrypt(ctx, encrypt, &outLen, bytes, inLen) <= 0) {
            log_error << "Could not encrypt string, error: " << ERR_error_string(ERR_get_error(), nullptr);
        }
        EVP_PKEY_CTX_free(ctx);
        return {reinterpret_cast<const char *>(encrypt), outLen};
    }

    std::string Crypto::RsaDecrypt(EVP_PKEY *keyPair, const std::string &in) {

        size_t outLen;
        const int inLen = static_cast<int>(in.length());
        const auto *bytes = (unsigned char *) in.c_str();

        EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(keyPair, nullptr);
        EVP_PKEY_decrypt_init(ctx);
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);

        // Get output length
        if (EVP_PKEY_decrypt(ctx, nullptr, &outLen, bytes, inLen) <= 0) {
            log_error << "Could not get length of encrypted string, error: " << ERR_error_string(ERR_get_error(), nullptr);
        };

        auto *decrypt = static_cast<unsigned char *>(OPENSSL_malloc(outLen));
        if (EVP_PKEY_decrypt(ctx, decrypt, &outLen, bytes, inLen) <= 0) {
            log_error << "Could not decrypt string, error: " << ERR_error_string(ERR_get_error(), nullptr);
        }
        EVP_PKEY_CTX_free(ctx);
        return {reinterpret_cast<char *>(decrypt), outLen};
    }

}// namespace AwsMock::Core