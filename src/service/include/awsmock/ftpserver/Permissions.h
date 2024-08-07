#pragma once

namespace AwsMock::FtpServer {

    enum class Permission : int {
        FileRead = (1 << 0),   /**< Download files */
        FileWrite = (1 << 1),  /**< Upload files as new files */
        FileAppend = (1 << 2), /**< Upload files that get appended to existing files */
        FileDelete = (1 << 3), /**< Delete existing files or overwrite an existing file */
        FileRename = (1 << 4), /**< Rename existing files */

        DirList = (1 << 5),   /**< Retrieve the content of directories */
        DirCreate = (1 << 6), /**< Create new directories */
        DirDelete = (1 << 7), /**< Delete existing directories */
        DirRename = (1 << 8), /**< Rename existing directories */

        All = (FileRead | FileWrite | FileAppend | FileDelete | FileRename | DirList | DirCreate | DirDelete | DirRename),
        ReadOnly = (FileRead | DirList),
        None = 0
    };

    inline Permission operator~(Permission a) { return static_cast<Permission>(~static_cast<int>(a)); }
    inline Permission operator|(Permission a, Permission b) { return static_cast<Permission>(static_cast<int>(a) | static_cast<int>(b)); }
    inline Permission operator&(Permission a, Permission b) { return static_cast<Permission>(static_cast<int>(a) & static_cast<int>(b)); }
    inline Permission operator^(Permission a, Permission b) { return static_cast<Permission>(static_cast<int>(a) ^ static_cast<int>(b)); }
    inline Permission &operator|=(Permission &a, Permission b) { return reinterpret_cast<Permission &>(reinterpret_cast<int &>(a) |= static_cast<int>(b)); }
    inline Permission &operator&=(Permission &a, Permission b) { return reinterpret_cast<Permission &>(reinterpret_cast<int &>(a) &= static_cast<int>(b)); }
    inline Permission &operator^=(Permission &a, Permission b) { return reinterpret_cast<Permission &>(reinterpret_cast<int &>(a) ^= static_cast<int>(b)); }

}// namespace AwsMock::FtpServer
