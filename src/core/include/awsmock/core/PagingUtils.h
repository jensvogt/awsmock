//
// Created by vogje01 on 6/27/25.
//

#ifndef AWSMOCK_CORE_PAGING_UTILS_H
#define AWSMOCK_CORE_PAGING_UTILS_H

// C++ includes
#include <map>
#include <vector>

namespace AwsMock::Core {
    template<class S, class P>
    std::map<S, P> PageMap(std::map<S, P> map, const long pageSize, const long pageIndex) {
        std::map<S, P> page;
        if (!map.empty()) {
            int i = 0;
            for (const auto &[fst, snd]: map) {
                if (pageSize >= 0) {
                    if (i >= (pageIndex * pageSize) && i < (pageIndex + 1) * pageSize) {
                        page[fst] = snd;
                    }
                } else {
                    page[fst] = snd;
                }
            }
        }
        return page;
    }


    template<class T>
    std::vector<T> PageVector(std::vector<T> vec, const long pageSize, const long pageIndex) {
        typename std::vector<T>::iterator endArray;
        if (pageSize * (pageIndex + 1) > vec.size()) {
            endArray = vec.end();
        } else {
            endArray = vec.begin() + pageSize * (pageIndex + 1);
        }
        return std::vector(vec.begin() + pageSize * pageIndex, endArray);
    }

}// namespace AwsMock::Core
#endif// AWSMOCK_CORE_PAGING_UTILS_H
