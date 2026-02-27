//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWS_MOCK_CORE_CRYPTO_UTILS_H
#define AWS_MOCK_CORE_CRYPTO_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// Standard C++ includes
#include <array>
#include <string>
#include <string_view>

// Openssl includes
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

// Boost includes
#include <boost/algorithm/hex.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/hash2/sha2.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>

// AwsMock includes
#include <awsmock/core/Base64Utils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/RandomUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/logging/LogStream.h>

// 64 kB buffer
#define AWSMOCK_BUFFER_SIZE 8192
#define AWSMOCK_BIG_BUFFER_SIZE 65536

#define CRYPTO_RSA_KEY_LEN_4096 4096
#define CRYPTO_RSA_KEY_LEN_2048 2048
#define CRYPTO_RSA_KEY_LEN_1024 1024
#define CRYPTO_RSA_KEY_EXP 65535
#define CRYPTO_RSA_KEY_LINE_LENGTH 64

#define CRYPTO_AES256_KEY_SIZE 32
#define CRYPTO_AES256_BLOCK_SIZE 16
#define CRYPTO_HMAC224_BLOCK_SIZE 32
#define CRYPTO_HMAC256_BLOCK_SIZE 32
#define SHA256_EMPTY_STRING "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"

namespace AwsMock::Core {

    static const std::string _base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /**
     * @brief Cryptographic utilities like MD5 hash, SHA1, SHA256 etc.
     *
     * @par
     * This class contains AWS cryptographic method as well as OpenSSL methods. AWS cryptographic methods are denoted with a preceding 'AWS'.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Crypto {

      public:

        /**
         * @brief Returns the MD5 hash of a string.
         *
         * @param content string to hash
         * @return MD5 hash of the given string
         */
        static std::string GetMd5FromString(const std::string &content);

        /**
         * @brief Returns the MD5 hash of a string.
         *
         * @param content string to hash
         * @return MD5 hash of the given string
         */
        static std::string GetMd5FromString(const unsigned char *content);

        /**
         * @brief Returns the MD5 hash of a file.
         *
         * @param fileName file name to hash
         * @return MD5 hash of the given file
         */
        static std::string GetMd5FromFile(const std::string &fileName);

        /**
         * @brief Returns the SHA1 hash of a string.
         *
         * @param content string to hash
         * @return SHA1 hash of the given string
         */
        static std::string GetSha1FromString(const std::string &content);

        /**
         * @brief Returns the SHA1 hash of a file.
         *
         * @param fileName file name to hash
         * @return SHA1 hash of the given file
         */
        static std::string GetSha1FromFile(const std::string &fileName);

        /**
         * @brief Returns the SHA256 hash of a string.
         *
         * @param content string to hash
         * @return SHA256 hash of the given string
         */
        static std::string GetSha256FromString(const std::string &content);

        /**
         * @brief Returns the SHA256 hash of a file.
         *
         * @param fileName file name to hash
         * @return SHA256 hash of the given file
         */
        static std::string GetSha256FromFile(const std::string &fileName);

        /**
         * @brief Returns the hex encoded HMAC SHA224 hash of a string.
         *
         * @param key string for hashing
         * @param content string to hash
         * @return hey-encoded HMAC SHA224 hash of the given string
         */
        static std::string GetHmacSha224FromString(const std::string &key, const std::string &content);

        /**
         * @brief Returns the hex encoded HMAC SHA384 hash of a string.
         *
         * @param key string for hashing
         * @param content string to hash
         * @return hey-encoded HMAC SHA284 hash of the given string
         */
        static std::string GetHmacSha384FromString(const std::string &key, const std::string &content);

        /**
         * @brief Returns the hex encoded HMAC SHA512 hash of a string.
         *
         * @param key string for hashing
         * @param content string to hash
         * @return hey-encoded HMAC SHA512 hash of the given string
         */
        static std::string GetHmacSha512FromString(const std::string &key, const std::string &content);

        /**
         * @brief Returns the hex encoded HMAC SHA256 hash of a string.
         *
         * @param key string for hashing
         * @param content string to hash
         * @return SHA256 hash of the given string
         */
        static std::string GetHmacSha256FromString(const std::string &key, const std::string &content);

        /**
         * @brief Returns the hex encoded SHA256 hash of a string.
         *
         * @param key byte array for hashing
         * @param content string to hash
         * @return SHA256 hash of the given string
         */
        static std::string GetHmacSha256FromString(const std::array<unsigned char, CRYPTO_HMAC256_BLOCK_SIZE> &key, const std::string &content);

        /**
         * @brief Returns the SHA256 hash of a string.
         *
         * @param key string for hashing
         * @param msg string to hash
         * @return SHA256 hash of the given string
         */
        static std::vector<unsigned char> GetHmacSha256FromStringRaw(const std::string &key, const std::string &msg);

        /**
         * @brief Returns the SHA256 hash of a string.
         *
         * @param key byte array for hashing
         * @param msg string to hash
         * @return SHA256 hash of the given string
         */
        static std::vector<unsigned char> GetHmacSha256FromStringRaw(const std::vector<unsigned char> &key, const std::string &msg);

        /**
         * @brief Creates a AES256 encryption key.
         *
         * @param key 265bit key material
         * @param iv iv
         */
        static void CreateAes256Key(unsigned char *key, unsigned char *iv);

        /**
         * @brief AES256 encryption of a string
         *
         * @param plaintext input string
         * @param len plaintext length
         * @param key encryption key
         * @param out output string
         */
        static void Aes256EncryptString(const unsigned char *plaintext, int *len, const unsigned char *key, unsigned char *out);

        /**
         * @brief AES256 decryption of a string.
         *
         * @param ciphertext input string
         * @param len ciphertext length
         * @param key encryption key
         * @param out output string
         */
        static void Aes256DecryptString(const unsigned char *ciphertext, int *len, const unsigned char *key, unsigned char *out);

        /**
         * @brief AES256 encryption of a file
         *
         * @par
         * The original plaintext file will be replaced by the encrypted file.
         *
         * @param filename input file name
         * @param key encryption key
         */
        static void Aes256EncryptFile(const std::string &filename, unsigned char *key);

        /**
         * @brief AES256 decryption of a file.
         *
         * @par
         * The actual file will stay encrypted. Outfile must be deleted after is has been send to client.
         *
         * @param filename input file name
         * @param outFilename output filename
         * @param key encryption key
         */
        static void Aes256DecryptFile(const std::string &filename, std::string &outFilename, unsigned char *key);

        /**
         * @brief Creates a HMAC encryption key.
         *
         * @param key key material
         * @param length length of the HMAC key
         */
        static void CreateHmacKey(unsigned char *key, int length);

        /**
         * @brief Base64 encoding.
         *
         * @param decodedString input string
         * @return BASE64 encoded string.
         */
        static std::string Base64Encode(const std::string &decodedString);

        /**
         * @brief Base64 decoding.
         *
         * @param encodedString encoded input string
         * @return BASE64 decoded string.
         */
        static std::string Base64Decode(const std::string &encodedString);

        static void Base64Decode(const std::string &encodedString, const std::string &filename);

        /**
         * @brief Checks whether the input string is Base64 encoded.
         *
         * @param inputString input string
         * @return BASE64 encoded string.
         */
        static bool IsBase64(const std::string &inputString);

        /**
         * @brief Converts the given string to hex encoded string.
         *
         * @param input input byte array
         * @return hex encoded string
         */
        static std::string HexEncode(const std::string &input);

        /**
         * @brief Convert of an unsigned char* array to a hex string
         *
         * @param hash input char array
         * @param size input char length
         * @return hex encoded string
         */
        static std::string HexEncode(const unsigned char *hash, int size);

        /**
         * @brief Convert of an unsigned char* array to a hex string
         *
         * @param digest input char array
         * @return hex encoded string
         */
        static std::string HexEncode(const std::vector<unsigned char> &digest);

        /**
         * @brief Decodes a hex string to an unsigned char array.
         *
         * @param hex hex string
         * @param out output string
         * @return unsigned char array.
         */
        static void HexDecode(const std::string &hex, unsigned char *out);

        /**
         * @brief Generate an RSA key pair of the given length.
         *
         * @param keyLength key length
         * @return RSA key pair.
         */
        static EVP_PKEY *GenerateRsaKeys(unsigned int keyLength);

        /**
         * @brief Converts the public key to a string.
         *
         * @param pRSA pointer to RSA key pair
         */
        static std::string GetRsaPublicKey(const EVP_PKEY *pRSA);

        /**
         * @brief Converts the private key to a string.
         *
         * @param pRSA pointer to RSA key pair
         */
        static std::string GetRsaPrivateKey(const EVP_PKEY *pRSA);

        /**
         * @brief Read an EVP_PKEY from a string.
         *
         * @param privateKey key as string
         * @return *EVP_PKEY
         */
        static EVP_PKEY *ReadRsaPrivateKey(const std::string &privateKey);

        /**
         * @brief Read an EVP_PKEY from a string.
         *
         * @param publicKey key as string
         * @return *EVP_PKEY
         */
        static EVP_PKEY *ReadRsaPublicKey(const std::string &publicKey);

        /**
         * @brief Encrypt a string using RSA encryption.
         *
         * <p>
         * The output string will be Base64 encoded.
         * </p>
         *
         * @param keyPair RSA keypair.
         * @param in ini string
         * @return Base64 encoded string
         */
        static std::string RsaEncrypt(EVP_PKEY *keyPair, const std::string &in);

        /**
         * @brief Decrypts a string using RSA encryption.
         *
         * <p>
         * The input string must be Base64 encoded.
         * </p>
         *
         * @param keyPair RSA keypair.
         * @param in ini string
         * @return Base64 encoded string
         */
        static std::string RsaDecrypt(EVP_PKEY *keyPair, const std::string &in);

      private:

        /**
         * @brief Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
         *
         * Fills in the encryption and decryption ctx objects and returns 0 on success.
         *
         * @param key_data key data
         * @param key_data_len length of key data
         * @param salt salt value
         * @param ctx openssl model
         */
        static int Aes256EncryptionInit(const unsigned char *key_data, int key_data_len, const unsigned char *salt, EVP_CIPHER_CTX *ctx);

        /**
         * @brief Create a 256 bit key and IV using the supplied key_data.
         *
         * salt can be added for taste. Fills in the encryption and decryption ctx objects and returns 0 on success.
         *
         * @param key_data key data
         * @param key_data_len length of key data
         * @param salt salt value
         * @param ctx openssl model
         */
        static int Aes256DecryptionInit(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *ctx);

        /**
         * Salt values
         */
        static unsigned int _salt[];

        /**
         * Salt values
         */
        static unsigned char _iv[16];
    };

}// namespace AwsMock::Core

#endif