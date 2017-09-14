#ifndef WATCHDOG_COMMON_H
#define WATCHDOG_COMMON_H

#include <utility>
#include <string>

namespace Watch {

    using Watch_Path = std::pair<int, std::string>;
    using FlagBearer = std::size_t;

} // namespace Watch

#endif

