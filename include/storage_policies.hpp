#ifndef WATCHDOG_PATHS_H
#define WATCHDOG_PATHS_H

#include <string>

#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include <watchdog_common.hpp>

namespace Watch {

    // Abstract class?
    class Storage_Policy {
        public:
            virtual void add(int key, std::string value) = 0;
            virtual std::string find(int key) = 0;
            /* virtual void append(const Storage_Policy &s) = 0; */
    };

    class Small : public Storage_Policy {
        public:
            void add(int key, std::string value) override
            {
                back_.push_back(std::make_pair(key, value));
            }

            std::string find(int key) override
            {
                auto it = std::find_if(back_.begin(), back_.end(),
                        [key](const Watch_Path &wp) {
                            return wp.first == key;
                        });

                return it->second;
            }

            void append(const Small &s)
            {
                back_.insert(back_.end(), s.back_.begin(), s.back_.end());
            }

            using storage_type = std::vector<Watch_Path>;

            const storage_type &backing()
            {
                return back_;
            }

        private:
            storage_type back_;
    };

    class Large : public Storage_Policy {
        public:
            void add(int key, std::string value) override
            {
                back_.insert(std::make_pair(key, value));
            }

            std::string find(int key) override
            {
                return back_[key];
            }

            void append(const Large &s)
            {
                back_.insert(s.back_.begin(), s.back_.end());
            }

            using storage_type = std::map<Watch_Path::first_type,
                                          Watch_Path::second_type>;

            const storage_type &backing()
            {
                return back_;
            }

        private:
            storage_type back_;
    };

} // Watch

#endif

