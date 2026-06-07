//
// Created by vogje01 on 6/27/25.
//

#pragma once

// C++ includes
#include <map>
#include <vector>

namespace Awsmock::Core {

    template<class S, class P>
    std::map<S, P> PageMap(std::map<S, P> map, const long pageSize, const long pageIndex) {
        std::map<S, P> page;
        if (!map.empty()) {
            for (const auto &[fst, snd]: map) {
                if (pageSize >= 0) {
                    if (constexpr int i = 0; i >= (pageIndex * pageSize) && i < (pageIndex + 1) * pageSize) {
                        page[fst] = snd;
                    }
                } else {
                    page[fst] = snd;
                }
            }
            return page;
        }
        return map;
    }

    template<class T>
    std::vector<T> PageVector(std::vector<T> vec, const long pageSize, const long pageIndex) {
        if (pageSize <= 0) {
            return vec;
        }
        typename std::vector<T>::iterator endArray;
        if (pageSize * (pageIndex + 1) > vec.size()) {
            endArray = vec.end();
        } else {
            endArray = vec.begin() + pageSize * (pageIndex + 1);
        }
        return std::vector(vec.begin() + pageSize * pageIndex, endArray);
    }

}// namespace Awsmock::Core
