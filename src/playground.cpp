#include <cstdio>
#include <cstdlib>
#include <climits>

// C headers (Whee)
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

// Maximum number of events to process at once
#define MAX_EVENTS 1024

// Upper limit on filename length
#define LEN_NAME 255

// Size of event object
#define EVENT_SIZE (sizeof(inotify_event))

// Buufer size for storing event data
#define BUFFER_LENGTH (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    int length;
    int i = 0;
    int wd; // ???

    int fd;

    char buffer[BUFFER_LENGTH];

    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return 0;
    }

    wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd == -1) {
        perror("inotify_add_watch");
        return 0;
    } else {
        fprintf(stderr, "Watching: [%s]\n", argv[1]);
    }

    inotify_event *ev = nullptr;
    /* while(true) { */
    for (int nEvents = 0; nEvents < 5; ++nEvents) {
        i = 0;
        length = read(fd, buffer, BUFFER_LENGTH);
        if (length < 0) {
            perror("read");
            return 0;
        }

        for (; i < length; i += EVENT_SIZE + ev->len) {
            ev = (inotify_event*) &buffer[i];
            if (ev->len == 0) continue;
            // On create
            if (ev->mask & IN_CREATE) {
                if (ev->mask & IN_ISDIR) {
                    printf("Created directory %s\n", ev->name);
                } else {
                    printf("Created file %s (%d)\n", ev->name, wd);
                }
            }
            // On modify
            if (ev->mask & IN_MODIFY) {
                if (ev->mask & IN_ISDIR) {
                    printf("Modified directory %s", ev->name);
                } else {
                    printf("Modified file %s (%d)\n", ev->name, wd);
                }
            }
            // On delete
            if (ev->mask & IN_DELETE) {
                if (ev->mask & IN_ISDIR) {
                    printf("Deleted directory %s", ev->name);
                } else {
                    printf("Deleted file %s (%d)\n", ev->name, wd);
                }
            }
        }
    }

    // Cleanup, but we can't ever get here if we use the while loop
    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}

