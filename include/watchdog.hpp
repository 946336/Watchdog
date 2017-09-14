#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

// inotify
#include <sys/inotify.h>

#if WATCHDOG_DEBUG
#include <cstdio>
#endif

#include <functional>
#include <utility>
#include <algorithm>

#include <vector>
#include <map>
#include <set>

#include <flags.hpp>
#include <exceptions.hpp>
#include <helpers.hpp>
#include <watchdog_common.hpp>
#include <storage_policies.hpp>

// If you want to override the defaults, this will allow you to do so by
// defining these names before you include this header.

#ifndef WATCHDOG_MAX_EVENTS
// Maximum number of events the buffer can hold at once
#define WATCHDOG_MAX_EVENTS 1024
#endif

#ifndef WATCHDOG_MAX_LEN_NAME
// Maximum length of paths to watch
#define WATCHDOG_MAX_LEN_NAME 255
#endif

#ifndef WATCHDOG_DEBUG
#define WATCHDOG_DEBUG false
#endif

#if WATCHDOG_DEBUG
#include <cstdio>
#endif

namespace Watch {

    // For more information, see `man inotify`
    using Event = inotify_event;

    // Callbacks attached to Events are fired and given the Event as an
    // argument.
    using Callback = std::function<void(Event*, std::string path)>;

    // Do you want to watch directories recursively?
    enum Recurse {
        Normally,
        Recursively
    };

    // max_events and max_len_name used to determine internal buffer length
    // For more information, see man or info pages for inotify
    template < Recurse RECURSE,
               FlagBearer Default_Flags = Flags::Add,
               class Container = Small,
               std::size_t MAX_EVENTS = WATCHDOG_MAX_EVENTS,
               std::size_t MAX_LEN_NAME = WATCHDOG_MAX_LEN_NAME >
    class Sentry {
        public:
            // Don't allow assignment or copying
            Sentry(const Sentry &src) = delete;
            Sentry& operator=(const Sentry &src) = delete;

            // Path is required, but the ignore list is optional
            Sentry(const std::string &path,
                   const std::set<std::string> ignore = {})
                : paths_(1, path), ignored_(ignore)
            {
                if (WATCHDOG_DEBUG) {
                    fprintf(stderr, "[DEBUG]: Setting up to watch %s %s\n",
                            path.c_str(),
                            (RECURSE == Normally)
                                ? "normally" : "recursively");
                }

                fd = inotify_init();
                if (fd < 0) {
                    throw Exception("Failed to initiate watch");
                }

                if (RECURSE == Watch::Recursively) {
                    for (const auto &path_ : enumerateSubdirectories(path)) {
                        if (ignored_.find(path_) == ignored_.end()) {
                            paths_.push_back(path_);
                        } else if (WATCHDOG_DEBUG) {
                            fprintf(stderr, "[DEBUG]: Ignoring %s\n",
                                    path_.c_str());
                        }
                    }
                }

                if (WATCHDOG_DEBUG) {
                    for (const auto &path : paths_) {
                        fprintf(stderr, "[DEBUG]: Watching %s\n", path.c_str());
                    }
                }
            }

            ~Sentry()
            {
                if (WATCHDOG_DEBUG) {
                    fprintf(stderr, "[DEBUG]: Cleaning up watches for %s\n",
                            paths_[0].c_str());
                }

                for (const auto wd : wds.backing()) {
                    inotify_rm_watch(fd, wd.first);
                }
                close(fd);
            }

            void add_callback(Callback cb, FlagBearer flags)
            {
                if (WATCHDOG_DEBUG) {
                    fprintf(stderr, "[DEBUG]: Registering callback for %s\n",
                            paths_[0].c_str());
                }

                Container wds_;

                // Attempt to add all watches. While _this_ is in left in a
                // relatively consistent state if an exception is thrown, you
                // probably have other problems when that happens.
                for (const auto path : paths_) {
                    int wd = inotify_add_watch(fd, path.c_str(),
                            flags | Default_Flags);
                    if (wd < 0) {
                        throw Exception("Failed to add watch to " + path);
                    }

                    wds_.add(wd, path);
                }

                _callbacks.push_back(std::make_pair(flags, cb));

                wds.append(wds_);

            }

            void listen()
            {
                if (WATCHDOG_DEBUG) {
                    fprintf(stderr, "[DEBUG]: Forever listening for "
                            "changes to %s\n",
                            paths_[0].c_str());
                }

#if WATCHDOG_DEBUG
                while (true) listen_(1);
#else
                while (true) listen_(10000);
#endif
            }

        private:

            void listen_(std::size_t howManyTimes)
            {
                if (WATCHDOG_DEBUG) {
                    fprintf(stderr, "[DEBUG]: Listening for changes to %s\n",
                            paths_[0].c_str());
                }

                Event *ev = nullptr;
                int length = 0;

                for (std::size_t event_count = 0;
                        event_count < howManyTimes;
                        ++event_count) {

                    length = read(fd, buffer, buffer_length);
                    if (length < 0) {
                        throw Exception("Failed to read events");
                    }

                    if (WATCHDOG_DEBUG) {
                        fprintf(stderr, "[DEBUG]: Read %d bytes of "
                                "events\n", length);
                    }

                    // Process each event's callback
                    int i = 0;
                    for (; i < length; i += sizeof(Event) + ev->len) {
                        // Technically unsafe, I know
                        ev = (Event*) &buffer[i];
                        dispatch(ev);
                    }
                }
            }

            void dispatch(Event *ev)
            {
                // Call each callback that matches
                for (auto cbp : _callbacks) {
                    if (WATCHDOG_DEBUG) {
                        fprintf(stderr, "[DEBUG]: Mask: 0x%lx "
                                "Event Flags: 0x%x\n",
                                cbp.first, ev->mask);
                    }

                    // Ignore when mask doesn't match
                    if ((cbp.first & ev->mask) == 0) continue;
                    // Ignore Ignored events
                    if (ev->mask & Reply::Ignored) {
                        if (WATCHDOG_DEBUG) {
                            fprintf(stderr, "[DEBUG]: From %s -> ignored\n",
                                    ev->name);
                        }
                        continue;
                    }

                    // Explode when the queue does
                    if (ev->mask & Reply::Overflow) {
                        throw Exception("Inotify queue overflowed");
                    }

                    cbp.second(ev, wds.find(ev->wd));
                }
            }

            std::vector< std::pair<FlagBearer, Callback> > _callbacks;
            std::vector< std::string > paths_; // Hmm...
            std::set< std::string > ignored_;

            // buffer_length is nonstatic
            char buffer[ MAX_EVENTS * ( sizeof(Event) + MAX_LEN_NAME ) ];
            std::size_t buffer_length = MAX_EVENTS
                    * ( sizeof(Event) + MAX_LEN_NAME );

            int fd; // File descriptor
            Container wds;

    };

    using Dog = Sentry<Watch::Normally>;
    using Pen = Sentry<Watch::Recursively>;

} // namespace Watch

#endif

