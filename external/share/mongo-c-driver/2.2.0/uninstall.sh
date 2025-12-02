#!/usr/bin/env bash
#
# MongoDB C Driver uninstall program, generated with CMake
#
# Copyright 2009-present MongoDB, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")
#
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -eu

__rmfile() {
    set -eu
    abs=$__prefix/$1
    printf "Remove file %s: " "$abs"
    if test -f "$abs" || test -L "$abs"
    then
        rm -- "$abs"
        echo "ok"
    else
        echo "skipped: not present"
    fi
}

__rmdir() {
    set -eu
    abs=$__prefix/$1
    printf "Remove directory %s: " "$abs"
    if test -d "$abs"
    then
        list="$(find "$abs" -mindepth 1)"
        if test "$list" = ""
        then
            rmdir -- "$abs" 2>/dev/null && echo "ok" || echo "skipped: not empty"
        fi
    else
        echo "skipped: not present"
    fi
}

__prefix=${DESTDIR:-}/home/vogje01/work/awsmock/external

__rmfile "lib/libbson2.a"
__rmfile "lib/cmake/bson-2.2.0/bson_static-targets.cmake"
__rmfile "lib/cmake/bson-2.2.0/bson_static-targets-relwithdebinfo.cmake"
__rmfile "lib/pkgconfig/bson2-static.pc"
__rmfile "lib/libbson2.so.2.2.0"
__rmfile "lib/libbson2.so.2"
__rmfile "lib/libbson2.so"
__rmfile "lib/cmake/bson-2.2.0/bson_shared-targets.cmake"
__rmfile "lib/cmake/bson-2.2.0/bson_shared-targets-relwithdebinfo.cmake"
__rmfile "lib/pkgconfig/bson2.pc"
__rmfile "include/bson-2.2.0/bson/bson-version-functions.h"
__rmfile "include/bson-2.2.0/bson/bson-bcon.h"
__rmfile "include/bson-2.2.0/bson/bson-vector.h"
__rmfile "include/bson-2.2.0/bson/bson-decimal128.h"
__rmfile "include/bson-2.2.0/bson/bson-json.h"
__rmfile "include/bson-2.2.0/bson/bson-context.h"
__rmfile "include/bson-2.2.0/bson/bson-clock.h"
__rmfile "include/bson-2.2.0/bson/bson-types.h"
__rmfile "include/bson-2.2.0/bson/bson-value.h"
__rmfile "include/bson-2.2.0/bson/bson-string.h"
__rmfile "include/bson-2.2.0/bson/bson-keys.h"
__rmfile "include/bson-2.2.0/bson/bson-reader.h"
__rmfile "include/bson-2.2.0/bson/config.h"
__rmfile "include/bson-2.2.0/bson/error.h"
__rmfile "include/bson-2.2.0/bson/macros.h"
__rmfile "include/bson-2.2.0/bson/version.h"
__rmfile "include/bson-2.2.0/bson/bson-endian.h"
__rmfile "include/bson-2.2.0/bson/memory.h"
__rmfile "include/bson-2.2.0/bson/bson-prelude.h"
__rmfile "include/bson-2.2.0/bson/bson-utf8.h"
__rmfile "include/bson-2.2.0/bson/bson_t.h"
__rmfile "include/bson-2.2.0/bson/bson-writer.h"
__rmfile "include/bson-2.2.0/bson/bson-iter.h"
__rmfile "include/bson-2.2.0/bson/compat.h"
__rmfile "include/bson-2.2.0/bson/bson.h"
__rmfile "include/bson-2.2.0/bson/bson-oid.h"
__rmfile "include/bson-2.2.0/bson/bson-version-functions.h"
__rmfile "include/bson-2.2.0/bson/bson-bcon.h"
__rmfile "include/bson-2.2.0/bson/bson-vector.h"
__rmfile "include/bson-2.2.0/bson/bson-decimal128.h"
__rmfile "include/bson-2.2.0/bson/bson-json.h"
__rmfile "include/bson-2.2.0/bson/bson-context.h"
__rmfile "include/bson-2.2.0/bson/bson-clock.h"
__rmfile "include/bson-2.2.0/bson/bson-types.h"
__rmfile "include/bson-2.2.0/bson/bson-value.h"
__rmfile "include/bson-2.2.0/bson/bson-string.h"
__rmfile "include/bson-2.2.0/bson/bson-keys.h"
__rmfile "include/bson-2.2.0/bson/bson-reader.h"
__rmfile "include/bson-2.2.0/bson/config.h"
__rmfile "include/bson-2.2.0/bson/error.h"
__rmfile "include/bson-2.2.0/bson/macros.h"
__rmfile "include/bson-2.2.0/bson/version.h"
__rmfile "include/bson-2.2.0/bson/bson-endian.h"
__rmfile "include/bson-2.2.0/bson/memory.h"
__rmfile "include/bson-2.2.0/bson/bson-prelude.h"
__rmfile "include/bson-2.2.0/bson/bson-utf8.h"
__rmfile "include/bson-2.2.0/bson/bson_t.h"
__rmfile "include/bson-2.2.0/bson/bson-writer.h"
__rmfile "include/bson-2.2.0/bson/bson-iter.h"
__rmfile "include/bson-2.2.0/bson/compat.h"
__rmfile "include/bson-2.2.0/bson/bson.h"
__rmfile "include/bson-2.2.0/bson/bson-oid.h"
__rmfile "lib/cmake/bson-2.2.0/00-mongo-platform-targets.cmake"
__rmfile "lib/cmake/bson-2.2.0/bsonConfig.cmake"
__rmfile "lib/cmake/bson-2.2.0/bsonConfigVersion.cmake"
__rmfile "lib/pkgconfig/mongoc2.pc"
__rmfile "bin/mongoc2-stat"
__rmfile "lib/pkgconfig/mongoc2-static.pc"
__rmfile "lib/libmongoc2.a"
__rmfile "lib/libmongoc2.so.2.2.0"
__rmfile "lib/libmongoc2.so.2"
__rmfile "lib/libmongoc2.so"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-config.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-version.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-apm.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-bulk-operation.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-bulkwrite.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-change-stream.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-client.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-client-pool.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-client-side-encryption.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-collection.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-cursor.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-database.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-error.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-flags.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-find-and-modify.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-gridfs.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-gridfs-bucket.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-gridfs-file.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-gridfs-file-page.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-gridfs-file-list.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-handshake.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-host-list.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-init.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-iovec.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-log.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-macros.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-oidc-callback.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-opcode.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-optional.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-prelude.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-read-concern.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-read-prefs.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-server-api.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-server-description.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-client-session.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-sleep.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-socket.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-tls-openssl.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-buffered.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-file.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-gridfs.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-socket.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-structured-log.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-topology-description.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-uri.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-version-functions.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-write-concern.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-rand.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-stream-tls.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-ssl.h"
__rmfile "include/mongoc-2.2.0/mongoc/mongoc-bulkwrite.h"
__rmfile "lib/cmake/mongoc-2.2.0/mongoc-targets.cmake"
__rmfile "lib/cmake/mongoc-2.2.0/mongoc-targets-relwithdebinfo.cmake"
__rmfile "lib/cmake/mongoc-2.2.0/mongocConfig.cmake"
__rmfile "lib/cmake/mongoc-2.2.0/mongocConfigVersion.cmake"
__rmfile "share/mongo-c-driver/2.2.0/COPYING"
__rmfile "share/mongo-c-driver/2.2.0/NEWS"
__rmfile "share/mongo-c-driver/2.2.0/README.rst"
__rmfile "share/mongo-c-driver/2.2.0/THIRD_PARTY_NOTICES"
__rmfile "share/mongo-c-driver/2.2.0/uninstall.sh"
__rmdir "share/mongo-c-driver/2.2.0"
__rmdir "share/mongo-c-driver"
__rmdir "lib/pkgconfig"
__rmdir "lib/cmake/mongoc-2.2.0"
__rmdir "lib/cmake/bson-2.2.0"
__rmdir "lib/cmake"
__rmdir "include/mongoc-2.2.0/mongoc"
__rmdir "include/mongoc-2.2.0"
__rmdir "include/bson-2.2.0/bson"
__rmdir "include/bson-2.2.0"
