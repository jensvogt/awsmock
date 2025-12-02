//
// Created by vogje01 on 3/30/25.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_SERVER_H
#define AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_SERVER_H
/**
 * This is a sample implementation of a libssh based SSH server
 * Copyright 2014 Audrius Butkevicius
 *
 * This file is part of the SSH Library
 *
 * You are free to copy this file, modify it in any way, consider it being public
 * domain. This does not apply to the rest of the library though, but it is
 * allowed to cut-and-paste working code from this file to any license of
 * program.
 *
 * The goal is to show the API in action.
*/

#include <chrono>
#include <sstream>
#include <string>

// Libssh includes
#include <libssh/callbacks.h>
#include <libssh/server.h>
#include <libssh/sftp.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#ifndef _WIN32
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/wait.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <sys/stat.h>

// below are for sftp
#include <cerrno>
#include <cinttypes>
#include <ctime>
#include <dirent.h>
#include <openssl/bn.h>
#include <openssl/opensslv.h>

// Awsmock includes
#include <awsmock/service/s3/S3Service.h>

#ifndef KEYS_FOLDER
#ifdef _WIN32
#include <direct.h>
#define KEYS_FOLDER
#else
#define KEYS_FOLDER "/etc/ssh/"
#endif
#endif

#define BUF_SIZE 1048576
#define SESSION_END (SSH_CLOSED | SSH_CLOSED_ERROR)

#define _DATA_BYTE_CONST(data, pos) ((uint8_t) (((const uint8_t *) (data))[(pos)]))
#define _DATA_BYTE(data, pos) (((uint8_t *) (data))[(pos)])
#define PULL_BE_U8(data, pos) (_DATA_BYTE_CONST(data, pos))
#define PULL_BE_U16(data, pos) ((((uint16_t) (PULL_BE_U8(data, pos))) << 8) | (uint16_t) PULL_BE_U8(data, (pos) + 1))
#define PULL_BE_U32(data, pos) ((((uint32_t) PULL_BE_U16(data, pos)) << 16) | (uint32_t) (PULL_BE_U16(data, (pos) + 2)))
#define PUSH_BE_U8(data, pos, val) (_DATA_BYTE(data, pos) = ((uint8_t) (val)))
#define PUSH_BE_U16(data, pos, val) (PUSH_BE_U8((data), (pos), (uint8_t) (((uint16_t) (val)) >> 8)), PUSH_BE_U8((data), (pos) + 1, (uint8_t) ((val) & 0xff)))
#define PUSH_BE_U32(data, pos, val) (PUSH_BE_U16((data), (pos), (uint16_t) (((uint32_t) (val)) >> 16)), PUSH_BE_U16((data), (pos) + 2, (uint16_t) ((val) & 0xffff)))
#define PUSH_BE_U64(data, pos, val) (PUSH_BE_U32((data), (pos), (uint32_t) (((uint64_t) (val)) >> 32)), PUSH_BE_U32((data), (pos) + 4, (uint32_t) ((val) & 0xffffffff)))

#ifndef MAX_PACKET_LEN
#define MAX_PACKET_LEN 262144
#endif
#ifndef ERROR_BUFFERLEN
#define ERROR_BUFFERLEN 1024
#endif

#ifndef LOG_SIZE
#define LOG_SIZE 1024
#endif
#ifndef UNUSED_PARAM
#define UNUSED_PARAM(param) param __unused__
#endif /* UNUSED_PARAM */

/* Buffer size maximum is 256M */
#define BUFFER_SIZE_MAX 0x10000000
#ifdef DEBUG_BUFFER
/**
 * @internal
 *
 * @brief Check that preconditions and postconditions are valid.
 *
 * @param[in]  buf      The buffer to check.
 */
static void buffer_verify(ssh_buffer buf) {
    bool do_abort = false;

    if (buf->data == NULL) {
        return;
    }

    if (buf->used > buf->allocated) {
        fprintf(stderr,
                "BUFFER ERROR: allocated %u, used %u\n",
                buf->allocated,
                buf->used);
        do_abort = true;
    }
    if (buf->pos > buf->used) {
        fprintf(stderr,
                "BUFFER ERROR: position %u, used %u\n",
                buf->pos,
                buf->used);
        do_abort = true;
    }
    if (buf->pos > buf->allocated) {
        fprintf(stderr,
                "BUFFER ERROR: position %u, allocated %u\n",
                buf->pos,
                buf->allocated);
        do_abort = true;
    }
    if (do_abort) {
        abort();
    }
}

#else
#define buffer_verify(x)
#endif

/* error handling structure */
struct error_struct {
    int error_code;
    char error_buffer[ERROR_BUFFERLEN];
};

extern "C" {

#define SSH_SFTP_CALLBACK(name) static int name(sftp_client_message message)

typedef int (*sftp_server_message_callback)(sftp_client_message message);

struct sftp_message_handler {
    const char *name;
    const char *extended_name;
    uint8_t type;

    sftp_server_message_callback cb;
};

LIBSSH_API int sftp_channel_default_subsystem_request(ssh_session session,
                                                      ssh_channel channel,
                                                      const char *subsystem,
                                                      void *userdata);
LIBSSH_API int sftp_channel_default_data_callback(ssh_session session,
                                                  ssh_channel channel,
                                                  void *data,
                                                  uint32_t len,
                                                  int is_stderr,
                                                  void *userdata);

void _ssh_set_error(void *error, int code, const char *function, const char *descr, ...) PRINTF_ATTRIBUTE(4, 5);
#define ssh_set_error(error, code, ...) _ssh_set_error(error, code, __func__, __VA_ARGS__)

void _ssh_set_error_oom(void *error, const char *function);
#define ssh_set_error_oom(error) _ssh_set_error_oom(error, __func__)

void _ssh_set_error_invalid(void *error, const char *function);
#define ssh_set_error_invalid(error) _ssh_set_error_invalid(error, __func__)

void ssh_reset_error(void *error);

#define __VA_NARG__(...) \
    (__VA_NARG_(__VA_ARGS__, __RSEQ_N()))
#define __VA_NARG_(...) \
    VA_APPLY_VARIADIC_MACRO(__VA_ARG_N, (__VA_ARGS__))
#define __VA_ARG_N(                                       \
        _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,          \
        _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
        _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
        _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
        _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
        _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
        _61, _62, _63, N, ...) N
#define __RSEQ_N()                                  \
    63, 62, 61, 60,                                 \
            59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
            49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
            39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
            29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
            19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
            9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/**
 * Get the argument count of variadic arguments
 */
/*
 * Since MSVC 2010 there is a bug in passing __VA_ARGS__ to subsequent
 * macros as a single token, which results in:
 *    warning C4003: not enough actual parameters for macro '_VA_ARG_N'
 *  and incorrect behavior. This fixes issue.
 */
#define VA_APPLY_VARIADIC_MACRO(macro, tuple) macro tuple

#ifndef HAVE_NTOHLL
#ifdef WORDS_BIGENDIAN
#define ntohll(x) (x)
#else
#ifndef ntohll
#define ntohll(x) (((uint64_t) ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif
#endif
#endif
/** Free memory space */
#define SAFE_FREE(x)          \
    do {                      \
        if ((x) != nullptr) { \
            free(x);          \
            x = nullptr;      \
        }                     \
    } while (0)
#define bignum_safe_free(num)     \
    do {                          \
        if ((num) != nullptr) {   \
            BN_clear_free((num)); \
            (num) = nullptr;      \
        }                         \
    } while (0)
#define SSH_BUFFER_PACK_END ((uint32_t) 0x4f65feb3)
typedef BIGNUM *bignum;

#ifndef HAVE_HTONLL
#ifdef WORDS_BIGENDIAN
#define htonll(x) (x)
#else
#ifndef htonll
#define htonll(x) \
    (((uint64_t) htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#endif
#endif
#endif
#define bignum_num_bytes(num) (size_t) BN_num_bytes(num)
#define bignum_num_bits(num) (size_t) BN_num_bits(num)
#define bignum_is_bit_set(num, bit) BN_is_bit_set(num, (int) bit)
#define bignum_bn2bin(num, len, ptr) BN_bn2bin(num, ptr)

/*
 * Describes a buffer state
 * [XXXXXXXXXXXXDATA PAYLOAD       XXXXXXXXXXXXXXXXXXXXXXXX]
 * ^            ^                  ^                       ^]
 * \_data points\_pos points here  \_used points here |    /
 *   here                                          Allocated
 */
struct ssh_buffer_struct {
    bool secure;
    uint32_t used;
    uint32_t allocated;
    uint32_t pos;
    uint8_t *data;
};
#define bignum_set_word(bn, n) BN_set_word(bn, n)
#define bignum_bin2bn(data, datalen, dest)     \
    do {                                       \
        (*dest) = BN_new();                    \
        if ((*dest) != nullptr) {              \
            BN_bin2bn(data, datalen, (*dest)); \
        }                                      \
    } while (0)
/* must be 32 bits number + immediately our data */
#ifdef _MSC_VER
#pragma pack(1)
#endif
struct ssh_string_struct {
    uint32_t size;
    unsigned char data[1];
};
typedef ssh_string_struct *ssh_string;
int _ssh_buffer_pack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...);
#define ssh_buffer_pack(buffer, format, ...) _ssh_buffer_pack((buffer), (format), __VA_NARG__(__VA_ARGS__), __VA_ARGS__, SSH_BUFFER_PACK_END)

int awsmock_ssh_buffer_unpack_va(ssh_buffer_struct *buffer, const char *format, size_t argc, va_list ap);
int _ssh_buffer_unpack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...);
#define ssh_buffer_unpack(buffer, format, ...) _ssh_buffer_unpack((buffer), (format), __VA_NARG__(__VA_ARGS__), __VA_ARGS__, SSH_BUFFER_PACK_END)
#define MAX_ENTRIES_NUM_IN_PACKET 50
#define MAX_LONG_NAME_LEN 350
}

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/sftpserver/SftpUser.h>
#include <boost/log/trivial.hpp>
#define DEF_STR_SIZE 1024
inline char authorizedkeys[DEF_STR_SIZE] = {0};

namespace AwsMock::Service {

    class SftpServer {

      public:

        /**
         * Constructor
         */
        SftpServer() = default;

        /**
         * Add a user to the database
         */
        static void AddUser(const std::string &userName, const std::string &password, const std::string &homeDirectory);

        /**
         * Main method
         *
         * @param port SFTP port
         * @param hostKey host key
         * @param address listen address
         * @param serverId server ID
         */
        void operator()(const std::string &port, const std::string &hostKey, const std::string &address, const std::string &serverId) const;

        /**
         * SFTP users
         */
        static SftpUsers _sftpUsers;

      private:

        /**
         * Server port
         */
        std::string _port = "2222";

        /**
         * Host key
         */
        std::string _hostKey = "/etc/ssh/ssh_host_ed25519_key";

        /**
         * Bind address
         */
        std::string _address = "0.0.0.0";
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_SERVER_SFTP_SERVER_H
