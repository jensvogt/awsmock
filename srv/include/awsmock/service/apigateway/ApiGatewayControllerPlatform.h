#pragma once

// ── POSIX (Linux + macOS) ─────────────────────────────────
#if defined(__linux__) || defined(__APPLE__)
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <unistd.h>
  #include <csignal>
  #include <fcntl.h>
  #include <cerrno>
  #include <cstring>

  #ifdef __linux__
    #include <sys/mman.h>    // memfd_create, sendfile
    #include <sys/sendfile.h>
  #endif

// ── Windows ───────────────────────────────────────────────
#elif defined(_WIN32)
  #include <windows.h>
  #include <process.h>
  // waitpid equivalent on Windows:
  // WaitForSingleObject(handle, timeoutMs)
  // GetExitCodeProcess(handle, &code)
#endif

// ── Always ────────────────────────────────────────────────
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>