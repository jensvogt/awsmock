//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_CORE_CRYPTO_UTILS_TEST_H
#define AWSMOCK_CORE_CRYPTO_UTILS_TEST_H

// Boost includes
#include <boost/locale/config.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

// Local includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>

#define TEST_STRING "The quick brown fox jumps over the lazy dog"
#define BASE64_TEST_STRING "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw=="
#define TEST_STRING1 "{\"id\":\"DLI393_9783836272926\",\"artikelTyp\":\"physisch\",\"ursprungsDatei\":\"DLI393/20231113_RheinwerkVerlag_10329_13112023131739313.xml\",\"ursprungsFormat\":\"Onix2.1\",\"standDatum\":\"2023-11-13T00:00:00\",\"lieferantenId\":\"DLI393\"}"
#define MD5_SUM "d41d8cd98f00b204e9800998ecf8427e"
#define MD5_SUM1 "5e39cce4e34eb60f350d1cf2f1098166"
#define MD5_SUM2 "9e107d9d372bb6826bd81d3542a419d6"
#define MD5_SUM3 "9e107d9d372bb6826bd81d3542a419d6"
#define SHA1_SUM "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"
#define SHA256_SUM "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"
#define SHA256_SUM_EMPTY "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(Md5StringTest) {

        // arrange

        // act
        const std::string result = Crypto::GetMd5FromString(TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, MD5_SUM2);
    }

    BOOST_AUTO_TEST_CASE(Md5UtfStringTest) {

        // arrange
        auto *input = static_cast<unsigned char *>(malloc(sizeof(TEST_STRING1)));
        memcpy(input, TEST_STRING1, sizeof(TEST_STRING1));

        // act
        std::string utf8_string = boost::locale::conv::to_utf<char>(TEST_STRING1, "Latin1");
        const std::string result = Crypto::GetMd5FromString(input);

        // assert
        BOOST_CHECK_EQUAL(result, MD5_SUM1);
    }

    BOOST_AUTO_TEST_CASE(Md5FileTest) {

        // arrange
        const std::string file = FileUtils::CreateTempFile("txt", TEST_STRING);

        // act
        const std::string result = Crypto::GetMd5FromFile(file);

        // assert
        BOOST_CHECK_EQUAL(result, MD5_SUM3);
    }

    BOOST_AUTO_TEST_CASE(Md5DoubleFileTest) {

        // arrange
        const std::string file1 = FileUtils::CreateTempFile("txt", TEST_STRING);
        const std::string file2 = FileUtils::CreateTempFile("txt", TEST_STRING);

        // act
        const std::string result1 = Crypto::GetMd5FromFile(file1);
        const std::string result2 = Crypto::GetMd5FromFile(file2);

        // assert
        BOOST_CHECK_EQUAL(result1, result2);
    }


    BOOST_AUTO_TEST_CASE(Sha1StringTest) {
        // arrange

        // act
        const std::string result = Crypto::GetSha1FromString(TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, SHA1_SUM);
    }

    BOOST_AUTO_TEST_CASE(Sha1FileTest) {

        // arrange
        const std::string file = FileUtils::CreateTempFile("txt", TEST_STRING);

        // act
        const std::string result = Crypto::GetSha1FromFile(file);

        // assert
        BOOST_CHECK_EQUAL(result, SHA1_SUM);
    }

    BOOST_AUTO_TEST_CASE(Sha256StringTest) {

        // arrange

        // act
        const std::string result = Crypto::GetSha256FromString(TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, SHA256_SUM);
    }

    BOOST_AUTO_TEST_CASE(Sha256EmptyStringTest) {

        // arrange

        // act
        const std::string result = Crypto::GetSha256FromString("");

        // assert
        BOOST_CHECK_EQUAL(result, SHA256_SUM_EMPTY);
    }

    BOOST_AUTO_TEST_CASE(Sha256FileTest) {

        // arrange
        const std::string file = FileUtils::CreateTempFile("txt", TEST_STRING);

        // act
        const std::string result = Crypto::GetSha256FromFile(file);

        // assert
        BOOST_CHECK_EQUAL(result, SHA256_SUM);
    }

    BOOST_AUTO_TEST_CASE(GetHmacSha256Test) {

        // arrange

        // act
        const std::string result = Crypto::GetHmacSha256FromString("secretKey", TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, "6a7323506a6493e320d27b6eb5c64e722a314e15ddb753c837738e0c174cdb03");
    }

    BOOST_AUTO_TEST_CASE(Base64EncodeTest) {

        // arrange

        // act
        const std::string result = Crypto::Base64Encode(TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, BASE64_TEST_STRING);
    }

    BOOST_AUTO_TEST_CASE(Base64DecodeTest) {

        // arrange

        // act
        const std::string result = Crypto::Base64Decode(BASE64_TEST_STRING);

        // assert
        BOOST_CHECK_EQUAL(result, TEST_STRING);
    }

    BOOST_AUTO_TEST_CASE(Base64Test) {

        // arrange
        const std::string testText = "Base64 hashing";

        // act
        const std::string encrypted = Crypto::Base64Encode(testText);
        const std::string decrypted = Crypto::Base64Decode(encrypted);

        // assert
        BOOST_CHECK_EQUAL(testText, decrypted);
    }

    BOOST_AUTO_TEST_CASE(Base64FileDecodeTest) {

        // arrange
        const std::string tempFile = FileUtils::CreateTempFile("b64", 300);

        // act
        Crypto::Base64Decode(BASE64_TEST_STRING, tempFile);

        // assert
        const long size = FileUtils::FileSize(tempFile);
        BOOST_CHECK_EQUAL(43, size);
    }

    BOOST_AUTO_TEST_CASE(Aes256KeyText) {

        // arrange
        unsigned char key[CRYPTO_AES256_KEY_SIZE];
        unsigned char iv[CRYPTO_AES256_BLOCK_SIZE];

        // act
        Crypto::CreateAes256Key(key, iv);

        // assert
        BOOST_CHECK_EQUAL(sizeof(key) / sizeof(key[0]), CRYPTO_AES256_KEY_SIZE);
        BOOST_CHECK_EQUAL(sizeof(iv) / sizeof(iv[0]), CRYPTO_AES256_BLOCK_SIZE);
    }

    BOOST_AUTO_TEST_CASE(Aes256EncryptionText) {

        // arrange
        const std::string testText = "This is a super secure text";
        const std::string key = "TestKey";
        int len = static_cast<int>(testText.length());

        // act
        const auto cyphertext = static_cast<unsigned char *>(malloc(testText.length() * 2));
        Crypto::Aes256EncryptString(reinterpret_cast<const unsigned char *>(testText.c_str()), &len, reinterpret_cast<const unsigned char *>(key.c_str()), cyphertext);

        const auto plaintext = static_cast<unsigned char *>(malloc(testText.length() * 2));
        Crypto::Aes256DecryptString(cyphertext, &len, reinterpret_cast<const unsigned char *>(key.c_str()), plaintext);
        plaintext[len] = '\0';

        // assert
        BOOST_CHECK_EQUAL(reinterpret_cast<const char *>(plaintext), testText.c_str());
    }

    BOOST_AUTO_TEST_CASE(GenerateRsaKeyTest) {

        // arrange
        const std::string testText = "This_is_a_super_secure_text";

        // Generate key pair and initialize
        EVP_PKEY *keyPair = Crypto::GenerateRsaKeys(4096);

        // act
        // encrypt
        const std::string encrypted = Crypto::RsaEncrypt(keyPair, testText);

        // decrypt
        const std::string decrypted = Crypto::RsaDecrypt(keyPair, encrypted);

        // assert
        BOOST_CHECK_EQUAL(testText, decrypted);
    }

    BOOST_AUTO_TEST_CASE(GetRsaKeyTest) {

        // arrange
        EVP_PKEY *keyPair = Crypto::GenerateRsaKeys(4096);

        // Generate key pair and initialize
        const std::string publicKey = Crypto::GetRsaPublicKey(keyPair);
        const std::string privateKey = Crypto::GetRsaPrivateKey(keyPair);

        // assert
        BOOST_CHECK_EQUAL(publicKey.empty(), false);
        BOOST_CHECK_EQUAL(800, publicKey.size());
        BOOST_CHECK_EQUAL(privateKey.empty(), false);
        BOOST_CHECK_GT(privateKey.size(), 4000);
    }

    BOOST_AUTO_TEST_CASE(HexEncodeDecodeTest) {

        // arrange
        std::string testString = "This is a test string";

        // act
        const std::string encoded = Crypto::HexEncode(reinterpret_cast<unsigned char *>(testString.data()), testString.length());
        const auto decoded = static_cast<unsigned char *>(malloc(encoded.length() * 2));
        Crypto::HexDecode(encoded, decoded);

        // assert
        BOOST_CHECK_EQUAL_COLLECTIONS(testString.c_str(), testString.c_str() + strlen(testString.c_str()), decoded, decoded + strlen((const char *) decoded));
        //BOOST_CHECK_EQUAL(testString, std::string{(const char *) decoded});
    }
}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_CRYPTO_UTILS_TEST_H