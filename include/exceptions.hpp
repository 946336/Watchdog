#ifndef WATCHDOG_EXCEPTIONS_H
#define WATCHDOG_EXCEPTIONS_H

#include <stdexcept>

namespace Watch {

    class Exception : public std::runtime_error {
        public:
            Exception(const char *what) : std::runtime_error(what) {}
            Exception(const std::string &what) : std::runtime_error(what) {}
    };

} // namespace Watch

#endif

