#pragma once

#include <bsoncxx/document/view.hpp>
#include <mongocxx/cursor.hpp>
#include <functional>
#include <iterator>
#include <type_traits>

// ---------------------------------------------------------------------------
// BsonCursorRange<T, Mapper>
//
// A lazy, single-pass input range that wraps a mongocxx::cursor and maps
// each document to T on demand.
//
// Two construction modes:
//
//  1. Trait-based (default) — uses BsonMapper<T>::fromBson(view):
//
//       BsonCursorRange<UserDto> range(std::move(cursor));
//
//  2. Custom mapper — pass any callable (bsoncxx::document::view) -> T:
//
//       auto range = BsonCursorRange<UserDto>::withMapper(
//           std::move(cursor),
//           [](bsoncxx::document::view doc) {
//               return UserDto{ ... };
//           });
//
// IMPORTANT CONSTRAINTS (inherited from the MongoDB C++ driver):
//   - The cursor is move-only and single-pass; iterate at most once.
//   - bsoncxx::document::view is only valid until the next increment;
//     BsonMapper / your custom callable must copy any data it needs.
// ---------------------------------------------------------------------------

namespace AwsMock::Core {

    // Forward-declare the trait so specializations can live alongside each DTO.
    // Leaving the primary template undefined means a missing specialization is a
    // compile-time error rather than a silent runtime failure.
    template<typename T>
    struct BsonMapper;

    // ---------------------------------------------------------------------------
    // Implementation detail: the iterator is parameterised on the mapper type so
    // that a plain function pointer, a lambda, or a BsonMapper trait wrapper all
    // resolve at compile time with zero overhead.
    // ---------------------------------------------------------------------------
    namespace detail {

        template<typename T, typename MapFn>
        class BsonIterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T *;
            using reference = T; // returns by value — view lifetime is transient

            BsonIterator(mongocxx::cursor::iterator inner, MapFn mapper) : inner_(std::move(inner)), mapper_(std::move(mapper)) {
            }

            // Dereference: map the current document to T on demand.
            // The returned T is a value — do NOT hold a reference across operator++.
            value_type operator*() const {
                return mapper_(*inner_);
            }

            BsonIterator &operator++() {
                ++inner_;
                return *this;
            }

            bool operator==(const BsonIterator &o) const { return inner_ == o.inner_; }
            bool operator!=(const BsonIterator &o) const { return inner_ != o.inner_; }

        private:
            mongocxx::cursor::iterator inner_;
            MapFn mapper_;
        };

    } // namespace detail

    // ---------------------------------------------------------------------------
    // BsonCursorRange
    // ---------------------------------------------------------------------------
    template<typename T, typename MapFn = std::function<T(bsoncxx::document::view)> >
    class BsonCursorRange {
    public:
        using iterator = detail::BsonIterator<T, MapFn>;

        // -----------------------------------------------------------------------
        // Trait-based constructor — uses BsonMapper<T>::fromBson
        // -----------------------------------------------------------------------
        explicit BsonCursorRange(mongocxx::cursor cursor) : cursor_(std::move(cursor)), mapper_([](bsoncxx::document::view doc) {
            return BsonMapper<T>::fromBson(doc);
        }) {
        }

        // -----------------------------------------------------------------------
        // Factory: custom mapper callable
        //
        //   auto range = BsonCursorRange<MyDto>::withMapper(
        //       std::move(cursor),
        //       [](bsoncxx::document::view doc) { return MyDto{...}; });
        //
        // The deduced MapFn avoids the overhead of std::function when the caller
        // passes a lambda or function pointer directly.
        // -----------------------------------------------------------------------
        template<typename Fn>
        static auto withMapper(mongocxx::cursor cursor, Fn &&fn) -> BsonCursorRange<T, std::decay_t<Fn> > {
            return BsonCursorRange<T, std::decay_t<Fn> >(std::move(cursor), std::forward<Fn>(fn));
        }

        // Single-pass — begin() may only be called once
        iterator begin() { return iterator(cursor_.begin(), mapper_); }
        iterator end() { return iterator(cursor_.end(), mapper_); }

    private:
        // Private constructor used by withMapper
        BsonCursorRange(mongocxx::cursor cursor, MapFn mapper) : cursor_(std::move(cursor)), mapper_(std::move(mapper)) {
        }

        mongocxx::cursor cursor_;
        MapFn mapper_;
    };

} // namespace AwsMock::Core
