#ifndef WATCHDOG_DEBUG
#define WATCHDOG_DEBUG true
#endif
#include <watchdog.hpp>
#include <iostream>

void usage(const std::string &invokedAs)
{
    std::cerr << "Usage: " << invokedAs << " [path]\n";
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string path(argv[1]);

    /* Watch::Dog watcher(path); // Watch nonrecursively */
    Watch::Pen watcher(path); // Watch recursively

    std::cout << "Watching " << path << std::endl;

    // Examine all filesystem events supported by inotify
    watcher.add_callback([](Watch::Event *ev, std::string path) {
            auto names = Watch::Flags::get_names(ev->mask,
                    Watch::Flags::Names::All);
            printf("From %s:\n", Watch::join_paths(path,
                        std::string(ev->name, ev->len)).c_str());
            for (const auto &name : names) {
                std::cout << name << std::endl;
            }
        }, Watch::On::All);

    watcher.listen();

    return 0;
}

