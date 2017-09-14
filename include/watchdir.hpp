#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <string>
#include <functional>
#include <map>

#include <thread>

#include "flags.hpp"

namespace Watch {

    // For more information, see `man inotify`
    using Event = inotify_event;

    // Callbacks attached to Events are fired and given the Event as an
    // argument.
    using Callback = std::function<void(Event*)>;

    // For more information, see man or info pages for inotify
    class Dog {
        public:
            // max_events and max_len_name used to determine internal buffer
            // length
            Dog(const std::string &dirname,
                const std::size_t max_events = 1024,
                const std::size_t max_len_name = 255);
            Dog(const Dog &src);
            ~Dog();

            void set_callback(Callback cb, FlagBearer flags);

            // File in the directory was read (ie read, execve)
            void set_on_access_handler(Callback cb);

            // Metadata changed (ie permissions, etc)
            void set_on_attrib_handler(Callback cb);

            // When a file in the directory opened for writing is closed
            void set_on_close_write_handler(Callback bc);

            // When a file or directory opened not for writing is closed
            void set_on_close_nowrite_handler(Callback cb);

            // On creation of a file or directory
            void set_on_create_handler(Callback cb);

            // On deletion of a file or directory inside the watched directory
            void set_on_delete_sub_handler(Callback cb);

            // On deletion of the directory itself
            void set_on_delete_handler(Callback cb);

            // On modification of the directory or its contents
            void set_on_modify_handler(Callback cb);

            // When the watched directory is moved
            void set_on_move_handler(Callback cb);

            // When subdirectories are moved
            void set_on_moved_from_handler(Callback cb);
            void set_on_moved_to_handler(Callback cb);

            // Don't allow assignment
            Dog& operator=(const Dog &src) = delete;

        private:
            std::map<FlagBearer, Callback> _callbacks;

    };

} // namespace Watch

