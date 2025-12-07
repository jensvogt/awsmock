//
// Created by vogje01 on 3/30/25.
//

#include "awsmock/repository/CognitoDatabase.h"

#include <awsmock/core/logging/LogStream.h>
#include <awsmock/sftpserver/SftpServer.h>
#include <awsmock/sftpserver/SftpUser.h>

#ifdef _WIN32
extern HANDLE g_ServiceStopEvent;
#endif

namespace AwsMock::Service {
    SftpUsers SftpServer::_sftpUsers;
}

//================================= sftpserver =======================================
extern "C" {
#ifdef _WIN32

#define ctime_r(t, b) ctime_s(b, sizeof(b), t)

int gettimeofday(timeval *tp, struct timezone *tzp) {
    namespace sc = std::chrono;
    const sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    const auto s = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec = s.count();
    tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();
    return 0;
}

size_t my_strnlen(const char *src, size_t n) {
    size_t len = 0;
    while (len < n && src[len]) len++;
    return len;
}

char *mystrndup(const char *s, size_t n) {
    const size_t len = my_strnlen(s, n);
    const auto p = static_cast<char *>(malloc(len + 1));
    if (p) {
        memcpy(p, s, len);
        p[len] = '\0';
    }
    return p;
}
#endif// _WIN32

// User home directory
char *userBasePath;
char *currentUser;
const char *currentServerId;

enum sftp_handle_type {
    SFTP_nullptr_HANDLE,
    SFTP_DIR_HANDLE,
    SFTP_FILE_HANDLE
};

struct sftp_handle {
    sftp_handle_type type;
    int fd;
    DIR *dirp;
    char *name;
};

static char *FtpFileNameToRealPath(const char *filename) {
    const auto realFilename = static_cast<char *>(malloc(strlen(userBasePath) + strlen(filename) + 1));
    strcpy(realFilename, userBasePath);
    if (filename[0] != '\0' && filename[0] != '.') {
        if (strncmp(filename, userBasePath, strlen(userBasePath)) != 0) {
            strcpy(realFilename + strlen(realFilename), filename);
        } else {
            strcpy(realFilename, filename);
        }
    }
    realFilename[strlen(realFilename)] = '\0';
    return realFilename;
}

extern void sftp_set_error(const sftp_session sftp, int errnum);
/*void sftp_set_error(const sftp_session sftp, int errnum) {
    if (sftp != nullptr) {
        sftp->errnum = errnum;
    }
}*/

static const char *ssh_str_error(int u_errno) {
    switch (u_errno) {
        case SSH_FX_NO_SUCH_FILE:
            return "No such file";
        case SSH_FX_PERMISSION_DENIED:
            return "Permission denied";
        case SSH_FX_BAD_MESSAGE:
            return "Bad message";
        case SSH_FX_OP_UNSUPPORTED:
            return "Operation not supported";
        default:
            return "Operation failed";
    }
}

static void stat_to_filexfer_attrib(const struct stat *z_st, sftp_attributes_struct *z_attr) {
    z_attr->flags = 0 | static_cast<uint32_t>(SSH_FILEXFER_ATTR_SIZE);
    z_attr->size = z_st->st_size;

    z_attr->flags |= static_cast<uint32_t>(SSH_FILEXFER_ATTR_UIDGID);
    z_attr->uid = z_st->st_uid;
    z_attr->gid = z_st->st_gid;

    z_attr->flags |= static_cast<uint32_t>(SSH_FILEXFER_ATTR_PERMISSIONS);
    z_attr->permissions = z_st->st_mode;

    z_attr->flags |= static_cast<uint32_t>(SSH_FILEXFER_ATTR_ACMODTIME);
    z_attr->atime = z_st->st_atime;
    z_attr->mtime = z_st->st_mtime;
}

static void clear_filexfer_attrib(struct sftp_attributes_struct *z_attr) {
    z_attr->flags = 0;
    z_attr->size = 0;
    z_attr->uid = 0;
    z_attr->gid = 0;
    z_attr->permissions = 0;
    z_attr->atime = 0;
    z_attr->mtime = 0;
}

/* members that are common to ssh_session and ssh_bind */
struct ssh_common_struct {
    error_struct error;
    ssh_callbacks callbacks; /* Callbacks to user functions */
    int log_verbosity;       /* verbosity of the log functions */
};

static int unix_errno_to_ssh_stat(int u_errno) {
    int ret = SSH_OK;
    switch (u_errno) {
        case 0:
            break;
        case ENOENT:
        case ENOTDIR:
        case EBADF:
        case ELOOP:
            ret = SSH_FX_NO_SUCH_FILE;
            break;
        case EPERM:
        case EACCES:
        case EFAULT:
            ret = SSH_FX_PERMISSION_DENIED;
            break;
        case ENAMETOOLONG:
        case EINVAL:
            ret = SSH_FX_BAD_MESSAGE;
            break;
        case ENOSYS:
            ret = SSH_FX_OP_UNSUPPORTED;
            break;
        default:
            ret = SSH_FX_FAILURE;
            break;
    }

    return ret;
}

/**
 * @brief Return s3 key
 *
 * @param name filename
 * @param prefix basedir prefix
 * @return S3 key
 */
static const char *get_s3_key(const char *name, const char *prefix) {
    const auto s3Key = static_cast<char *>(malloc(PATH_MAX));
    strcpy(s3Key, name + strlen(prefix) + 1);
    return s3Key;
}

/**
 * @internal
 *
 * @brief Registers an out of memory error
 *
 * @param  error       The place to store the error.
 *
 */
extern void _ssh_set_error_oom(void *error, const char *function);
/*void _ssh_set_error_oom(void *error, const char *function) {
    auto *err = static_cast<error_struct *>(error);

    snprintf(err->error_buffer, sizeof(err->error_buffer), "%s: Out of memory", function);
    err->error_code = SSH_FATAL;
}*/

/**
 * @internal
 *
 * @brief Add a 16 bits unsigned integer to the tail of a buffer.
 *
 * @param[in]  buffer   The buffer to add the integer.
 * @param[in]  data     The 16 bits integer to add.
 * @return              0 on success, -1 on error.
 */
int awsmock_ssh_buffer_add_u16(ssh_buffer_struct *buffer, uint16_t data) {
    if (const int rc = ssh_buffer_add_data(buffer, &data, sizeof(data)); rc < 0) {
        return -1;
    }

    return 0;
}

/**
 * @internal
 *
 * @brief Add a 32 bits unsigned integer to the tail of a buffer.
 *
 * @param[in]  buffer   The buffer to add the integer.
 * @param[in]  data     The 32 bits integer to add.
 * @return              0 on success, -1 on error.
 */
int awsmock_ssh_buffer_add_u32(ssh_buffer_struct *buffer, uint32_t data) {
    if (const int rc = ssh_buffer_add_data(buffer, &data, sizeof(data)); rc < 0) {
        return -1;
    }

    return 0;
}

/**
 * @internal
 *
 * @brief Add a SSH string to the tail of a buffer.
 *
 * @param[in]  buffer   The buffer to add the string.
 * @param[in]  string   The SSH String to add.
 * @return              0 on success, < 0 on error.
 */
int awsmock_ssh_buffer_add_ssh_string(ssh_buffer_struct *buffer, ssh_string_struct *string) {
    uint32_t len = 0;

    if (string == nullptr) {
        return -1;
    }

    len = ssh_string_len(string);
    if (ssh_buffer_add_data(buffer, string, len + sizeof(uint32_t)) < 0) {
        return -1;
    }

    return 0;
}

/**
 * @defgroup libssh_log The SSH logging functions
 * @ingroup libssh
 *
 * Logging functions for debugging and problem resolving.
 */
static int current_timestring(int hires, char *buf, size_t len) {
    char tbuf[64];
    timeval tv{};
    time_t t;

    gettimeofday(&tv, nullptr);
    t = tv.tv_sec;

    const tm *tm = localtime(&t);
    if (tm == nullptr) {
        return -1;
    }

    if (hires) {
        strftime(tbuf, sizeof(tbuf) - 1, "%Y/%m/%d %H:%M:%S", tm);
        snprintf(buf, len, "%s.%06ld", tbuf, (long) tv.tv_usec);
    } else {
        strftime(tbuf, sizeof(tbuf) - 1, "%Y/%m/%d %H:%M:%S", tm);
        snprintf(buf, len, "%s", tbuf);
    }

    return 0;
}

static void ssh_log_stderr(int verbosity, const char *function, const char *buffer) {
    char date[128] = {};

    if (const int rc = current_timestring(1, date, sizeof(date)); rc == 0) {
        fprintf(stderr, "[%s, %d] %s:", date, verbosity, function);
    } else {
        fprintf(stderr, "[%d] %s", verbosity, function);
    }

    fprintf(stderr, "  %s\n", buffer);
}

static void ssh_log_custom(ssh_logging_callback log_fn, int verbosity, const char *function, const char *buffer) {
    char buf[LOG_SIZE + 64];

    snprintf(buf, sizeof(buf), "%s: %s", function, buffer);
    log_fn(verbosity, function, buf, ssh_get_log_userdata());
}

extern void ssh_log_function(const int verbosity, const char *function, const char *buffer);
/*void ssh_log_function(const int verbosity, const char *function, const char *buffer) {
    if (const ssh_logging_callback log_fn = ssh_get_log_callback()) {
        ssh_log_custom(log_fn, verbosity, function, buffer);
        return;
    }

    ssh_log_stderr(verbosity, function, buffer);
}*/

/**
 * @internal
 *
 * @brief Registers an error with a description.
 *
 * @param  error       The place to store the error.
 * @param  code        The class of error.
 * @param  descr       The description, which can be a format string.
 * @param  ...         The arguments for the format string.
 */
extern void _ssh_set_error(void *error, int code, const char *function, const char *descr, ...);
/*void _ssh_set_error(void *error, int code, const char *function, const char *descr, ...) {
    auto *err = static_cast<struct ssh_common_struct *>(error);
    va_list va;

    va_start(va, descr);
    vsnprintf(err->error.error_buffer, ERROR_BUFFERLEN, descr, va);
    va_end(va);

    err->error.error_code = code;
    if (ssh_get_log_level() == SSH_LOG_TRACE) {
        ssh_log_function(SSH_LOG_TRACE, function, err->error.error_buffer);
    }
}*/

/**
 * @internal
 *
 * @brief Add a 64 bits unsigned integer to the tail of a buffer.
 *
 * @param[in]  buffer   The buffer to add the integer.
 * @param[in]  data     The 64 bits integer to add.
 * @return              0 on success, -1 on error.
 */
int awsmock_ssh_buffer_add_u64(struct ssh_buffer_struct *buffer, uint64_t data) {
    if (const int rc = ssh_buffer_add_data(buffer, &data, sizeof(data)); rc < 0) {
        return -1;
    }

    return 0;
}

/**
 * @internal
 *
 * @brief Add a 8 bits unsigned integer to the tail of a buffer.
 *
 * @param[in]  buffer   The buffer to add the integer.
 * @param[in]  data     The 8 bits integer to add.
 * @return              0 on success, -1 on error.
 */
int awsmock_ssh_buffer_add_u8(struct ssh_buffer_struct *buffer, uint8_t data) {
    if (const int rc = ssh_buffer_add_data(buffer, &data, sizeof(uint8_t)); rc < 0) {
        return -1;
    }

    return 0;
}

static int realloc_buffer(ssh_buffer_struct *buffer, uint32_t needed) {
    uint32_t smallest = 1;
    uint8_t *newBuffer = nullptr;

    buffer_verify(buffer);

    // Find the smallest power of two which is greater or equal to needed
    while (smallest <= needed) {
        if (smallest == 0) {
            return -1;
        }
        smallest <<= 1;
    }
    needed = smallest;

    if (needed > BUFFER_SIZE_MAX) {
        return -1;
    }

    if (buffer->secure) {
        newBuffer = static_cast<uint8_t *>(malloc(needed));
        if (newBuffer == nullptr) {
            return -1;
        }
        memcpy(newBuffer, buffer->data, buffer->used);
        memset(buffer->data, 0, buffer->used);
        SAFE_FREE(buffer->data);
    } else {
        newBuffer = static_cast<uint8_t *>(realloc(buffer->data, needed));
        if (newBuffer == nullptr) {
            return -1;
        }
    }
    buffer->data = newBuffer;
    buffer->allocated = needed;

    buffer_verify(buffer);
    return 0;
}

/**
 * @brief shifts a buffer to remove unused data in the beginning
 *
 * @param buffer SSH buffer
 */
static void buffer_shift(ssh_buffer buffer) {
    size_t burn_pos = buffer->pos;

    buffer_verify(buffer);

    if (buffer->pos == 0) {
        return;
    }
    memmove(buffer->data, buffer->data + buffer->pos, buffer->used - buffer->pos);
    buffer->used -= buffer->pos;
    buffer->pos = 0;

    if (buffer->secure) {
        void *ptr = buffer->data + buffer->used;
        memset(ptr, 0, burn_pos);
    }

    buffer_verify(buffer);
}

/**
 * @brief Ensure the buffer has at least a certain preallocated size.
 *
 * @param[in]  buffer   The buffer to enlarge.
 *
 * @param[in]  len      The length to ensure as allocated.
 *
 * @return              0 on success, < 0 on error.
 */
int awsmock_ssh_buffer_allocate_size(ssh_buffer_struct *buffer, uint32_t len) {
    buffer_verify(buffer);

    if (buffer->allocated < len) {
        if (buffer->pos > 0) {
            buffer_shift(buffer);
        }
        if (realloc_buffer(buffer, len) < 0) {
            return -1;
        }
    }

    buffer_verify(buffer);

    return 0;
}

/**
 * @brief Pre-calculate the size we need for packing the buffer.
 *
 * This makes sure that enough memory is allocated for packing the buffer and
 * we only have to do one memory allocation.
 *
 * @param[in]  buffer    The buffer to allocate
 * @param[in]  format    A format string of arguments.
 * @param[in]  argc      The number of arguments.
 * @param[in]  ap        The va_list of arguments.
 * @return SSH_OK on success, SSH_ERROR on error.
 */
static int ssh_buffer_pack_allocate_va(ssh_buffer_struct *buffer, const char *format, size_t argc, va_list ap) {
    const char *p = nullptr;
    ssh_string string = nullptr;
    char *cstring = nullptr;
    size_t needed_size = 0;
    size_t len;
    size_t count;
    int rc = SSH_OK;

    for (p = format, count = 0; *p != '\0'; p++, count++) {
        // Invalid number of arguments passed
        if (count > argc) {
            return SSH_ERROR;
        }

        switch (*p) {
            case 'b':
                va_arg(ap, unsigned int);
                needed_size += sizeof(uint8_t);
                break;
            case 'w':
                va_arg(ap, unsigned int);
                needed_size += sizeof(uint16_t);
                break;
            case 'd':
                va_arg(ap, uint32_t);
                needed_size += sizeof(uint32_t);
                break;
            case 'q':
                va_arg(ap, uint64_t);
                needed_size += sizeof(uint64_t);
                break;
            case 'S':
                string = va_arg(ap, ssh_string);
                needed_size += 4 + ssh_string_len(string);
                string = nullptr;
                break;
            case 's':
                cstring = va_arg(ap, char *);
                needed_size += sizeof(uint32_t) + strlen(cstring);
                cstring = nullptr;
                break;
            case 'P':
                len = va_arg(ap, size_t);
                needed_size += len;
                va_arg(ap, void *);
                count++; /* increase argument count */
                break;
            case 'B':
                va_arg(ap, bignum);
                /*
                 * Use a fixed size for a bignum
                 * (they should normally be around 32)
                 */
                needed_size += 64;
                break;
            case 't':
                cstring = va_arg(ap, char *);
                needed_size += strlen(cstring);
                cstring = nullptr;
                break;
            default:
                log_error << "Invalid buffer format: " << *p;
                rc = SSH_ERROR;
        }
        if (rc != SSH_OK) {
            break;
        }
    }

    if (argc != count) {
        return SSH_ERROR;
    }

    if (rc != SSH_ERROR) {
        // Check if our canary is intact, if not, something really bad happened.
        if (const uint32_t canary = va_arg(ap, uint32_t); canary != SSH_BUFFER_PACK_END) {
            abort();
        }
    }

    rc = awsmock_ssh_buffer_allocate_size(buffer, static_cast<uint32_t>(needed_size));
    if (rc != 0) {
        return SSH_ERROR;
    }

    return SSH_OK;
}


ssh_string ssh_make_bignum_string(bignum num) {
    ssh_string ptr = nullptr;
    size_t pad = 0;
    size_t len = bignum_num_bytes(num);
    size_t bits = bignum_num_bits(num);

    if (len == 0) {
        return nullptr;
    }

    /* If the first bit is set we have a negative number */
    if (!(bits % 8) && bignum_is_bit_set(num, bits - 1)) {
        pad++;
    }

#ifdef DEBUG_CRYPTO
    SSH_LOG(SSH_LOG_TRACE,
            "%zu bits, %zu bytes, %zu padding",
            bits, len, pad);
#endif /* DEBUG_CRYPTO */

    ptr = ssh_string_new(len + pad);
    if (ptr == nullptr) {
        return nullptr;
    }

    // We have a negative number so we need a leading zero
    if (pad) {
        ptr->data[0] = 0;
    }

    bignum_bn2bin(num, len, ptr->data + pad);

    return ptr;
}

/**
 * @brief Add multiple values in a buffer on a single function call
 *
 * @param[in] buffer    The buffer to add to
 * @param[in] format    A format string of arguments.
 * @param[in] ap        A va_list of arguments.
 * @returns             SSH_OK on success, SSH_ERROR on error
 * @see ssh_buffer_add_format() for format list values.
 */
static int ssh_buffer_pack_va(ssh_buffer_struct *buffer, const char *format, size_t argc, va_list ap) {
    int rc = SSH_ERROR;
    const char *p;
    union {
        uint8_t byte;
        uint16_t word;
        uint32_t dword;
        uint64_t qword;
        ssh_string string;
        void *data;
    } o;
    char *cstring;
    bignum b;
    size_t len;
    size_t count;

    if (argc > 256) {
        return SSH_ERROR;
    }

    for (p = format, count = 0; *p != '\0'; p++, count++) {
        // Invalid number of arguments passed
        if (count > argc) {
            return SSH_ERROR;
        }

        switch (*p) {
            case 'b':
                o.byte = static_cast<uint8_t>(va_arg(ap, unsigned int));
                rc = awsmock_ssh_buffer_add_u8(buffer, o.byte);
                break;
            case 'w':
                o.word = static_cast<uint16_t>(va_arg(ap, unsigned int));
                o.word = htons(o.word);
                rc = awsmock_ssh_buffer_add_u16(buffer, o.word);
                break;
            case 'd':
                o.dword = va_arg(ap, uint32_t);
                o.dword = htonl(o.dword);
                rc = awsmock_ssh_buffer_add_u32(buffer, o.dword);
                break;
            case 'q':
                o.qword = va_arg(ap, uint64_t);
                o.qword = htonll(o.qword);
                rc = awsmock_ssh_buffer_add_u64(buffer, o.qword);
                break;
            case 'S':
                o.string = va_arg(ap, ssh_string);
                rc = awsmock_ssh_buffer_add_ssh_string(buffer, o.string);
                o.string = nullptr;
                break;
            case 's':
                cstring = va_arg(ap, char *);
                len = strlen(cstring);
                rc = awsmock_ssh_buffer_add_u32(buffer, htonl(len));
                if (rc == SSH_OK) {
                    rc = ssh_buffer_add_data(buffer, cstring, len);
                }
                cstring = nullptr;
                break;
            case 'P':
                len = va_arg(ap, size_t);

                o.data = va_arg(ap, void *);
                count++; /* increase argument count */

                rc = ssh_buffer_add_data(buffer, o.data, len);
                o.data = nullptr;
                break;
            case 'B':
                b = va_arg(ap, bignum);
                o.string = ssh_make_bignum_string(b);
                if (o.string == nullptr) {
                    rc = SSH_ERROR;
                    break;
                }
                rc = awsmock_ssh_buffer_add_ssh_string(buffer, o.string);
                SAFE_FREE(o.string);
                break;
            case 't':
                cstring = va_arg(ap, char *);
                len = strlen(cstring);
                rc = ssh_buffer_add_data(buffer, cstring, len);
                cstring = nullptr;
                break;
            default:
                log_error << "Invalid buffer format: " << *p;
                rc = SSH_ERROR;
        }
        if (rc != SSH_OK) {
            break;
        }
    }

    if (argc != count) {
        return SSH_ERROR;
    }

    if (rc != SSH_ERROR) {
        // Check if our canary is intact, if not something really bad happened
        if (uint32_t canary = va_arg(ap, uint32_t); canary != SSH_BUFFER_PACK_END) {
            abort();
        }
    }
    return rc;
}

/**
 * @brief Add multiple values in a buffer on a single function call
 *
 * @param[in] buffer    The buffer to add to
 * @param[in] format    A format string of arguments. This string contains single
 *                      letters describing the order and type of arguments:
 *                         'b': uint8_t  (pushed in network byte order)
 *                         'w': uint16_t (pushed in network byte order)
 *                         'd': uint32_t (pushed in network byte order)
 *                         'q': uint64_t (pushed in network byte order)
 *                         'S': ssh_string
 *                         's': char * (C string, pushed as SSH string)
 *                         't': char * (C string, pushed as free text)
 *                         'P': size_t, void * (len of data, pointer to data)
 *                              only pushes data.
 *                         'B': bignum (pushed as SSH string)
 * @returns             SSH_OK on success, SSH_ERROR on error
 * @warning             when using 'P' with a constant size (e.g. 8), do not forget to cast to (size_t).
 */
extern int _ssh_buffer_pack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...);
/*int _ssh_buffer_pack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...) {
    va_list ap;

    if (argc > 256) {
        return SSH_ERROR;
    }

    va_start(ap, argc);
    int rc = ssh_buffer_pack_allocate_va(buffer, format, argc, ap);
    va_end(ap);

    if (rc != SSH_OK) {
        return rc;
    }

    va_start(ap, argc);
    rc = ssh_buffer_pack_va(buffer, format, argc, ap);
    va_end(ap);

    return rc;
}*/

/**
 * @brief Add data at the head of a buffer.
 *
 * @param[in]  buffer   The buffer to add the data.
 * @param[in]  data     The data to prepend.
 * @param[in]  len      The length of data to prepend.
 * @return              0 on success, -1 on error.
 */
int awsmock_ssh_buffer_prepend_data(ssh_buffer_struct *buffer, const void *data, uint32_t len) {
    buffer_verify(buffer);

    if (len <= buffer->pos) {
        // It's possible to insert data between begin and pos
        memcpy(buffer->data + (buffer->pos - len), data, len);
        buffer->pos -= len;
        buffer_verify(buffer);
        return 0;
    }
    // pos isn't high enough
    if (buffer->used - buffer->pos + len < len) {
        return -1;
    }

    if (buffer->allocated < (buffer->used - buffer->pos + len)) {
        if (realloc_buffer(buffer, buffer->used - buffer->pos + len) < 0) {
            return -1;
        }
    }
    memmove(buffer->data + len, buffer->data + buffer->pos, buffer->used - buffer->pos);
    memcpy(buffer->data, data, len);
    buffer->used += len - buffer->pos;
    buffer->pos = 0;
    buffer_verify(buffer);
    return 0;
}

int awsmock_sftp_packet_write(sftp_session sftp, uint8_t type, ssh_buffer payload) {
    constexpr uint8_t header[5] = {};

    /* Add size of type */
    const uint32_t payload_size = ssh_buffer_get_len(payload) + sizeof(uint8_t);
    PUSH_BE_U32(header, 0, payload_size);
    PUSH_BE_U8(header, 4, type);

    if (const int rc = awsmock_ssh_buffer_prepend_data(payload, header, sizeof(header)); rc < 0) {
        ssh_set_error_oom(sftp->session);
        sftp_set_error(sftp, SSH_FX_FAILURE);
        return -1;
    }

    const int size = ssh_channel_write(sftp->channel, ssh_buffer_get(payload), ssh_buffer_get_len(payload));
    if (size < 0) {
        sftp_set_error(sftp, SSH_FX_FAILURE);
        return -1;
    }

    if (static_cast<uint32_t>(size) != ssh_buffer_get_len(payload)) {
        log_error << "Had to write " << ssh_buffer_get_len(payload) << " bytes, wrote only " << size;
    }

    return size;
}

/**
 * @brief Handle the statvfs request, return information the mounted file system.
 *
 * @param  msg          The sftp client message.
 * @param  st           The statvfs state of target file.
 * @return              0 on success, < 0 on error with ssh and sftp error set.
 * @see sftp_get_error()
 */
static int sftp_reply_statvfs(sftp_client_message msg, sftp_statvfs_t st) {
    int ret = 0;
    ssh_buffer out = ssh_buffer_new();
    if (out == nullptr) {
        return -1;
    }

    log_debug << "Sending statvfs reply";

    if (awsmock_ssh_buffer_add_u32(out, msg->id) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_bsize)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_frsize)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_blocks)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_bfree)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_bavail)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_files)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_ffree)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_favail)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_fsid)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_flag)) < 0 ||
        awsmock_ssh_buffer_add_u64(out, ntohll(st->f_namemax)) < 0 ||
        awsmock_sftp_packet_write(msg->sftp, SSH_FXP_EXTENDED_REPLY, out) < 0) {
        ret = -1;
    }
    SSH_BUFFER_FREE(out);

    return ret;
}

static int process_open(sftp_client_message client_msg) {
    const char *filename = sftp_client_message_get_filename(client_msg);

    const std::string filenameStr = std::string(filename);
    const uint32_t msg_flag = sftp_client_message_get_flags(client_msg);
    const uint32_t mode = client_msg->attr->permissions;
    ssh_string handle_s = nullptr;
    struct sftp_handle *h = nullptr;
    int file_flag;
    int fd = -1;

    log_debug << "Processing open: filename " << filename << ", mode: " << mode;

    if ((msg_flag & static_cast<uint32_t>(SSH_FXF_READ)) == SSH_FXF_READ &&
        (msg_flag & static_cast<uint32_t>(SSH_FXF_WRITE)) == SSH_FXF_WRITE) {
        file_flag = O_RDWR;// file must exist
        if ((msg_flag & static_cast<uint32_t>(SSH_FXF_CREAT)) == SSH_FXF_CREAT) file_flag |= O_CREAT;
    } else if ((msg_flag & static_cast<uint32_t>(SSH_FXF_WRITE)) == SSH_FXF_WRITE) {
        file_flag = O_WRONLY;
        if ((msg_flag & static_cast<uint32_t>(SSH_FXF_APPEND)) == SSH_FXF_APPEND) file_flag |= O_APPEND;
        if ((msg_flag & static_cast<uint32_t>(SSH_FXF_CREAT)) == SSH_FXF_CREAT) file_flag |= O_CREAT;
    } else if ((msg_flag & static_cast<uint32_t>(SSH_FXF_READ)) == SSH_FXF_READ) {
        file_flag = O_RDONLY;
    } else {
        log_warning << "Undefined message flag: " << msg_flag;
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "Flag error");
        return SSH_ERROR;
    }

    fd = open(filename, file_flag, mode);
    if (fd == -1) {
        const int saved_errno = errno;
        log_error << "Error open file with error: " << strerror(saved_errno);
        const int status = unix_errno_to_ssh_stat(saved_errno);
        sftp_reply_status(client_msg, status, "Write error");
        return SSH_ERROR;
    }

    h = static_cast<struct sftp_handle *>(calloc(1, sizeof(struct sftp_handle)));
    if (h == nullptr) {
        close(fd);
        log_error << "Failed to allocate a new handle";
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "Failed to allocate new handle");
        return SSH_ERROR;
    }
    h->fd = fd;
    h->type = SFTP_FILE_HANDLE;
    handle_s = sftp_handle_alloc(client_msg->sftp, h);
    if (handle_s != nullptr) {
        sftp_reply_handle(client_msg, handle_s);
        ssh_string_free(handle_s);
    } else {
        close(fd);
        log_error << "Failed to allocate handle";
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "Failed to allocate handle");
    }

    return SSH_OK;
}

/**
 * @brief Log the content of a buffer in hexadecimal format, similar to the
 * output of 'hexdump -C' command.
 *
 * The first logged line is the given description followed by the length.
 * Then the content of the buffer is logged 16 bytes per line in the following
 * format:
 *
 * (offset) (first 8 bytes) (last 8 bytes) (the 16 bytes as ASCII char values)
 *
 * The output for a 16 bytes array containing values from 0x00 to 0x0f would be:
 *
 * "Example (16 bytes):"
 * "  00000000  00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  ................"
 *
 * The value for each byte as corresponding ASCII character is printed at the
 * end if the value is printable. Otherwise, it is replaced with '.'.
 *
 * @param[in] descr A description for the content to be logged
 * @param[in] what  The buffer to be logged
 * @param[in] len   The length of the buffer given in what
 *
 * @note If a too long description is provided (which would result in a first
 * line longer than 80 bytes), the function will fail.
 */
void awsmock_ssh_log_hexdump(const char *descr, const unsigned char *what, size_t len) {
    size_t i;
    char ascii[17];
    const unsigned char *pc = nullptr;
    size_t count = 0;
    ssize_t printed = 0;

    /* The required buffer size is calculated from:
     *
     *  2 bytes for spaces at the beginning
     *  8 bytes for the offset
     *  2 bytes for spaces
     * 24 bytes to print the first 8 bytes + spaces
     *  1 byte for an extra space
     * 24 bytes to print next 8 bytes + spaces
     *  2 bytes for extra spaces
     * 16 bytes for the content as ASCII characters at the end
     *  1 byte for the ending '\0'
     *
     * Resulting in 80 bytes.
     *
     * Except for the first line (description + size), all lines have fixed
     * length. If a too long description is used, the function will fail.
     * */
    char buffer[80];

    /* Print description */
    if (descr != nullptr) {
        printed = snprintf(buffer, sizeof(buffer), "%s ", descr);
        if (printed < 0) {
            goto error;
        }
        count += printed;
    } else {
        printed = snprintf(buffer, sizeof(buffer), "(nullptr description) ");
        if (printed < 0) {
            goto error;
        }
        count += printed;
    }

    if (len == 0) {
        printed = snprintf(buffer + count, sizeof(buffer) - count, "(zero length):");
        if (printed < 0) {
            goto error;
        }
        log_trace << buffer;
        return;
    }

    printed = snprintf(buffer + count, sizeof(buffer) - count, "(%zu bytes):", len);
    if (printed < 0) {
        goto error;
    }
    count += printed;

    if (what == nullptr) {
        printed = snprintf(buffer + count, sizeof(buffer) - count, "(nullptr)");
        if (printed < 0) {
            goto error;
        }
        log_trace << buffer;
        return;
    }

    log_trace << buffer;

    // Reset state
    count = 0;
    pc = what;

    for (i = 0; i < len; i++) {
        // Add one space after printing 8 bytes
        if ((i % 8) == 0) {
            if (i != 0) {
                printed = snprintf(buffer + count, sizeof(buffer) - count, " ");
                if (printed < 0) {
                    goto error;
                }
                count += printed;
            }
        }

        // Log previous line and reset state for new line
        if (i % 16 == 0) {
            if (i != 0) {
                printed = snprintf(buffer + count, sizeof(buffer) - count,
                                   "  %s", ascii);
                if (printed < 0) {
                    goto error;
                }
                log_trace << buffer;
                count = 0;
            }

            /* Start a new line with the offset */
            printed = snprintf(buffer, sizeof(buffer), "  %08zx ", i);
            if (printed < 0) {
                goto error;
            }
            count += printed;
        }

        // Print the current byte hexadecimal representation
        printed = snprintf(buffer + count, sizeof(buffer) - count, " %02x", pc[i]);
        if (printed < 0) {
            goto error;
        }
        count += printed;

        // If printable, store the ASCII character
        if (isprint(pc[i])) {
            ascii[i % 16] = pc[i];
        } else {
            ascii[i % 16] = '.';
        }
        ascii[i % 16 + 1] = '\0';
    }

    // Add padding if not exactly 16 characters
    while (i % 16 != 0) {
        // Add one space after printing 8 bytes
        if (i % 8 == 0) {
            if (i != 0) {
                printed = snprintf(buffer + count, sizeof(buffer) - count, " ");
                if (printed < 0) {
                    goto error;
                }
                count += printed;
            }
        }

        printed = snprintf(buffer + count, sizeof(buffer) - count, "   ");
        if (printed < 0) {
            goto error;
        }
        count += printed;
        i++;
    }

    // Print the last printable part
    printed = snprintf(buffer + count, sizeof(buffer) - count, "   %s", ascii);
    if (printed < 0) {
        goto error;
    }

    log_trace << buffer;

    return;

error:
    log_error << "Could not print to buffer";
}

/**
 * @brief Read the requested number of bytes from a local file.
 *
 * A call to read() may perform a short read even when sufficient data is
 * present in the file. This function can be used to avoid such short reads.
 *
 * This function tries to read the requested number of bytes from the file
 * until one of the following occurs :
 *     - Requested number of bytes are read.
 *     - EOF is encountered before reading the requested number of bytes.
 *     - An error occurs.
 *
 * On encountering an error due to an interrupt, this function ignores that
 * error and continues trying to read the data.
 *
 * @param[in] fd          The file descriptor of the local file to read from.
 * @param[out] buf        Pointer to a buffer in which read data will be stored.
 * @param[in] nbytes      Number of bytes to read.
 * @returns               Number of bytes read on success, SSH_ERROR on error with errno set to indicate the error.
 */
extern ssize_t ssh_readn(int fd, void *buf, size_t nbytes);
/*ssize_t ssh_readn(int fd, void *buf, size_t nbytes) {
    size_t total_bytes_read = 0;

    if (fd < 0 || buf == nullptr || nbytes == 0) {
        errno = EINVAL;
        return SSH_ERROR;
    }

    do {
        const ssize_t bytes_read = read(fd, static_cast<char *>(buf) + total_bytes_read, nbytes - total_bytes_read);
        if (bytes_read == -1) {
            if (errno == EINTR) {
                // Ignoring errors due to signal interrupts
                continue;
            }

            return SSH_ERROR;
        }

        if (bytes_read == 0) {
            // EOF encountered on the local file before reading nbytes
            break;
        }

        total_bytes_read += static_cast<size_t>(bytes_read);
    } while (total_bytes_read < nbytes);

    return total_bytes_read;
}*/

static int process_read(sftp_client_message client_msg) {
    const sftp_session sftp = client_msg->sftp;
    const ssh_string handle = client_msg->handle;
    const struct sftp_handle *h = nullptr;
    ssize_t readn = 0;
    int fd = -1;
    char *buffer = nullptr;

    awsmock_ssh_log_hexdump("Processing read: handle:", reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle));

    h = static_cast<struct sftp_handle *>(sftp_handle(sftp, handle));
    if (h->type == SFTP_FILE_HANDLE) {
        fd = h->fd;
    }

    if (fd < 0) {
        sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
        log_error << "Invalid fd " << fd << " received from handle";
        return SSH_ERROR;
    }
    if (const off_t off = lseek(fd, client_msg->offset, SEEK_SET); off == -1) {
        sftp_reply_status(client_msg, SSH_FX_FAILURE, nullptr);
        log_error << "Error seeking file fd: " << fd << " at offset: " << client_msg->offset;
        return SSH_ERROR;
    }

    buffer = static_cast<char *>(malloc(client_msg->len));
    if (buffer == nullptr) {
        ssh_set_error_oom(sftp->session);
        sftp_reply_status(client_msg, SSH_FX_FAILURE, nullptr);
        log_error << "Failed to allocate memory for read data";
        return SSH_ERROR;
    }
    readn = ssh_readn(fd, buffer, client_msg->len);
    if (readn < 0) {
        sftp_reply_status(client_msg, SSH_FX_FAILURE, nullptr);
        log_error << "Read file error!";
        free(buffer);
        return SSH_ERROR;
    }
    if (readn > 0) {
        sftp_reply_data(client_msg, buffer, readn);
    } else {
        sftp_reply_status(client_msg, SSH_FX_EOF, nullptr);
    }

    free(buffer);
    return SSH_OK;
}

/**
 * @brief Write the requested number of bytes to a local file.
 *
 * A call to write() may perform a short write on a local file. This function
 * can be used to avoid short writes.
 *
 * This function tries to write the requested number of bytes until those many
 * bytes are written or some error occurs.
 *
 * On encountering an error due to an interrupt, this function ignores that
 * error and continues trying to write the data.
 *
 * @param[in] fd          The file descriptor of the local file to write to.
 * @param[in] buf         Pointer to a buffer in which data to write is stored.
 * @param[in] nbytes      Number of bytes to write.
 * @returns               Number of bytes written on success,SSH_ERROR on error with errno set to indicate theerror.
 */
extern ssize_t ssh_writen(int fd, const void *buf, size_t nbytes);
/*ssize_t ssh_writen(int fd, const void *buf, size_t nbytes) {
    size_t total_bytes_written = 0;

    if (fd < 0 || buf == nullptr || nbytes == 0) {
        errno = EINVAL;
        return SSH_ERROR;
    }

    do {
        const auto bytes_written = write(fd, static_cast<const char *>(buf) + total_bytes_written, nbytes - total_bytes_written);
        if (bytes_written == -1) {
            if (errno == EINTR) {
                // Ignoring errors due to signal interrupts
                continue;
            }

            return SSH_ERROR;
        }

        total_bytes_written += static_cast<size_t>(bytes_written);
    } while (total_bytes_written < nbytes);

    return static_cast<int>(total_bytes_written);
}*/

static int process_write(sftp_client_message client_msg) {
    const sftp_session sftp = client_msg->sftp;
    const ssh_string handle = client_msg->handle;
    const struct sftp_handle *h = nullptr;
    ssize_t written = 0;
    int fd = -1;
    const char *msg_data = nullptr;

    awsmock_ssh_log_hexdump("Processing write: handle", reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle));

    h = static_cast<struct sftp_handle *>(sftp_handle(sftp, handle));
    if (h->type == SFTP_FILE_HANDLE) {
        fd = h->fd;
    }
    if (fd < 0) {
        sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
        log_error << "Write file fd error!";
        return SSH_ERROR;
    }

    msg_data = ssh_string_get_char(client_msg->data);
    const uint32_t len = ssh_string_len(client_msg->data);

    if (const off_t off = lseek(fd, client_msg->offset, SEEK_SET); off == -1) {
        sftp_reply_status(client_msg, SSH_FX_FAILURE, nullptr);
        log_error << "Error seeking file at offset: " << client_msg->offset;
        return SSH_ERROR;
    }
    written = ssh_writen(fd, msg_data, len);
    if (written != static_cast<ssize_t>(len)) {
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "Write error");
        log_error << "File write error!";
        return SSH_ERROR;
    }

    sftp_reply_status(client_msg, SSH_FX_OK, nullptr);

    return SSH_OK;
}

static int process_close(sftp_client_message client_msg) {
    const sftp_session sftp = client_msg->sftp;
    const ssh_string handle = client_msg->handle;
    struct sftp_handle *h = nullptr;
    int ret;

    log_debug << "Processing close, handle: " << reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle);
    awsmock_ssh_log_hexdump("Processing close: handle:", reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle));

    h = static_cast<struct sftp_handle *>(sftp_handle(sftp, handle));

    // Send to S3
    if (h->fd > 0) {
        char filePath[PATH_MAX];
        char realFilePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%d", "/proc/self/fd", h->fd);
#ifdef _WIN32
        strncpy_s(realFilePath, reinterpret_cast<char const *>(std::filesystem::canonical(filePath).c_str()), PATH_MAX);
        const int nBytes = strlen(realFilePath);
#else
        const int nBytes = readlink(filePath, realFilePath, PATH_MAX);
#endif
        realFilePath[nBytes] = '\0';
        //const auto p = new AwsMock::Service::S3Service();
        //p->PutObject(currentUser, realFilePath, currentServerId);
    }

    if (h->type == SFTP_FILE_HANDLE) {
        const int fd = h->fd;
        close(fd);
        ret = SSH_OK;
    } else if (h->type == SFTP_DIR_HANDLE) {
        DIR *dir = h->dirp;
        closedir(dir);
        ret = SSH_OK;
    } else {
        ret = SSH_ERROR;
    }

    SAFE_FREE(h->name);
    sftp_handle_remove(sftp, h);
    SAFE_FREE(h);

    if (ret == SSH_OK) {
        sftp_reply_status(client_msg, SSH_FX_OK, nullptr);
    } else {
        log_error << "Error closing file failed";
        sftp_reply_status(client_msg, SSH_FX_BAD_MESSAGE, "Invalid handle");
    }

    return SSH_OK;
}

static int process_opendir(sftp_client_message client_msg) {
    DIR *dir = nullptr;
    const char *dir_name = sftp_client_message_get_filename(client_msg);

    ssh_string handle_s = nullptr;
    struct sftp_handle *h = nullptr;

    log_debug << "Processing opendir: " << dir_name;

    dir = opendir(dir_name);
    if (dir == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "No such directory");
        return SSH_ERROR;
    }

    h = static_cast<struct sftp_handle *>(calloc(1, sizeof(struct sftp_handle)));
    if (h == nullptr) {
        closedir(dir);
        log_error << "Failed to allocate a new handle";
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "Failed to allocate new handle");
        return SSH_ERROR;
    }
    h->dirp = dir;
    h->name = strdup(dir_name);
    h->type = SFTP_DIR_HANDLE;
    handle_s = sftp_handle_alloc(client_msg->sftp, h);

    if (handle_s != nullptr) {
        sftp_reply_handle(client_msg, handle_s);
        ssh_string_free(handle_s);
    } else {
        closedir(dir);
        sftp_reply_status(client_msg, SSH_FX_FAILURE, "No handle available");
    }

    return SSH_OK;
}

static int readdir_long_name(char *z_file_name, struct stat *z_st, char *z_long_name) {
    char tmpbuf[MAX_LONG_NAME_LEN];
    char time[50];
    char *ptr = z_long_name;
    const int mode = z_st->st_mode;

    *ptr = '\0';

    switch (mode & S_IFMT) {
        case S_IFDIR:
            *ptr++ = 'd';
            break;
        default:
            *ptr++ = '-';
            break;
    }

    /* user */
    if (mode & 0400) *ptr++ = 'r';
    else
        *ptr++ = '-';

    if (mode & 0200) *ptr++ = 'w';
    else
        *ptr++ = '-';

#ifndef _WIN32
    if (mode & 0100) {
        if (mode & S_ISUID) *ptr++ = 's';
        else
            *ptr++ = 'x';
    } else
        *ptr++ = '-';
#endif

    /* group */
    if (mode & 040) *ptr++ = 'r';
    else
        *ptr++ = '-';
    if (mode & 020) *ptr++ = 'w';
    else
        *ptr++ = '-';
    if (mode & 010) *ptr++ = 'x';
    else
        *ptr++ = '-';

    /* other */
    if (mode & 04) *ptr++ = 'r';
    else
        *ptr++ = '-';
    if (mode & 02) *ptr++ = 'w';
    else
        *ptr++ = '-';
    if (mode & 01) *ptr++ = 'x';
    else
        *ptr++ = '-';

    *ptr++ = ' ';
    *ptr = '\0';

    snprintf(tmpbuf, sizeof(tmpbuf), "%3d %d %d %d", static_cast<int>(z_st->st_nlink), static_cast<int>(z_st->st_uid), static_cast<int>(z_st->st_gid), static_cast<int>(z_st->st_size));
    strcat(z_long_name, tmpbuf);

    ctime_r(&z_st->st_mtime, time);
    if ((ptr = strchr(time, '\n'))) {
        *ptr = '\0';
    }
    snprintf(tmpbuf, sizeof(tmpbuf), " %s %s", time + 4, z_file_name);
    strcat(z_long_name, tmpbuf);

    return SSH_OK;
}

static int process_readdir(const sftp_client_message client_msg) {
    const sftp_session sftp = client_msg->sftp;
    const ssh_string handle = client_msg->handle;
    const struct sftp_handle *h = nullptr;
    int ret = SSH_OK;
    int entries = 0;
    dirent *dentry = nullptr;
    DIR *dir = nullptr;
    const char *handle_name = nullptr;

    log_debug << "Processing readdir: " << reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle);

    awsmock_ssh_log_hexdump("Processing readdir: handle", reinterpret_cast<const unsigned char *>(ssh_string_get_char(handle)), ssh_string_len(handle));

    h = static_cast<struct sftp_handle *>(sftp_handle(sftp, client_msg->handle));
    if (h->type == SFTP_DIR_HANDLE) {
        dir = h->dirp;
        handle_name = h->name;
    }
    if (dir == nullptr) {
        log_error << "Got wrong handle from msg";
        sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
        return SSH_ERROR;
    }

    if (handle_name == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
        return SSH_ERROR;
    }

    int srclen = strlen(handle_name);
    if (srclen + 2 >= PATH_MAX) {
        log_error << "Handle string length exceed max length!";
        sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
        return SSH_ERROR;
    }

    for (int i = 0; i < MAX_ENTRIES_NUM_IN_PACKET; i++) {
        dentry = readdir(dir);

        if (dentry != nullptr) {
            char long_path[PATH_MAX];
            sftp_attributes_struct attr{};
            struct stat st{};

            if (strlen(dentry->d_name) + srclen + 1 >= PATH_MAX) {
                log_error << "Dandle string length exceed max length!";
                sftp_reply_status(client_msg, SSH_FX_INVALID_HANDLE, nullptr);
                return SSH_ERROR;
            }
            snprintf(long_path, PATH_MAX, "%s/%s", handle_name, dentry->d_name);

#ifndef _WIN32
            if (lstat(long_path, &st) == 0) {
                stat_to_filexfer_attrib(&st, &attr);
            } else {
                clear_filexfer_attrib(&attr);
            }
#endif
            if (char long_name[MAX_LONG_NAME_LEN]; readdir_long_name(dentry->d_name, &st, long_name) == 0) {
                sftp_reply_names_add(client_msg, dentry->d_name, long_name, &attr);
            } else {
                printf("readdir long name error\n");
            }

            entries++;
        } else {
            break;
        }
    }

    if (entries > 0) {
        ret = sftp_reply_names(client_msg);
    } else {
        sftp_reply_status(client_msg, SSH_FX_EOF, nullptr);
    }

    return ret;
}

static int process_mkdir(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *filename = sftp_client_message_get_filename(client_msg);
    const uint32_t msg_flags = client_msg->attr->flags;
    const uint32_t permission = client_msg->attr->permissions;
    const uint32_t mode = (msg_flags & static_cast<uint32_t>(SSH_FILEXFER_ATTR_PERMISSIONS)) ? permission & static_cast<uint32_t>(07777) : 0777;
    int status = SSH_FX_OK;
    int rv;

    log_debug << "Processing mkdir " << filename << ", mode: " << mode;

    if (filename == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

#ifdef _WIN32
    rv = _mkdir(filename);
#else
    rv = mkdir(filename, mode);
#endif
    if (rv < 0) {
        const int saved_errno = errno;
        log_error << "Failed to mkdir: " << strerror(saved_errno);
        status = unix_errno_to_ssh_stat(saved_errno);
        ret = SSH_ERROR;
    }

    sftp_reply_status(client_msg, status, nullptr);

    return ret;
}

static int process_rmdir(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *filename = sftp_client_message_get_filename(client_msg);
    int status = SSH_FX_OK;
    int rv;

    log_debug << "Processing rmdir: " << filename;

    if (filename == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

#ifdef _WIN32
    rv = _rmdir(filename);
#else
    rv = rmdir(filename);
#endif
    if (rv < 0) {
        status = unix_errno_to_ssh_stat(errno);
        ret = SSH_ERROR;
    }

    sftp_reply_status(client_msg, status, nullptr);

    return ret;
}

static int process_realpath(sftp_client_message client_msg) {
    const char *filename = sftp_client_message_get_filename(client_msg);
    char *path = nullptr;

    log_debug << "Processing realpath: " << filename;

#ifdef _WIN32
    // TODO: fix me
#else
    path = FtpFileNameToRealPath(filename);
#endif
    if (path == nullptr) {
        const int saved_errno = errno;
        const int status = unix_errno_to_ssh_stat(saved_errno);
        const char *err_msg = ssh_str_error(status);

        log_error << "Realpath failed: " << strerror(saved_errno);
        sftp_reply_status(client_msg, status, err_msg);
        return SSH_ERROR;
    }
    sftp_reply_name(client_msg, path, nullptr);
    log_debug << "Processed realpath: " << path;
    free(path);

    return SSH_OK;
}


static int process_lstat(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *filename = sftp_client_message_get_filename(client_msg);
    sftp_attributes_struct attr{};
    struct stat st{};

    log_debug << "Processing lstat, incoming: " << filename;
    filename = FtpFileNameToRealPath(const_cast<char *>(filename));
    log_debug << "Processing lstat, realPath: " << filename;

    if (filename == nullptr) {
        log_error << "File name error, filename: " << filename;
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

#ifdef _WIN32
    // TODO: fix me
#else
    const int rv = lstat(filename, &st);
    if (rv < 0) {
        int status = SSH_FX_OK;
        const int saved_errno = errno;
        log_error << "lstat failed: " << strerror(saved_errno) << ", filename: " << filename;
        status = unix_errno_to_ssh_stat(saved_errno);
        sftp_reply_status(client_msg, status, nullptr);
        ret = SSH_ERROR;
    } else {
        stat_to_filexfer_attrib(&st, &attr);
        sftp_reply_attr(client_msg, &attr);
    }
#endif

    return ret;
}

static int process_stat(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *filename = sftp_client_message_get_filename(client_msg);
    sftp_attributes_struct attr{};
    struct stat st{};

    filename = FtpFileNameToRealPath(filename);
    log_debug << "Processing stat: " << filename;

    if (filename == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

    if (const int rv = stat(filename, &st); rv < 0) {
        int status = SSH_FX_OK;
        const int saved_errno = errno;
        log_error << "lstat failed: " << strerror(saved_errno);
        status = unix_errno_to_ssh_stat(saved_errno);
        sftp_reply_status(client_msg, status, nullptr);
        ret = SSH_ERROR;
    } else {
        stat_to_filexfer_attrib(&st, &attr);
        sftp_reply_attr(client_msg, &attr);
    }

    return ret;
}

static int process_setstat(sftp_client_message client_msg) {
    int rv;
    constexpr int ret = SSH_OK;
    int status = SSH_FX_OK;
    uint32_t msg_flags = client_msg->attr->flags;
    const char *filename = sftp_client_message_get_filename(client_msg);

    log_debug << "Processing setstat: " << filename;

    if (filename == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

    if (msg_flags & SSH_FILEXFER_ATTR_SIZE) {
#ifdef _WIN32
        // TODO: fix me
#else
        rv = truncate(filename, static_cast<int>(client_msg->attr->size));
        if (rv < 0) {
            const int saved_errno = errno;
            log_error << "changing size failed: " << strerror(saved_errno);
            status = unix_errno_to_ssh_stat(saved_errno);
            sftp_reply_status(client_msg, status, nullptr);
            return rv;
        }
#endif
    }

    if (msg_flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
        rv = chmod(filename, client_msg->attr->permissions);
        if (rv < 0) {
            const int saved_errno = errno;
            log_error << "chmod failed: " << strerror(saved_errno);
            status = unix_errno_to_ssh_stat(saved_errno);
            sftp_reply_status(client_msg, status, nullptr);
            return rv;
        }
    }

    if (msg_flags & SSH_FILEXFER_ATTR_UIDGID) {
#ifdef _WIN32
        // TODO: fix me
#else
        rv = chown(filename, client_msg->attr->uid, client_msg->attr->gid);
        if (rv < 0) {
            const int saved_errno = errno;
            log_error << "chown failed: " << strerror(saved_errno);
            status = unix_errno_to_ssh_stat(saved_errno);
            sftp_reply_status(client_msg, status, nullptr);
            return rv;
        }
#endif
    }

    if (msg_flags & SSH_FILEXFER_ATTR_ACMODTIME) {
        timeval tv[2];

        tv[0].tv_sec = client_msg->attr->atime;
        tv[0].tv_usec = 0;
        tv[1].tv_sec = client_msg->attr->mtime;
        tv[1].tv_usec = 0;
#ifdef _WIN32
        // TODO: fix me
#else
        rv = utimes(filename, tv);
        if (rv < 0) {
            const int saved_errno = errno;
            log_error << "utimes failed: " << strerror(saved_errno);
            status = unix_errno_to_ssh_stat(saved_errno);
            sftp_reply_status(client_msg, status, nullptr);
            return rv;
        }
#endif
    }

    sftp_reply_status(client_msg, status, nullptr);
    return ret;
}

static int process_readlink(sftp_client_message client_msg) {
    const char *filename = sftp_client_message_get_filename(client_msg);
    char buf[PATH_MAX];
    int ret = SSH_OK;
    int len = -1;

    log_debug << "Processing readlink: " << filename;

    if (filename == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

#ifdef _WIN32
    // TODO: fix me
#else
    len = static_cast<int>(readlink(filename, buf, sizeof(buf) - 1));
    if (len < 0) {
        int status = SSH_FX_OK;
        const int saved_errno = errno;
        log_error << "readlink failed: " << strerror(saved_errno);
        status = unix_errno_to_ssh_stat(saved_errno);
        const char *err_msg = ssh_str_error(status);
        sftp_reply_status(client_msg, status, err_msg);
        ret = SSH_ERROR;
    } else {
        buf[len] = '\0';
        sftp_reply_name(client_msg, buf, nullptr);
    }
#endif
    return ret;
}

/**
 * Note, that this function is using reversed order of the arguments than the
 * OpenSSH sftp server as they have the arguments switched. See
 * section '4.1 sftp: Reversal of arguments to SSH_FXP_SYMLINK' in
 * https://github.com/openssh/openssh-portable/blob/master/PROTOCOL
 * for more information
 */
static int process_symlink(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *destpath = sftp_client_message_get_filename(client_msg);
    const char *srcpath = ssh_string_get_char(client_msg->data);

    log_debug << "processing symlink: src: " << srcpath << " dest: " << destpath;

    if (srcpath == nullptr || destpath == nullptr) {
        sftp_reply_status(client_msg, SSH_FX_NO_SUCH_FILE, "File name error");
        return SSH_ERROR;
    }

#ifdef _WIN32
    // TODO: fix me
#else
    if (const int rv = symlink(srcpath, destpath); rv < 0) {
        int status = SSH_FX_OK;
        const int saved_errno = errno;
        status = unix_errno_to_ssh_stat(saved_errno);
        log_debug << "symlink failed: " << strerror(saved_errno);
        sftp_reply_status(client_msg, status, "Write error");
        ret = SSH_ERROR;
    } else {
        sftp_reply_status(client_msg, SSH_FX_OK, "write success");
    }
#endif
    return ret;
}

static int
process_remove(sftp_client_message client_msg) {
    int ret = SSH_OK;
    const char *filename = sftp_client_message_get_filename(client_msg);
    int status = SSH_FX_OK;

    log_debug << "processing remove: " << filename;

    int rv = unlink(filename);
    if (rv < 0) {
        int saved_errno = errno;
        log_debug << "unlink failed: " << strerror(saved_errno);
        status = unix_errno_to_ssh_stat(saved_errno);
        ret = SSH_ERROR;
    }

    sftp_reply_status(client_msg, status, nullptr);

    return ret;
}

static int process_unsupported(sftp_client_message client_msg) {
    sftp_reply_status(client_msg, SSH_FX_OP_UNSUPPORTED, "Operation not supported");
    log_warning << "Message type not implemented: " << sftp_client_message_get_type(client_msg);
    return SSH_OK;
}

static int process_extended_statvfs(sftp_client_message client_msg) {
    const char *path = sftp_client_message_get_filename(client_msg);

#ifdef _WIN32
    // TODO: fix me
#else

    struct statvfs st{};

    log_debug << "processing extended statvfs: " << path;

    int rv = statvfs(path, &st);
    if (rv != 0) {
        const int saved_errno = errno;
        log_debug << "statvfs failed: " << strerror(saved_errno);
        const int status = unix_errno_to_ssh_stat(saved_errno);
        sftp_reply_status(client_msg, status, nullptr);
        return SSH_ERROR;
    }

    const auto sftp_statvfs = static_cast<sftp_statvfs_t>(calloc(1, sizeof(struct sftp_statvfs_struct)));
    if (sftp_statvfs == nullptr) {
        log_debug << "Failed to allocate statvfs structure";
        sftp_reply_status(client_msg, SSH_FX_FAILURE, nullptr);
        return SSH_ERROR;
    }
    uint64_t flag = (st.f_flag & ST_RDONLY) ? SSH_FXE_STATVFS_ST_RDONLY : 0;
    flag |= (st.f_flag & ST_NOSUID) ? SSH_FXE_STATVFS_ST_NOSUID : 0;

    sftp_statvfs->f_bsize = st.f_bsize;
    sftp_statvfs->f_frsize = st.f_frsize;
    sftp_statvfs->f_blocks = st.f_blocks;
    sftp_statvfs->f_bfree = st.f_bfree;
    sftp_statvfs->f_bavail = st.f_bavail;
    sftp_statvfs->f_files = st.f_files;
    sftp_statvfs->f_ffree = st.f_ffree;
    sftp_statvfs->f_favail = st.f_favail;
    sftp_statvfs->f_fsid = st.f_fsid;
    sftp_statvfs->f_flag = flag;
    sftp_statvfs->f_namemax = st.f_namemax;

    rv = sftp_reply_statvfs(client_msg, sftp_statvfs);
    free(sftp_statvfs);
    if (rv == 0) {
        return SSH_OK;
    }
#endif
    return SSH_ERROR;
}

sftp_session awsmock_sftp_server_new(ssh_session session, ssh_channel chan) {
    sftp_session sftp = nullptr;

    sftp = static_cast<sftp_session>(calloc(1, sizeof(sftp_session_struct)));
    if (sftp == nullptr) {
        ssh_set_error_oom(session);
        return nullptr;
    }

    sftp->read_packet = static_cast<sftp_packet>(calloc(1, sizeof(sftp_packet_struct)));
    if (sftp->read_packet == nullptr) {
        goto error;
    }

    sftp->read_packet->payload = ssh_buffer_new();
    if (sftp->read_packet->payload == nullptr) {
        goto error;
    }

    sftp->session = session;
    sftp->channel = chan;

    return sftp;

error:
    ssh_set_error_oom(session);
    if (sftp->read_packet != nullptr) {
        if (sftp->read_packet->payload != nullptr) {
            SSH_BUFFER_FREE(sftp->read_packet->payload);
        }
        SAFE_FREE(sftp->read_packet);
    }
    SAFE_FREE(sftp);
    return nullptr;
}

/**
 * @brief Default subsystem request handler for SFTP subsystem
 *
 * @param[in]  session   The ssh session
 * @param[in]  channel   The existing ssh channel
 * @param[in]  subsystem The subsystem name. Only "sftp" is handled
 * @param[out] userdata  The pointer to sftp_session which will get the
 *                       resulting SFTP session
 *
 * @return SSH_OK when the SFTP server was successfully initialized, SSH_ERROR
 *         otherwise.
 */
extern int sftp_channel_default_subsystem_request(ssh_session session, ssh_channel channel, const char *subsystem, void *userdata);
/*int sftp_channel_default_subsystem_request(ssh_session session, ssh_channel channel, const char *subsystem, void *userdata) {
    if (strcmp(subsystem, "sftp") == 0) {
        auto *sftp = static_cast<sftp_session *>(userdata);

        // initialize sftp session and file handler
        *sftp = awsmock_sftp_server_new(session, channel);
        if (*sftp == nullptr) {
            return SSH_ERROR;
        }

        return SSH_OK;
    }
    return SSH_ERROR;
}*/

extern int sftp_reply_version(sftp_client_message client_msg);
/*int sftp_reply_version(sftp_client_message client_msg) {
    const sftp_session sftp = client_msg->sftp;
    const ssh_session session = sftp->session;

    log_debug << "Sending version packet";

    const int version = sftp->client_version;
    ssh_buffer reply = ssh_buffer_new();
    if (reply == nullptr) {
        log_error << "Could not allocate reply buffer";
        ssh_set_error_oom(session);
        return -1;
    }

    int rc = ssh_buffer_pack(reply, "dssssss", LIBSFTP_VERSION, "posix-rename@openssh.com", "1", "hardlink@openssh.com", "1", "statvfs@openssh.com", "2");
    if (rc != SSH_OK) {
        ssh_set_error_oom(session);
        SSH_BUFFER_FREE(reply);
        return -1;
    }

    rc = awsmock_sftp_packet_write(sftp, SSH_FXP_VERSION, reply);
    if (rc < 0) {
        log_error << "Could not write to reply buffer";
        SSH_BUFFER_FREE(reply);
        return -1;
    }
    SSH_BUFFER_FREE(reply);

    log_debug << "Server version sent";

    if (version > LIBSFTP_VERSION) {
        sftp->version = LIBSFTP_VERSION;
    } else {
        sftp->version = version;
    }

    return SSH_OK;
}*/

/**
 * Functions to convert between host and network byte order.
 *
 * Please note that these functions normally take `unsigned long int' or
 * `unsigned short int' values as arguments and also return them.  But
 * this was a short-sighted decision since on different systems the types
 * may have different representations but the values are always the same.
 */
const sftp_message_handler message_handlers[] = {
        {"open", nullptr, SSH_FXP_OPEN, process_open},
        {"close", nullptr, SSH_FXP_CLOSE, process_close},
        {"read", nullptr, SSH_FXP_READ, process_read},
        {"write", nullptr, SSH_FXP_WRITE, process_write},
        {"lstat", nullptr, SSH_FXP_LSTAT, process_lstat},
        {"fstat", nullptr, SSH_FXP_FSTAT, process_unsupported},
        {"setstat", nullptr, SSH_FXP_SETSTAT, process_setstat},//7
        {"fsetstat", nullptr, SSH_FXP_FSETSTAT, process_unsupported},
        {"opendir", nullptr, SSH_FXP_OPENDIR, process_opendir},
        {"readdir", nullptr, SSH_FXP_READDIR, process_readdir},
        {"remove", nullptr, SSH_FXP_REMOVE, process_remove},
        {"mkdir", nullptr, SSH_FXP_MKDIR, process_mkdir},
        {"rmdir", nullptr, SSH_FXP_RMDIR, process_rmdir},
        {"realpath", nullptr, SSH_FXP_REALPATH, process_realpath},//14
        {"stat", nullptr, SSH_FXP_STAT, process_stat},
        {"rename", nullptr, SSH_FXP_RENAME, process_unsupported},
        {"readlink", nullptr, SSH_FXP_READLINK, process_readlink},
        {"symlink", nullptr, SSH_FXP_SYMLINK, process_symlink},
        {"init", nullptr, SSH_FXP_INIT, sftp_reply_version},
        {nullptr, nullptr, 0, nullptr},
};

const sftp_message_handler extended_handlers[] = {
        /* here are some extended type handlers */
        {"statvfs", "statvfs@openssh.com", 0, process_extended_statvfs},
        {nullptr, nullptr, 0, nullptr},
};

static int process_extended(sftp_client_message sftp_msg) {
    int status = SSH_ERROR;
    const char *subtype = sftp_msg->submessage;
    sftp_server_message_callback handler = nullptr;

    log_debug << "processing extended message: " << subtype;

    for (int i = 0; extended_handlers[i].cb != nullptr; i++) {
        if (strcmp(subtype, extended_handlers[i].extended_name) == 0) {
            handler = extended_handlers[i].cb;
            break;
        }
    }
    if (handler != nullptr) {
        status = handler(sftp_msg);
        return status;
    }

    sftp_reply_status(sftp_msg, SSH_FX_OP_UNSUPPORTED, "Extended Operation not supported");
    log_debug << "Extended Message type %s not implemented" << subtype;
    return SSH_OK;
}

static int dispatch_sftp_request(sftp_client_message sftp_msg) {
    int status = SSH_ERROR;
    sftp_server_message_callback handler = nullptr;
    const uint8_t type = sftp_client_message_get_type(sftp_msg);

    if (message_handlers[type].name)
        log_trace << "Dispatch request, type: " << std::to_string(type) << ", name: " << message_handlers[type].name;

    for (int i = 0; message_handlers[i].cb != nullptr; i++) {
        if (type == message_handlers[i].type) {
            handler = message_handlers[i].cb;
            break;
        }
    }

    if (handler != nullptr) {
        status = handler(sftp_msg);
    } else {
        sftp_reply_status(sftp_msg, SSH_FX_OP_UNSUPPORTED, "Operation not supported");
        log_warning << "Message type not implemented, type: " << std::to_string(type) << ", name: " << message_handlers[type].name;
        return SSH_OK;
    }

    return status;
}

static int process_client_message(const sftp_client_message &client_msg) {
    int status = SSH_OK;
    if (client_msg == nullptr) {
        return SSH_ERROR;
    }

    switch (client_msg->type) {
        case SSH_FXP_EXTENDED:
            status = process_extended(client_msg);
            break;
        default:
            status = dispatch_sftp_request(client_msg);
    }

    if (status != SSH_OK)
        log_debug << "error occurred during processing client message!";

    return status;
}

enum sftp_longname_field_e {
    SFTP_LONGNAME_PERM = 0,
    SFTP_LONGNAME_FIXME,
    SFTP_LONGNAME_OWNER,
    SFTP_LONGNAME_GROUP,
    SFTP_LONGNAME_SIZE,
    SFTP_LONGNAME_DATE,
    SFTP_LONGNAME_TIME,
    SFTP_LONGNAME_NAME,
};

static char *sftp_parse_longname(const char *longname, enum sftp_longname_field_e longname_field) {
    size_t field = 0;

    const char *p = longname;

    // Find the beginning of the field which is specified by sftp_longname_field_e.
    while (field != longname_field) {
        if (isspace(*p)) {
            field++;
            p++;
            while (*p && isspace(*p)) {
                p++;
            }
        } else {
            p++;
        }
    }

    const char *q = p;
    while (!isspace(*q)) {
        q++;
    }

    const size_t len = q - p;
#ifdef _WIN32
    return mystrndup(p, len);
#else
    return strndup(p, len);
#endif
}

/**
 * sftp version 0-3 code. It is different from the v4 maybe a paste of the draft is better than the code
 *
 *   uint32   flags
 *   uint64   size           present only if flag SSH_FILEXFER_ATTR_SIZE
 *   uint32   uid            present only if flag SSH_FILEXFER_ATTR_UIDGID
 *   uint32   gid            present only if flag SSH_FILEXFER_ATTR_UIDGID
 *   uint32   permissions    present only if flag SSH_FILEXFER_ATTR_PERMISSIONS
 *   uint32   atime          present only if flag SSH_FILEXFER_ACMODTIME
 *   uint32   mtime          present only if flag SSH_FILEXFER_ACMODTIME
 *   uint32   extended_count present only if flag SSH_FILEXFER_ATTR_EXTENDED
 *   string   extended_type
 *   string   extended_data
 *   ...      more extended data (extended_type - extended_data pairs), so that number of pairs equals extended_count
 */
static sftp_attributes sftp_parse_attr_3(sftp_session sftp, ssh_buffer buf, int expectname) {
    sftp_attributes attr;
    int rc;

    attr = static_cast<sftp_attributes>(calloc(1, sizeof(struct sftp_attributes_struct)));
    if (attr == nullptr) {
        ssh_set_error_oom(sftp->session);
        sftp_set_error(sftp, SSH_FX_FAILURE);
        return nullptr;
    }

    if (expectname) {
        rc = ssh_buffer_unpack(buf, "ss",
                               &attr->name,
                               &attr->longname);
        if (rc != SSH_OK) {
            goto error;
        }
        log_debug << "Attribute name: " << attr->name;

        /* Set owner and group if we talk to openssh and have the longname */
        if (ssh_get_openssh_version(sftp->session)) {
            attr->owner = sftp_parse_longname(attr->longname, SFTP_LONGNAME_OWNER);
            if (attr->owner == nullptr) {
                goto error;
            }

            attr->group = sftp_parse_longname(attr->longname, SFTP_LONGNAME_GROUP);
            if (attr->group == nullptr) {
                goto error;
            }
        }
    }

    rc = ssh_buffer_unpack(buf, "d", &attr->flags);
    if (rc != SSH_OK) {
        goto error;
    }
    log_debug << "Flags: " << attr->flags;

    if (attr->flags & SSH_FILEXFER_ATTR_SIZE) {
        rc = ssh_buffer_unpack(buf, "q", &attr->size);
        if (rc != SSH_OK) {
            goto error;
        }
        log_debug << "Size: " << attr->size;
    }

    if (attr->flags & SSH_FILEXFER_ATTR_UIDGID) {
        rc = ssh_buffer_unpack(buf, "dd", &attr->uid, &attr->gid);
        if (rc != SSH_OK) {
            goto error;
        }
    }

    if (attr->flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
        rc = ssh_buffer_unpack(buf, "d", &attr->permissions);
        if (rc != SSH_OK) {
            goto error;
        }

        switch (attr->permissions & SSH_S_IFMT) {
            case SSH_S_IFSOCK:
            case SSH_S_IFBLK:
            case SSH_S_IFCHR:
            case SSH_S_IFIFO:
                attr->type = SSH_FILEXFER_TYPE_SPECIAL;
                break;
            case SSH_S_IFLNK:
                attr->type = SSH_FILEXFER_TYPE_SYMLINK;
                break;
            case SSH_S_IFREG:
                attr->type = SSH_FILEXFER_TYPE_REGULAR;
                break;
            case SSH_S_IFDIR:
                attr->type = SSH_FILEXFER_TYPE_DIRECTORY;
                break;
            default:
                attr->type = SSH_FILEXFER_TYPE_UNKNOWN;
                break;
        }
    }

    if (attr->flags & SSH_FILEXFER_ATTR_ACMODTIME) {
        rc = ssh_buffer_unpack(buf, "dd",
                               &attr->atime,
                               &attr->mtime);
        if (rc != SSH_OK) {
            goto error;
        }
    }

    if (attr->flags & SSH_FILEXFER_ATTR_EXTENDED) {
        rc = ssh_buffer_unpack(buf, "d", &attr->extended_count);
        if (rc != SSH_OK) {
            goto error;
        }

        if (attr->extended_count > 0) {
            rc = ssh_buffer_unpack(buf, "ss", &attr->extended_type, &attr->extended_data);
            if (rc != SSH_OK) {
                goto error;
            }
            attr->extended_count--;
        }
        /* just ignore the remaining extensions */

        while (attr->extended_count > 0) {
            ssh_string tmp1, tmp2;
            rc = ssh_buffer_unpack(buf, "SS", &tmp1, &tmp2);
            if (rc != SSH_OK) {
                goto error;
            }
            SAFE_FREE(tmp1);
            SAFE_FREE(tmp2);
            attr->extended_count--;
        }
    }

    return attr;

error:
    SSH_STRING_FREE(attr->extended_type);
    SSH_STRING_FREE(attr->extended_data);
    SAFE_FREE(attr->name);
    SAFE_FREE(attr->longname);
    SAFE_FREE(attr->owner);
    SAFE_FREE(attr->group);
    SAFE_FREE(attr);
    ssh_set_error(sftp->session, SSH_FATAL, "Invalid ATTR structure");
    sftp_set_error(sftp, SSH_FX_FAILURE);

    return nullptr;
}

/**
 * @brief gets a 32 bits unsigned int out of the buffer. Adjusts the read pointer.
 *
 * @param[in]  buffer   The buffer to read.
 * @param[in]  data     A pointer to a uint32_t where to store the data.
 * @returns             0 if there is not enough data in buffer, 4 otherwise.
 */
uint32_t awsmock_ssh_buffer_get_u32(ssh_buffer_struct *buffer, uint32_t *data) {
    return ssh_buffer_get_data(buffer, data, sizeof(uint32_t));
}

/**
 * @brief Get a 64 bits unsigned int out of the buffer and adjusts the read
 * pointer.
 *
 * @param[in]  buffer   The buffer to read.
 * @param[in]  data     A pointer to a uint64_t where to store the data.
 * @returns             0 if there is not enough data in buffer, 8 otherwise.
 */
uint32_t awsmock_ssh_buffer_get_u64(ssh_buffer_struct *buffer, uint64_t *data) {
    return ssh_buffer_get_data(buffer, data, sizeof(uint64_t));
}

/**
 * @brief Validates that the given length can be obtained from the buffer.
 *
 * @param[in]  buffer  The buffer to read from.
 * @param[in]  len     The length to be checked.
 * @return             SSH_OK if the length is valid, SSH_ERROR otherwise.
 */
int awsmock_ssh_buffer_validate_length(struct ssh_buffer_struct *buffer, size_t len) {
    if (buffer == nullptr || buffer->pos + len < len ||
        buffer->pos + len > buffer->used) {
        return SSH_ERROR;
    }

    return SSH_OK;
}

/**
 * @brief Get an SSH String out of the buffer and adjust the read pointer.
 *
 * @param[in]  buffer   The buffer to read.
 * @returns             The SSH String, nullptr on error.
 */
ssh_string_struct *awsmock_ssh_buffer_get_ssh_string(ssh_buffer_struct *buffer) {
    uint32_t stringlen;
    ssh_string_struct *str = nullptr;

    int rc = awsmock_ssh_buffer_get_u32(buffer, &stringlen);
    if (rc == 0) {
        return nullptr;
    }
    uint32_t hostlen = ntohl(stringlen);

    // verify if there is enough space in buffer to get it
    rc = awsmock_ssh_buffer_validate_length(buffer, hostlen);
    if (rc != SSH_OK) {
        return nullptr; /* it is indeed */
    }
    str = ssh_string_new(hostlen);
    if (str == nullptr) {
        return nullptr;
    }

    stringlen = ssh_buffer_get_data(buffer, ssh_string_data(str), hostlen);
    if (stringlen != hostlen) {
        // should never happen
        SAFE_FREE(str);
        return nullptr;
    }

    return str;
}

/**
 * Parse the attributes from a payload from some messages. It is coded on
 * baselines from the protocol version 4.
 * This code is more or less dead but maybe we will need it in the future.
 */
static sftp_attributes sftp_parse_attr_4(sftp_session sftp, ssh_buffer buf, int expectnames) {
    sftp_attributes attr = nullptr;
    ssh_string owner = nullptr;
    ssh_string group = nullptr;
    uint32_t flags = 0;
    int ok = 0;

    /* unused member variable */
    (void) expectnames;

    attr = static_cast<sftp_attributes>(calloc(1, sizeof(struct sftp_attributes_struct)));
    if (attr == nullptr) {
        ssh_set_error_oom(sftp->session);
        sftp_set_error(sftp, SSH_FX_FAILURE);
        return nullptr;
    }

    /* This isn't really a loop, but it is like a try..catch.. */
    do {
        if (awsmock_ssh_buffer_get_u32(buf, &flags) != 4) {
            break;
        }

        flags = ntohl(flags);
        attr->flags = flags;

        if (flags & SSH_FILEXFER_ATTR_SIZE) {
            if (awsmock_ssh_buffer_get_u64(buf, &attr->size) != 8) {
                break;
            }
            attr->size = ntohll(attr->size);
        }

        if (flags & SSH_FILEXFER_ATTR_OWNERGROUP) {
            owner = awsmock_ssh_buffer_get_ssh_string(buf);
            if (owner == nullptr) {
                break;
            }
            attr->owner = ssh_string_to_char(owner);
            SSH_STRING_FREE(owner);
            if (attr->owner == nullptr) {
                break;
            }

            group = awsmock_ssh_buffer_get_ssh_string(buf);
            if (group == nullptr) {
                break;
            }
            attr->group = ssh_string_to_char(group);
            SSH_STRING_FREE(group);
            if (attr->group == nullptr) {
                break;
            }
        }

        if (flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
            if (awsmock_ssh_buffer_get_u32(buf, &attr->permissions) != 4) {
                break;
            }
            attr->permissions = ntohl(attr->permissions);

            /* FIXME on windows! */
            switch (attr->permissions & SSH_S_IFMT) {
                case SSH_S_IFSOCK:
                case SSH_S_IFBLK:
                case SSH_S_IFCHR:
                case SSH_S_IFIFO:
                    attr->type = SSH_FILEXFER_TYPE_SPECIAL;
                    break;
                case SSH_S_IFLNK:
                    attr->type = SSH_FILEXFER_TYPE_SYMLINK;
                    break;
                case SSH_S_IFREG:
                    attr->type = SSH_FILEXFER_TYPE_REGULAR;
                    break;
                case SSH_S_IFDIR:
                    attr->type = SSH_FILEXFER_TYPE_DIRECTORY;
                    break;
                default:
                    attr->type = SSH_FILEXFER_TYPE_UNKNOWN;
                    break;
            }
        }

        if (flags & SSH_FILEXFER_ATTR_ACCESSTIME) {
            if (awsmock_ssh_buffer_get_u64(buf, &attr->atime64) != 8) {
                break;
            }
            attr->atime64 = ntohll(attr->atime64);

            if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
                if (awsmock_ssh_buffer_get_u32(buf, &attr->atime_nseconds) != 4) {
                    break;
                }
                attr->atime_nseconds = ntohl(attr->atime_nseconds);
            }
        }

        if (flags & SSH_FILEXFER_ATTR_CREATETIME) {
            if (awsmock_ssh_buffer_get_u64(buf, &attr->createtime) != 8) {
                break;
            }
            attr->createtime = ntohll(attr->createtime);

            if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
                if (awsmock_ssh_buffer_get_u32(buf, &attr->createtime_nseconds) != 4) {
                    break;
                }
                attr->createtime_nseconds = ntohl(attr->createtime_nseconds);
            }
        }

        if (flags & SSH_FILEXFER_ATTR_MODIFYTIME) {
            if (awsmock_ssh_buffer_get_u64(buf, &attr->mtime64) != 8) {
                break;
            }
            attr->mtime64 = ntohll(attr->mtime64);

            if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
                if (awsmock_ssh_buffer_get_u32(buf, &attr->mtime_nseconds) != 4) {
                    break;
                }
                attr->mtime_nseconds = ntohl(attr->mtime_nseconds);
            }
        }

        if (flags & SSH_FILEXFER_ATTR_ACL) {
            if ((attr->acl = awsmock_ssh_buffer_get_ssh_string(buf)) == nullptr) {
                break;
            }
        }

        if (flags & SSH_FILEXFER_ATTR_EXTENDED) {
            if (awsmock_ssh_buffer_get_u32(buf, &attr->extended_count) != 4) {
                break;
            }
            attr->extended_count = ntohl(attr->extended_count);

            while (attr->extended_count &&
                   ((attr->extended_type = awsmock_ssh_buffer_get_ssh_string(buf))) &&
                   ((attr->extended_data = awsmock_ssh_buffer_get_ssh_string(buf)))) {
                attr->extended_count--;
            }

            if (attr->extended_count) {
                break;
            }
        }
        ok = 1;
    } while (0);

    if (ok == 0) {
        /* break issued somewhere */
        SSH_STRING_FREE(attr->acl);
        SSH_STRING_FREE(attr->extended_type);
        SSH_STRING_FREE(attr->extended_data);
        SAFE_FREE(attr->owner);
        SAFE_FREE(attr->group);
        SAFE_FREE(attr);

        ssh_set_error(sftp->session, SSH_FATAL, "Invalid ATTR structure");

        return nullptr;
    }

    return attr;
}

sftp_attributes awsmock_sftp_parse_attr(sftp_session session, ssh_buffer buf, int expectname) {
    switch (session->version) {
        case 4:
            return sftp_parse_attr_4(session, buf, expectname);
        case 3:
        case 2:
        case 1:
        case 0:
            return sftp_parse_attr_3(session, buf, expectname);
        default:
            ssh_set_error(session->session, SSH_FATAL, "Version %d unsupported by client", session->server_version);
            return nullptr;
    }

    return nullptr;
}

static sftp_client_message sftp_make_client_message(sftp_session sftp, sftp_packet packet) {
    ssh_session session = sftp->session;
    sftp_client_message msg = nullptr;
    ssh_buffer payload = nullptr;
    int version, rc;

    msg = static_cast<sftp_client_message>(calloc(1, sizeof(sftp_client_message_struct)));
    if (msg == nullptr) {
        ssh_set_error_oom(session);
        return nullptr;
    }

    payload = packet->payload;
    msg->type = packet->type;
    msg->sftp = sftp;

    // take a copy of the whole packet
    msg->complete_message = ssh_buffer_new();
    if (msg->complete_message == nullptr) {
        ssh_set_error_oom(session);
        goto error;
    }

    rc = ssh_buffer_add_data(msg->complete_message, ssh_buffer_get(payload), ssh_buffer_get_len(payload));
    if (rc < 0) {
        goto error;
    }

    if (msg->type != SSH_FXP_INIT) {
        rc = static_cast<int>(awsmock_ssh_buffer_get_u32(payload, &msg->id));
        if (rc != sizeof(uint32_t)) {
            goto error;
        }
    }

    switch (msg->type) {
        case SSH_FXP_INIT:
            rc = ssh_buffer_unpack(payload, "d", &version);
            if (rc != SSH_OK) {
                printf("unpack init failed!\n");
                goto error;
            }
            version = ntohl(version);
            sftp->client_version = version;
            break;
        case SSH_FXP_CLOSE:
        case SSH_FXP_READDIR:
            msg->handle = awsmock_ssh_buffer_get_ssh_string(payload);
            if (msg->handle == nullptr) {
                goto error;
            }
            break;
        case SSH_FXP_READ:
            rc = ssh_buffer_unpack(payload, "Sqd", &msg->handle, &msg->offset, &msg->len);
            if (rc != SSH_OK) {
                goto error;
            }
            break;
        case SSH_FXP_WRITE:
            rc = ssh_buffer_unpack(payload, "SqS", &msg->handle, &msg->offset, &msg->data);
            if (rc != SSH_OK) {
                goto error;
            }
            break;
        case SSH_FXP_REMOVE:
        case SSH_FXP_RMDIR:
        case SSH_FXP_OPENDIR:
        case SSH_FXP_READLINK:
        case SSH_FXP_REALPATH:
            rc = ssh_buffer_unpack(payload, "s", &msg->filename);
            if (rc != SSH_OK) {
                goto error;
            }
            break;
        case SSH_FXP_RENAME:
        case SSH_FXP_SYMLINK:
            rc = ssh_buffer_unpack(payload, "sS", &msg->filename, &msg->data);
            if (rc != SSH_OK) {
                goto error;
            }
            break;
        case SSH_FXP_MKDIR:
        case SSH_FXP_SETSTAT:
            rc = ssh_buffer_unpack(payload, "s", &msg->filename);
            if (rc != SSH_OK) {
                goto error;
            }
            msg->attr = awsmock_sftp_parse_attr(sftp, payload, 0);
            if (msg->attr == nullptr) {
                goto error;
            }
            break;
        case SSH_FXP_FSETSTAT:
            msg->handle = awsmock_ssh_buffer_get_ssh_string(payload);
            if (msg->handle == nullptr) {
                goto error;
            }
            msg->attr = awsmock_sftp_parse_attr(sftp, payload, 0);
            if (msg->attr == nullptr) {
                goto error;
            }
            break;
        case SSH_FXP_LSTAT:
        case SSH_FXP_STAT:
            rc = ssh_buffer_unpack(payload,
                                   "s",
                                   &msg->filename);
            if (rc != SSH_OK) {
                goto error;
            }
            if (sftp->version > 3) {
                ssh_buffer_unpack(payload, "d", &msg->flags);
            }
            break;
        case SSH_FXP_OPEN:
            rc = ssh_buffer_unpack(payload,
                                   "sd",
                                   &msg->filename,
                                   &msg->flags);
            if (rc != SSH_OK) {
                goto error;
            }
            msg->attr = awsmock_sftp_parse_attr(sftp, payload, 0);
            if (msg->attr == nullptr) {
                goto error;
            }
            break;
        case SSH_FXP_FSTAT:
            rc = ssh_buffer_unpack(payload,
                                   "S",
                                   &msg->handle);
            if (rc != SSH_OK) {
                goto error;
            }
            break;
        case SSH_FXP_EXTENDED:
            rc = ssh_buffer_unpack(payload,
                                   "s",
                                   &msg->submessage);
            if (rc != SSH_OK) {
                goto error;
            }

            if (strcmp(msg->submessage, "hardlink@openssh.com") == 0 ||
                strcmp(msg->submessage, "posix-rename@openssh.com") == 0) {
                rc = ssh_buffer_unpack(payload,
                                       "sS",
                                       &msg->filename,
                                       &msg->data);
                if (rc != SSH_OK) {
                    goto error;
                }
            } else if (strcmp(msg->submessage, "statvfs@openssh.com") == 0) {
                rc = ssh_buffer_unpack(payload, "s", &msg->filename);
                if (rc != SSH_OK) {
                    goto error;
                }
            }
            break;
        default:
            ssh_set_error(sftp->session, SSH_FATAL, "Received unhandled sftp message %d", msg->type);
            goto error;
    }

    return msg;

error:
    sftp_client_message_free(msg);
    return nullptr;
}

/**
 * @brief Get the client message from a sftp packet.
 *
 * @param  sftp         The sftp session handle.
 *
 * @return              The pointer to the generated sftp client message.
 */
static sftp_client_message sftp_get_client_message_from_packet(sftp_session sftp) {
    sftp_packet packet = nullptr;

    packet = sftp->read_packet;
    if (packet == nullptr) {
        return nullptr;
    }
    return sftp_make_client_message(sftp, packet);
}

/**
 * Process the incoming data and copy them from the SSH packet buffer to the
 * SFTP packet buffer.
 * @returns number of decoded bytes.
 */
extern int sftp_decode_channel_data_to_packet(sftp_session sftp, void *data, uint32_t len);
/*int sftp_decode_channel_data_to_packet(sftp_session sftp, void *data, uint32_t len) {
    const sftp_packet packet = sftp->read_packet;

    if (packet->sftp == nullptr) {
        packet->sftp = sftp;
    }

    constexpr unsigned int data_offset = sizeof(uint32_t) + sizeof(uint8_t);
    // not enough bytes to read
    if (len < data_offset) {
        return SSH_ERROR;
    }

    const int payload_len = PULL_BE_U32(data, 0);
    packet->type = PULL_BE_U8(data, 4);

    // We should check the legality of payload length
    if (payload_len + sizeof(uint32_t) > len || payload_len < 0) {
        return SSH_ERROR;
    }

    const int to_read = static_cast<int>(payload_len - sizeof(uint8_t));
    if (const int rc = ssh_buffer_add_data(packet->payload, (static_cast<uint8_t *>(data) + data_offset), to_read); rc != 0) {
        return SSH_ERROR;
    }

    // We should check if we copied the whole data
    if (const int nread = static_cast<int>(ssh_buffer_get_len(packet->payload)); nread != to_read) {
        return SSH_ERROR;
    }

    // We should return how many bytes we decoded, including packet length header and the payload length.
    return static_cast<int>(payload_len + sizeof(uint32_t));
}*/

/**
 * @brief Default data callback for sftp server
 *
 * @param[in] session   The ssh session
 * @param[in] channel   The ssh channel with SFTP session opened
 * @param[in] data      The data to be processed.
 * @param[in] len       The length of input data to be processed
 * @param[in] is_stderr Unused channel flag for stderr flagging
 * @param[in] userdata  The pointer to sftp_session
 *
 * @return number of bytes processed, -1 when error occurs.
 */
extern auto sftp_channel_default_data_callback([[maybe_unused]] ssh_session session, [[maybe_unused]] ssh_channel channel, void *data, const uint32_t len, [[maybe_unused]] int is_stderr, void *userdata) -> int;
/*auto sftp_channel_default_data_callback([[maybe_unused]] ssh_session session, [[maybe_unused]] ssh_channel channel, void *data, const uint32_t len, [[maybe_unused]] int is_stderr, void *userdata) -> int {
    auto *sftpp = static_cast<sftp_session *>(userdata);
    sftp_session sftp = nullptr;

    if (sftpp == nullptr) {
        log_warning << "nullptr userdata passed to callback";
        return -1;
    }
    sftp = *sftpp;

    const int decode_len = sftp_decode_channel_data_to_packet(sftp, data, len);
    if (decode_len == -1) return -1;

    const sftp_client_message msg = sftp_get_client_message_from_packet(sftp);
    const int rc = process_client_message(msg);
    sftp_client_message_free(msg);
    if (rc != SSH_OK)
        log_debug << "process sftp failed!";

    return decode_len;
}*/

/**
 * @brief Get a 8 bits unsigned int out of the buffer and adjust the read
 * pointer.
 *
 * @param[in]  buffer   The buffer to read.
 * @param[in]  data     A pointer to a uint8_t where to store the data.
 * @returns             0 if there is not enough data in buffer, 1 otherwise.
 */
uint32_t awsmock_ssh_buffer_get_u8(struct ssh_buffer_struct *buffer, uint8_t *data) {
    return ssh_buffer_get_data(buffer, data, sizeof(uint8_t));
}

bignum ssh_make_string_bn(ssh_string string) {
    bignum bn = nullptr;
    size_t len = ssh_string_len(string);

#ifdef DEBUG_CRYPTO
    SSH_LOG(SSH_LOG_TRACE,
            "Importing a %zu bits, %zu bytes object ...",
            len * 8, len);
#endif /* DEBUG_CRYPTO */

    bignum_bin2bn(string->data, len, &bn);

    return bn;
}

/** @internal
 * @brief Get multiple values from a buffer on a single function call
 * @param[in] buffer    The buffer to get from
 * @param[in] format    A format string of arguments.
 * @param[in] ap        A va_list of arguments.
 * @returns             SSH_OK on success
 *                      SSH_ERROR on error
 * @see ssh_buffer_get_format() for format list values.
 */
extern int ssh_buffer_unpack_va(ssh_buffer_struct *buffer, const char *format, size_t argc, va_list ap);
/*int ssh_buffer_unpack_va(ssh_buffer_struct *buffer, const char *format, size_t argc, va_list ap) {
    int rc = SSH_ERROR;
    const char *p = format;
    union {
        uint8_t *byte;
        uint16_t *word;
        uint32_t *dword;
        uint64_t *qword;
        ssh_string *string;
        char **cstring;
        BIGNUM **bignum;
        void **data;
    } o{};
    size_t len;
    uint32_t rlen, max_len;
    ssh_string tmp_string = nullptr;
    va_list ap_copy;
    size_t count;

    max_len = ssh_buffer_get_len(buffer);

    // copy the argument list in case a rollback is needed
    va_copy(ap_copy, ap);

    if (argc > 256) {
        rc = SSH_ERROR;
        goto cleanup;
    }

    for (count = 0; *p != '\0'; p++, count++) {
        // Invalid number of arguments passed
        if (count > argc) {
            rc = SSH_ERROR;
            goto cleanup;
        }

        rc = SSH_ERROR;
        switch (*p) {
            case 'b':
                o.byte = va_arg(ap, uint8_t *);
                rlen = awsmock_ssh_buffer_get_u8(buffer, o.byte);
                rc = rlen == 1 ? SSH_OK : SSH_ERROR;
                break;
            case 'w':
                o.word = va_arg(ap, uint16_t *);
                rlen = ssh_buffer_get_data(buffer, o.word, sizeof(uint16_t));
                if (rlen == 2) {
                    *o.word = ntohs(*o.word);
                    rc = SSH_OK;
                }
                break;
            case 'd':
                o.dword = va_arg(ap, uint32_t *);
                rlen = awsmock_ssh_buffer_get_u32(buffer, o.dword);
                if (rlen == 4) {
                    *o.dword = ntohl(*o.dword);
                    rc = SSH_OK;
                }
                break;
            case 'q':
                o.qword = va_arg(ap, uint64_t *);
                rlen = awsmock_ssh_buffer_get_u64(buffer, o.qword);
                if (rlen == 8) {
                    *o.qword = ntohll(*o.qword);
                    rc = SSH_OK;
                }
                break;
            case 'B':
                o.bignum = va_arg(ap, bignum *);
                *o.bignum = nullptr;
                tmp_string = awsmock_ssh_buffer_get_ssh_string(buffer);
                if (tmp_string == nullptr) {
                    break;
                }
                *o.bignum = ssh_make_string_bn(tmp_string);
                ssh_string_burn(tmp_string);
                SSH_STRING_FREE(tmp_string);
                rc = (*o.bignum != nullptr) ? SSH_OK : SSH_ERROR;
                break;
            case 'S':
                o.string = va_arg(ap, ssh_string *);
                *o.string = awsmock_ssh_buffer_get_ssh_string(buffer);
                rc = *o.string != nullptr ? SSH_OK : SSH_ERROR;
                o.string = nullptr;
                break;
            case 's': {
                uint32_t u32len = 0;

                o.cstring = va_arg(ap, char **);
                *o.cstring = nullptr;
                rlen = awsmock_ssh_buffer_get_u32(buffer, &u32len);
                if (rlen != 4) {
                    break;
                }
                len = ntohl(u32len);
                if (len > max_len - 1) {
                    break;
                }

                rc = awsmock_ssh_buffer_validate_length(buffer, len);
                if (rc != SSH_OK) {
                    break;
                }

                *o.cstring = reinterpret_cast<char *>(static_cast<char **>(malloc(len + 1)));
                if (*o.cstring == nullptr) {
                    rc = SSH_ERROR;
                    break;
                }
                rlen = ssh_buffer_get_data(buffer, *o.cstring, len);
                if (rlen != len) {
                    SAFE_FREE(*o.cstring);
                    rc = SSH_ERROR;
                    break;
                }
                (*o.cstring)[len] = '\0';
                o.cstring = nullptr;
                rc = SSH_OK;
                break;
            }
            case 'P':
                len = va_arg(ap, size_t);
                if (len > max_len - 1) {
                    rc = SSH_ERROR;
                    break;
                }

                rc = awsmock_ssh_buffer_validate_length(buffer, len);
                if (rc != SSH_OK) {
                    break;
                }

                o.data = va_arg(ap, void **);
                count++;

                *o.data = malloc(len);
                if (*o.data == nullptr) {
                    rc = SSH_ERROR;
                    break;
                }
                rlen = ssh_buffer_get_data(buffer, *o.data, len);
                if (rlen != len) {
                    SAFE_FREE(*o.data);
                    rc = SSH_ERROR;
                    break;
                }
                o.data = nullptr;
                rc = SSH_OK;
                break;
            default:
                log_trace << "Invalid buffer format" << *p;
        }
        if (rc != SSH_OK) {
            break;
        }
    }

    if (argc != count) {
        rc = SSH_ERROR;
    }

cleanup:
    if (rc != SSH_ERROR) {
        // Check if our canary is intact, if not something really bad happened
        uint32_t canary = va_arg(ap, uint32_t);
        if (canary != SSH_BUFFER_PACK_END) {
            abort();
        }
    }

    if (rc != SSH_OK) {
        const char *last;
        // Reset the format string and erase everything that was allocated
        last = p;
        for (p = format; p < last; ++p) {
            switch (*p) {
                case 'b':
                    o.byte = va_arg(ap_copy, uint8_t *);
                    if (buffer->secure) {
                        memset(o.byte, 0, sizeof(uint8_t));
                        break;
                    }
                    break;
                case 'w':
                    o.word = va_arg(ap_copy, uint16_t *);
                    if (buffer->secure) {
                        memset(o.word, 0, sizeof(uint16_t));
                        break;
                    }
                    break;
                case 'd':
                    o.dword = va_arg(ap_copy, uint32_t *);
                    if (buffer->secure) {
                        memset(o.dword, 0, sizeof(uint32_t));
                        break;
                    }
                    break;
                case 'q':
                    o.qword = va_arg(ap_copy, uint64_t *);
                    if (buffer->secure) {
                        memset(o.qword, 0, sizeof(uint64_t));
                        break;
                    }
                    break;
                case 'B':
                    o.bignum = va_arg(ap_copy, bignum *);
                    bignum_safe_free(*o.bignum);
                    break;
                case 'S':
                    o.string = va_arg(ap_copy, ssh_string *);
                    if (buffer->secure) {
                        ssh_string_burn(*o.string);
                    }
                    SAFE_FREE(*o.string);
                    break;
                case 's':
                    o.cstring = va_arg(ap_copy, char **);
                    if (buffer->secure) {
                        memset(o.cstring, 0, strlen(*o.cstring));
                    }
                    SAFE_FREE(*o.cstring);
                    break;
                case 'P':
                    len = va_arg(ap_copy, size_t);
                    o.data = va_arg(ap_copy, void **);
                    if (buffer->secure) {
                        memset(o.data, 0, len);
                    }
                    SAFE_FREE(*o.data);
                    break;
                default:
                    (void) va_arg(ap_copy, void *);
                    break;
            }
        }
    }
    va_end(ap_copy);

    return rc;
}
*/

/**
 * @brief Get multiple values from a buffer on a single function call
 * @param[in] buffer    The buffer to get from
 * @param[in] format    A format string of arguments. This string contains single
 *                      letters describing the order and type of arguments:
 *                         'b': uint8_t *  (pulled in network byte order)
 *                         'w': uint16_t * (pulled in network byte order)
 *                         'd': uint32_t * (pulled in network byte order)
 *                         'q': uint64_t * (pulled in network byte order)
 *                         'S': ssh_string *
 *                         's': char ** (C string, pulled as SSH string)
 *                         'P': size_t, void ** (len of data, pointer to data)
 *                              only pulls data.
 *                         'B': bignum * (pulled as SSH string)
 * @param argc
 * @param ...
 * @returns             SSH_OK on success
 *                      SSH_ERROR on error
 * @warning             when using 'P' with a constant size (e.g. 8), do not forget to cast to (size_t).
 */
extern int _ssh_buffer_unpack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...);
/*int _ssh_buffer_unpack(ssh_buffer_struct *buffer, const char *format, size_t argc, ...) {
    va_list ap;

    va_start(ap, argc);
    const int rc = ssh_buffer_unpack_va(buffer, format, argc, ap);
    va_end(ap);
    return rc;
}*/

// ======================================================================================================================

static void set_default_keys(ssh_bind sshbind, const int rsa_already_set, const int ecdsa_already_set) {
    if (!rsa_already_set) {
        ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, KEYS_FOLDER "ssh_host_rsa_key");
    }
    if (!ecdsa_already_set) {
        ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, KEYS_FOLDER "ssh_host_ecdsa_key");
    }
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, KEYS_FOLDER "ssh_host_ed25519_key");
}

/**
 * An user data struct for channel.
 */
struct channel_data_struct {
    // Event which is used to poll the above descriptors.
    ssh_event event;
    sftp_session sftp;
};

/**
 * A user data struct for session.
 */
struct session_data_struct {
    // Pointer to the channel the session will allocate.
    ssh_channel channel;
    int auth_attempts;
    int authenticated;
};

static int auth_password(ssh_session session, const char *user, const char *pass, void *userdata) {
    auto *sdata = static_cast<struct session_data_struct *>(userdata);

    (void) session;
    const auto region = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.region");
    const auto userPoolId = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.userPoolId");

    if (AwsMock::Database::Entity::Cognito::User userEntity = AwsMock::Database::CognitoDatabase::instance().GetUserByUserName(region, userPoolId, user); userEntity.password == pass) {
        // const auto it = std::ranges::find_if(AwsMock::Service::SftpServer::_sftpUsers, [user, pass](const auto &sftpUser) {
        //     return sftpUser.userName == user && sftpUser.password == pass;
        // });
        //
        // if (it != AwsMock::Service::SftpServer::_sftpUsers.end()) {
        sdata->authenticated = 1;

        // Set current user
        currentUser = static_cast<char *>(malloc(128));
        strcpy(currentUser, userEntity.userName.c_str());

        // Set the user home directory
        const auto ftpBaseDir = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        userBasePath = static_cast<char *>(malloc(1024));
        strcpy(userBasePath, ftpBaseDir.c_str());
        strcpy(userBasePath + strlen(userBasePath), AwsMock::Core::FileUtils::separator().c_str());
        strcpy(userBasePath + strlen(userBasePath), currentUser);
        log_info << "SFTP user authenticated, userName: " << userEntity << ", homeDir: " << userBasePath;
        return SSH_AUTH_SUCCESS;
    }

    log_warning << "SFTP user not authenticated, userName: " << user << " password: " << pass;

    sdata->auth_attempts++;
    return SSH_AUTH_DENIED;
}

static int auth_publickey(const ssh_session session, const char *user, ssh_key_struct *pubkey, const char signature_state, void *userdata) {
    auto *sdata = static_cast<struct session_data_struct *>(userdata);

    (void) session;
    (void) user;

    if (signature_state == SSH_PUBLICKEY_STATE_NONE) {
        return SSH_AUTH_SUCCESS;
    }

    if (signature_state != SSH_PUBLICKEY_STATE_VALID) {
        return SSH_AUTH_DENIED;
    }

    // valid so far.  Now look through authorized keys for a match
    if (authorizedkeys[0]) {
        ssh_key key = nullptr;
        struct stat buf{};

        if (stat(authorizedkeys, &buf) == 0) {
            if (int result = ssh_pki_import_pubkey_file(authorizedkeys, &key); (result != SSH_OK) || (key == nullptr)) {
                log_error << "Unable to import public key file:" << authorizedkeys << std::endl;
            } else {
                result = ssh_key_cmp(key, pubkey, SSH_KEY_CMP_PUBLIC);
                ssh_key_free(key);
                if (result == 0) {
                    sdata->authenticated = 1;
                    return SSH_AUTH_SUCCESS;
                }
            }
        }
    }

    // no matches
    sdata->authenticated = 0;
    return SSH_AUTH_DENIED;
}

static ssh_channel channel_open(ssh_session session, void *userdata) {
    auto *sdata = static_cast<struct session_data_struct *>(userdata);

    sdata->channel = ssh_channel_new(session);
    return sdata->channel;
}

static void handle_session(const ssh_event &event, const ssh_session &session) {
    // Our struct holding information about the channel.
    channel_data_struct cdata = {.sftp = nullptr};

    // Our struct holding information about the session.
    session_data_struct sdata = {.channel = nullptr, .auth_attempts = 0, .authenticated = 0};

    ssh_channel_callbacks_struct channel_cb = {.userdata = &(cdata.sftp), .channel_data_function = sftp_channel_default_data_callback, .channel_subsystem_request_function = sftp_channel_default_subsystem_request};

    ssh_server_callbacks_struct server_cb = {.userdata = &sdata, .auth_password_function = auth_password, .channel_open_request_session_function = channel_open};

    if (authorizedkeys[0]) {
        server_cb.auth_pubkey_function = auth_publickey;
        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);
    } else
        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);

    ssh_callbacks_init(&server_cb);
    ssh_callbacks_init(&channel_cb);

    ssh_set_server_callbacks(session, &server_cb);

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        log_error << "Key exchanges files, error: " << ssh_get_error(session);
        return;
    }

    ssh_event_add_session(event, session);

    int n = 0;
    while (sdata.authenticated == 0 || sdata.channel == nullptr) {
        // If the user has used up all attempts, or if he hasn't been able to authenticate in 10 seconds (n * 100ms), disconnect.
        if (sdata.auth_attempts >= 3 || n >= 100) {
            return;
        }

        if (ssh_event_dopoll(event, 100) == SSH_ERROR) {
            log_error << "SFTP event poll failed, error:" << ssh_get_error(session);
            return;
        }
        n++;
    }

    ssh_set_channel_callbacks(sdata.channel, &channel_cb);

    do {
        // Poll the main event which takes care of the session, the channel and even our child process's stdout/stderr (once it's started).
        if (ssh_event_dopoll(event, -1) == SSH_ERROR) {
            log_error << "SFTP event poll failed, error:" << ssh_get_error(session);
            ssh_channel_close(sdata.channel);
        }

        // If the child process's stdout/stderr has been registered with the event,
        // or the child process hasn't started yet, continue.
        if (cdata.event != nullptr) {
            log_debug << "SFTP Event: " << cdata.event;
            continue;
        }

        // FIXME The server keeps hanging in the poll above when the client closes the channel
    } while (ssh_channel_is_open(sdata.channel));

    ssh_channel_send_eof(sdata.channel);
    ssh_channel_close(sdata.channel);

    // Wait up to 5 seconds for the client to terminate the session.
    for (n = 0; n < 50 && (ssh_get_status(session) & SESSION_END) == 0; n++) {
        ssh_event_dopoll(event, 100);
    }
}
}// extern C

namespace AwsMock::Service {
    void SftpServer::AddUser(const std::string &userName, const std::string &password, const std::string &homeDirectory) {
        SftpUser user = {.userName = userName, .password = password, .homeDirectory = homeDirectory};
        _sftpUsers.emplace_back(user);
    }

    void SftpServer::operator()(const std::string &port, const std::string &hostKey, const std::string &address, const std::string &serverId) const {
        ssh_bind sshbind = nullptr;
        ssh_session session = nullptr;
        ssh_event event = nullptr;
        currentServerId = serverId.c_str();

        // Change working directory
        const auto ftpBaseDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
#ifdef _WIN32
        int rc = _chdir(ftpBaseDir.c_str());
#else
        int rc = chdir(ftpBaseDir.c_str());
#endif
        if (rc < 0) {
            log_error << "Could not change to base path, basPath:" << ftpBaseDir;
            return;
        }
        if (rc = ssh_init(); rc < 0) {
            log_error << "SSH initialization failed";
            return;
        }

        sshbind = ssh_bind_new();
        if (sshbind == nullptr) {
            log_error << "SSH bind new failed";
            return;
        }
        log_info << "SFTP server starting, endpoint: " << address.c_str() << ":" << port.c_str() << ", hostKey: " << hostKey;

        // Command line options
        ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, port.c_str());
        ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, hostKey.c_str());
        ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, address.c_str());
        set_default_keys(sshbind, 0, 0);

        if (ssh_bind_listen(sshbind) < 0) {
            log_error << "SSH listen failed, error: " << ssh_get_error(sshbind);
            return;
        }

        while (true) {
            session = ssh_new();
            if (session == nullptr) {
                log_error << "SSH allocate session failed";
                continue;
            }

            // Blocks until there is a new incoming connection.
            if (ssh_bind_accept(sshbind, session) != SSH_ERROR) {
                event = ssh_event_new();
                if (event != nullptr) {
                    // Blocks until the SSH session ends by either child process exiting, or client disconnecting.
                    handle_session(event, session);
                    ssh_event_free(event);
                } else {
                    log_error << "Could not create polling context";
                }
            } else {
                log_error << "Could not bind, error: %s", ssh_get_error(sshbind);
            }

            // Since the session has been passed to a child fork, do some cleaning up at the parent process.
            ssh_disconnect(session);
            ssh_free(session);

#ifdef _WIN32
            // Wait for Windows server stop event
            if (WaitForSingleObject(g_ServiceStopEvent, 0) == WAIT_OBJECT_0) {
                break;
            }
#endif
        }
        ssh_bind_free(sshbind);
        ssh_finalize();
    }
}// namespace AwsMock::Service
