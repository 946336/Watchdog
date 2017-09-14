#ifndef WATCHDOG_FLAGS_H
#define WATCHDOG_FLAGS_H

#include <sys/inotify.h>

#include <cstddef>
#include <string>

#include <vector>

#include <watchdog_common.hpp>

namespace Watch {

    // Use these to describe the types of events you wish to watch for
    // See `inotify_add_watch()`
    namespace On {
        enum : FlagBearer {
            All = IN_ALL_EVENTS,

            Access = IN_ACCESS,
            Attributes = IN_ATTRIB,

            Close_Write = IN_CLOSE_WRITE,
            Close_Nowrite = IN_CLOSE_NOWRITE,
            Close = IN_CLOSE, // Both of the above

            Create = IN_CREATE,
            Delete = IN_DELETE_SELF,
            Delete_Sub = IN_DELETE,
            Modify = IN_MODIFY,
            Move = IN_MOVE_SELF,

            Moved_From = IN_MOVED_FROM,
            Moved_To = IN_MOVED_TO,
            Moved = IN_MOVE, // Both of the above

            Open = IN_OPEN,
        };
    } // namespace On

    // Check for these in the `mask` field set by `read()`
    namespace Reply {
        enum : FlagBearer {
            // Watch was removed for one reason or another
            Ignored = IN_IGNORED,
            // Subject of event is a directory
            Is_Directory = IN_ISDIR,
            // Event queue overflowed (and wd is -1)
            Overflow = IN_Q_OVERFLOW,
            // Filesystem was unmounted
            Unmounted = IN_UNMOUNT,
        };
    } // namespace Reply

    // Use these to further refine the behavior of the watch created by
    // `inotify_add_watch()`
    namespace Flags {
        enum : FlagBearer {
            // Don't follow symbolic links
            No_Follow = IN_DONT_FOLLOW,
            // Stop following things that have been moved out of the directory
            Unlink = IN_EXCL_UNLINK,
            // Append to kernel watch list for this directory instead of
            // replacing
            Add = IN_MASK_ADD,
            // Remove directory from watch list after a single event
            Once = IN_ONESHOT,
            // Watch only if named filesystem object is a directory
            Directory_Only = IN_ONLYDIR,
        };
    } // namespace Flags

    namespace Flags {

        enum Names : FlagBearer {
            Events = 0x1,
            Replies = 0x2,
            Flags = 0x4,
            All = Events | Replies | Flags
        };

        std::vector<std::string> get_names(const FlagBearer flags,
                                           const FlagBearer _names)
        {
            std::vector<std::string> names;

            if (_names & Names::Events) {
                // Filesystem events
                if (flags & On::Access) {
                    names.push_back("Access");
                } if (flags & On::Attributes) {
                    names.push_back("Attributes");
                } if (flags & On::Close_Write) {
                    names.push_back("Close_Write");
                } if (flags & On::Close_Nowrite) {
                    names.push_back("Close_Nowrite");
                } if (flags & On::Create) {
                    names.push_back("Create");
                } if (flags & On::Delete) {
                    names.push_back("Delete");
                } if (flags & On::Delete_Sub) {
                    names.push_back("Delete_Sub");
                } if (flags & On::Modify) {
                    names.push_back("Modify");
                } if (flags & On::Move) {
                    names.push_back("Move");
                } if (flags & On::Moved_From) {
                    names.push_back("Moved_From");
                } if (flags & On::Moved_To) {
                    names.push_back("Moved_To");
                } if (flags & On::Open) {
                    names.push_back("Open");
                }
            }

            if (_names & Names::Replies) {
                // On read
                if (flags & Reply::Ignored) {
                    names.push_back("Ignored");
                } if (flags & Reply::Is_Directory) {
                    names.push_back("Is_Directory");
                } if (flags & Reply::Overflow) {
                    names.push_back("Overflow");
                } if (flags & Reply::Unmounted) {
                    names.push_back("Unmounted");
                }
            }

            if (_names & Names::Flags) {
                // Other flags
                if (flags & Flags::No_Follow) {
                    names.push_back("No_Follow");
                } if (flags & Flags::Unlink) {
                    names.push_back("Unlink");
                } if (flags & Flags::Add) {
                    names.push_back("Add");
                } if (flags & Flags::Once) {
                    names.push_back("Once");
                } if (flags & Flags::Directory_Only) {
                    names.push_back("Directory_Only");
                }
            }

            return names;
        }
    } // namespace Flags

} // namespace Watch


#endif

