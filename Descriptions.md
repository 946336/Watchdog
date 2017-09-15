# Usage

Watchdog provides a number of aliases and enumerations within the `Watch`
namespace. Of the most interest to most users are the flags provided in the
namespaces `On`, `Reply`, and `Flags`.

Watchdog provides you with the `Watch::Sentry` class template and two
convenience aliases: `Watch::Dog` and `Watch::Pen`.

## Aliases and Enumerations

```c++
enum Recurse {
    Normally,
    Recursively
};
```

```c++
using Event = inotify_event;
using Callback = std::function<void(Event*, std::string path)>;
using Watch_Path = std::pair<int, std::string>;
using FlagBearer = std::size_t;

using Dog = Sentry<Normally>;
using Pen = Sentry<Recursively>;
```

## Preprocessor Directives and Definitions

Watchdog does not require, but will respond to the following preprocessor
directives and definitions:

* `WATCHDOG_DEBUG`: Defining this as `true` will cause Watchdog to emit
  debugging messages. This defaults to `false`.
* `WATCHDOG_MAX_EVENTS`: This integer determines the number of events that
  Watchdog should be prepared to buffer. Defaults to 1024.
* `WATCHDOG_MAX_LEN_NAME`: This integer determines the maximum filename length
  that Watchdog should be prepared to handle. Defaults to 255.
* `WATCHDO_THREADSAFE`: This causes Watchdog to protect its shared memory from
  data races. Defaults to `false`.

## Flags

The flags provided in the namespaces `On`, `Reply`, and `Flags` correspond to
the flags described in `man inotify`. In general, the flags in `Watch::On`
correspond to filesystem events that tend to be interesting.

Flags in `Watch::Reply` are set in the `Event*` provided to callbacks, and
flags in `Flags` are additional flags that do not correspond to filesystem
events but that are still otherwise useful.

## Sentry

`Watch::Sentry` is a class template with the following template parameters:

Required template parameters:

* `Recurse RECURSE`: Used to determine if Watchdog should attempt to watch
  recursively. It is _not_ and error to recursively watch a file, though it is
  somewhat pointless.

Optional template parameters (in order):

* `FlagBearer Default\_Flags`: Used to provide default flags when registering
  callbacks. Defaults to `Flags::Add`, so that multiple calls to
  `Sentry::add_callback` will properly add watches instead of replacing them.
* `class Container`: Used as the backing storage for watch descriptors and the
  paths associated with them. Watchdog provides two (`Small` and `Large`) and
  defaults to `Small`. If you are recursively watching a large directory tree,
  consider switching to `Large`.
* `std::size_t MAX_EVENTS`: Determines how many events Watchdog will prepare
  to buffer for. Defaults to `WATCHDOG_MAX_EVENTS`.
* `std::size_t MAX_LEN_NAME`: Determines how long of a filename Watchdog
  should be prepared to receive events for (in bytes). Defaults to
  `WATCHDOG_MAX_LEN_NAME`.

`Sentry` exposes the following methods:

* Constructor: Requires a path to watch and an optional list
  (`std::vector<std::string>`) of paths to ignore. These must match exactly,
  as regular expressions, globbing, and partial matches are not supported.
* `add_callback`: Registers a `Callback` to be called whenever an `Event` with
  a matching flag is detected by Watchdog.
* `listen`: `Sentry` enters a neverending loop, waiting for `Event`s.

