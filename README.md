# Watchdog

A simple, somewhat impoverished filesystem watcher built on top of GNU/Linux's
`inotify` API.

## Goals

Provide a few interfaces to common functionality. For example, we may provide
both a policy based filesystem watcher and a more classic set of somewhat
specialized filesystem watchers.

For example, the following blocks are equivalent:

    Watch::Dog<Watch::Dog::Directory> directory("src/");
    Watch::Dog<Watch::Dog::File> file("src/playground.cpp");

    Watch::Directory directory("src/");
    Watch::File file("src/playground.cpp");

## Developer Notes

* Test for correctness, completeness, and feature-parity between
  implementations. If at all possible, share implementations and provide
  multiple interfaces instead.
* This should also be thread-safe, or at least have the option of being thread
  safe.
* The policy-based implementation must be header-only.
* Consider using a switch somewhere to allow the shorter, non-template
  versions to be used as either header-only _or_ as a precompiled library.
    * This switch must be project-wide, or weird things happen.
* Look into providing built-in backgrounding capabilities via threads,
  pseudo non-blocking via polling/kqueue, and/or integration with something
  like ZeroMQ.

## Roadmap

1. Separate common constants into separate header file
2. Figure out a model for thread-safety.
3. Determine whether using a base class would make sense. If so, separate
   common elements into a base class. If not, create two separate classes.
4. Stub out classes to get signatures.
5. Begin writing tests for `inotify` bits.
6. Begin implementing towards tests for `inotify` bits.
7. Wrap implementation into all other provided interfaces.
8. Write those tests too, since they should be very simple ports.
9. Final tests, housekeeping, etc. for core functionality.
10. Begin work on QoL features like nonblocking watchers.

