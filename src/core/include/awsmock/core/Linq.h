//
// Created by vogje01 on 9/5/25.
//

#ifndef AWSMOCK_CORE_LINQ_H
#define AWSMOCK_CORE_LINQ_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace AwsMock::Core {

    /**
     * @brief LINQ like querying of a std::vector
     *
     * @tparam T vector class
     */
    template<typename T>
    class Query {

        std::vector<T> data;

      public:

        explicit Query(std::vector<T> d) : data(std::move(d)) {}

        // where: filter elements
        Query<T> where(std::function<bool(const T &)> pred) const {
            std::vector<T> result;
            std::copy_if(data.begin(), data.end(), std::back_inserter(result), pred);
            return Query<T>(std::move(result));
        }

        /**
         * @brief Select: project into new type
         */
        template<typename U>
        Query<U> select(std::function<U(const T &)> mapper) const {
            std::vector<U> result;
            result.reserve(data.size());
            std::transform(data.begin(), data.end(), std::back_inserter(result), mapper);
            return Query<U>(std::move(result));
        }

        /**
         * @brief order_by: sort ascending
         */
        Query<T> order_by(std::function<bool(const T &, const T &)> comp) const {
            std::vector<T> result = data;
            std::sort(result.begin(), result.end(), comp);
            return Query<T>(std::move(result));
        }

        /**
         * @brief group_by: group elements by key selector
         */
        template<typename K>
        std::map<K, std::vector<T>> group_by(std::function<K(const T &)> keySelector) const {
            std::map<K, std::vector<T>> groups;
            for (const auto &item: data) {
                groups[keySelector(item)].push_back(item);
            }
            return groups;
        }

        /**
         * @brief aggregate: sum (only works for numeric types)
         */
        T sum() const {
            return std::accumulate(data.begin(), data.end(), T{});
        }

        // aggregate: count
        [[nodiscard]] size_t count() const {
            return data.size();
        }

        // first element
        T first() const {
            if (data.empty()) throw std::runtime_error("No elements in query");
            return data.front();
        }

        // any: check if any element matches predicate
        bool any(std::function<bool(const T &)> pred) const {
            return std::any_of(data.begin(), data.end(), pred);
        }

        // to_vector: materialize result
        std::vector<T> to_vector() const {
            return data;
        }

        // range-for support
        auto begin() const { return data.begin(); }
        auto end() const { return data.end(); }
    };

    /**
     * @brief helper: start a query from vector
     */
    template<typename T>
    Query<T> from(const std::vector<T> &v) {
        return Query<T>(v);
    }

}// namespace AwsMock::Core

#endif//AWSMOCK_LINQ_H
