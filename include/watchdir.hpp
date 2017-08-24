#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <string>

namespace Watch {

    using Event = inotify_event;

    namespace Events {
        // Use these to describe the types of events you wish to watch for
        // See `inotify_add_watch()`
        enum class Flags : std::size_t {
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

        // Check for these in the `mask` field set by `read()`
        enum class Reply : std::size_t {
            // Watch was removed for one reason or another
            Ignored = IN_IGNORED,
            // Subject of event is a directory
            Is_Directory = IN_ISDIR,
            // Event queue overflowed (and wd is -1)
            Overflow = IN_Q_OVERFLOW,
            // Filesystem was unmounted
            Unmounted = IN_UNMOUNT,
        };
    } // namespace Events

    namespace Watch {
        // Use these to further refine the behavior of the watch created by
        // `inotify_add_watch()`
        enum class Flags : std::size_t {
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
    } // namespace Watch

    // For more information, see man or info pages for inotify
    class Directory {
        public:
            Directory(const std::string &dirname,
                      const std::size_t max_events = 1024,
                      const std::size_t max_len_name = 255);
            Directory(const Directory &src);
            ~Directory();

            // File in the directory was read (ie read, execve)
            void set_on_access_handler();

            // Metadata changed (ie permissions, etc)
            void set_on_attrib_handler();

            // When a file in the directory opened for writing is closed
            void set_on_close_write_handler();

            // When a file or directory opened not for writing is closed
            void set_on_close_nowrite_handler();

            // On creation of a file or directory
            void set_on_create_handler();

            // On deletion of a file or directory inside the watched directory
            void set_on_delete_sub_handler();

            // On deletion of the directory itself
            void set_on_delete_handler();

            // On modification of the directory or its contents
            void set_on_modify_handler();

            // When the watched directory is moved
            void set_on_move_handler();

            // When subdirectories are moved
            void set_on_moved_from_handler();
            void set_on_moved_to_handler();

            // Don't allow assignment
            Directory& operator=(const Directory &src) = delete;

        protected:

    };

    class File {

    };

} // namespace Watch

