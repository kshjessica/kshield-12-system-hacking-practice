#pragma once

#include <vector>

namespace Util {

    template<typename T>
    static T pop_front(std::vector<T> &v) {
        if (v.size() > 0) {
            auto item = v.front();
            v.erase(v.begin());
            return item;
        }

        return nullptr;
    }

} // namespace Util
