#pragma once

#include <iostream>

namespace Util {

    enum class ErrorLevel {

        Warning = 1, // just alert
        Error,   // raise panic

    }; // enum class ErrorLevel

    class Error {

        private:
            ErrorLevel level;
            const char *perror;
            const char *msg;

        public:
            Error() { };
            Error(ErrorLevel level, const char *perror, const char *msg) {
                this->level = level;
                this->perror = perror;
                this->msg = msg;
            }

            void report() {
                switch (this->level) {
                    case ErrorLevel::Warning:
                        std::cout << std::string("[WARNING] ") << this->perror << std::string(" @ ") << this->msg << std::endl;
                        break;

                    case ErrorLevel::Error:
                        std::cout << std::string("[ERROR] ") << this->perror << std::string(" @ ") << this->msg << std::endl;
                        break;
                }
            }

    }; // class Error

} // namespace Util
