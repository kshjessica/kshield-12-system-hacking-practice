#pragma once

#include <utility>
#include "error.hpp"

namespace Util {

    enum class OptionStats {

        None,
        Some,

    }; // enum class OptionStats

    template<class T>
    class Option {

        private:
            OptionStats stats;
            T value;

        public:
            Option() {
                this->stats = OptionStats::None;
            }
            Option(T val) {
                this->stats = OptionStats::Some;
                this->value = val;
            }

            OptionStats get_stats() {
                return this->stats;
            }
            T get_value() {
                return this->value;
            }
            bool set_value(T val) {
                this->stats = OptionStats::Some;
                this->value = val;
                return true;
            }

    }; // class Option

} // namespace Util
