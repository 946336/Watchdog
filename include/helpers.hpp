#ifndef WATCHDOG_HELPERS_H
#define WATCHDOG_HELPERS_H

// ftw
// From `man nftw`: By default, directories are handled before the files and
// subdirectories they contain (preorder traversal). However, this does not
// seem to be true.
#include <ftw.h>

#include <vector>
#include <string>

#ifndef WATCHDOG_THREADSAFE
#define WATCHDOG_THREADSAFE false
#endif

#if WATCHDOG_THREADSAFE
#include <mutex>
#endif

#if WATCHDOG_THREADSAFE
#include <stdio.h>
#endif

#include <exceptions.hpp>

namespace {
#if WATCHDOG_THREADSAFE
    std::mutex path_m;
#endif
    std::vector<std::string> static_paths;

    void add_path(const std::string fpath)
    {
#if WATCHDOG_THREADSAFE
        std::lock_guard<std::mutex> lock(path_m);
#endif
        static_paths.push_back(fpath);
    }

    enum class FTW : int {
        Continue = 0,
        Stop = 1,
    };


    int ftw_callback(const char *fpath,
                     const struct stat *sb,
                     int typeflag)
    {
        (void) sb;

        // Do not record anything that isn't a directory
        if (typeflag == FTW_F) return (int) FTW::Continue;
        if (typeflag != FTW_D) return (int) FTW::Continue;

#if WATCHDOG_DEBUG
        fprintf(stderr, "[DEBUG/FTW]: Found directory %s\n", fpath);
#endif

        add_path(fpath);

        return (int) FTW::Continue;
    }


}

namespace Watch {

    const char path_sep = '/';

    std::string join_paths(const std::string &lhs, const std::string &rhs)
    {
        if (rhs.empty()) return lhs;
        if (lhs.empty()) return rhs;

        std::string joined = lhs;
        if (lhs.back() != path_sep) {
            joined += path_sep;
        }
        joined += rhs;

        return joined;
    }

    std::vector<std::string>
    enumerateSubdirectories(const std::string &path,
                            const int max_open_fd = 256)
    {
#if WATCHDOG_THREADSAFE
        // Only allow one fs crawl at a time :|
        static std::mutex enumeration_lock;
        std::lock_guard<std::mutex> master_lock(enumeration_lock);
#endif

        { // Scope the lock_guard
#if WATCHDOG_THREADSAFE
            std::lock_guard<std::mutex> lock(path_m);
#endif
            static_paths.clear();
        }

        ftw(path.c_str(), ftw_callback, max_open_fd);

        return std::vector<std::string>(static_paths);
    }

} // Watch

#endif

