#pragma once

#include <utility>
#include "error.hpp"

namespace Util {

    enum class ResultStats {

        Ok = 1,
        Err,

    }; // enum class ResultOk

    template<class T>
    class Result {

        private:
            ResultStats stats;
            T ok;
            Error err;

        public:
            Result(T val) {
                this->stats = ResultStats::Ok;
                this->ok = val;
            }
            Result(Error err) {
                this->stats = ResultStats::Err;
                this->err = err;
            }

            ResultStats get_stats() {
                return this->stats;
            }
            T get_ok() {
                return this->ok;
            }
            Error get_err() {
                return this->err;
            }

    }; // class Result

} // namespace Util
