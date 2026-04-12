//
// Created by jensv on 10/04/2026.
//

#ifndef AWSMOCK_DTO_MAPPER_H
#define AWSMOCK_DTO_MAPPER_H

#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

// ===========================================================================
// mapper.hpp
//
// Provides three building blocks:
//
//   Mapper<Entity, Dto>                   — virtual, injectable, supports
//                                           nested mappers via constructor
//
//   StaticMapper<Derived, Entity, Dto>    — CRTP, zero virtual dispatch,
//                                           nested mappers called statically
//
//   MapperRegistry                        — type-erased store; lets a
//                                           polymorphic mapper resolve its
//                                           nested dependencies at runtime
//
// NESTED OBJECTS — polymorphic style
// -----------------------------------
// Inject the nested mapper through the constructor and store it as a
// const reference to the base Mapper<> interface:
//
//   class UserMapper : public Mapper<User, UserDto>
//   {
//       const Mapper<Address, AddressDto>& addr_;
//   public:
//       explicit UserMapper(const Mapper<Address, AddressDto>& a) : addr_(a) {}
//
//       UserDto toDto(const User& e) const override {
//           return { e.id, e.name, addr_.toDto(e.address) };
//       }
//       User toEntity(const UserDto& d) const override {
//           return { d.id, d.name, addr_.toEntity(d.address) };
//       }
//   };
//
// NESTED OBJECTS — static style
// ------------------------------
// Call the nested StaticMapper directly — no injection needed:
//
//   class UserMapper : public StaticMapper<UserMapper, User, UserDto>
//   {
//   public:
//       static UserDto toDto(const User& e) {
//           return { e.id, e.name, AddressMapper::toDto(e.address) };
//       }
//       static User toEntity(const UserDto& d) {
//           return { d.id, d.name, AddressMapper::toEntity(d.address) };
//       }
//   };
// ===========================================================================

// ===========================================================================
// 1.  Concepts — pin down what Entity and Dto must satisfy
// ===========================================================================

namespace AwsMock::Dto {

    template<typename T>
    concept Mappable = std::movable<T> && std::copyable<T>;
    // ===========================================================================
    // 1.  Mapper<Entity, Dto>  —  virtual / polymorphic
    // ===========================================================================

    template<Mappable Entity, Mappable Dto>
    class Mapper {
      public:

        virtual ~Mapper() = default;

        // -----------------------------------------------------------------------
        // Core contract
        // -----------------------------------------------------------------------
        virtual Dto toDto(const Entity &entity) const = 0;

        virtual Entity toEntity(const Dto &dto) const = 0;

        // -----------------------------------------------------------------------
        // Nested-object helpers
        // Delegates to another Mapper<NE, ND> held by the concrete subclass.
        // These are free functions on the base so callers don't need to know the
        // concrete nested mapper type.
        // -----------------------------------------------------------------------

        // Map a nested entity → dto using a supplied mapper reference
        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static ND mapNested(const Mapper<NE, ND> &nestedMapper, const NE &entity) {
            return nestedMapper.toDto(entity);
        }

        // Map a nested dto → entity using a supplied mapper reference
        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static NE mapNestedReverse(const Mapper<NE, ND> &nestedMapper, const ND &dto) {
            return nestedMapper.toEntity(dto);
        }

        // Map an optional nested value
        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::optional<ND> mapNestedOpt(
                const Mapper<NE, ND> &nestedMapper,
                const std::optional<NE> &opt) {
            if (!opt) return std::nullopt;
            return nestedMapper.toDto(*opt);
        }

        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::optional<NE> mapNestedOptReverse(
                const Mapper<NE, ND> &nestedMapper,
                const std::optional<ND> &opt) {
            if (!opt) return std::nullopt;
            return nestedMapper.toEntity(*opt);
        }

        // Map a vector of nested values
        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::vector<ND> mapNestedList(
                const Mapper<NE, ND> &nestedMapper,
                const std::vector<NE> &entities) {
            std::vector<ND> out;
            out.reserve(entities.size());
            std::ranges::transform(entities, std::back_inserter(out),
                                   [&](const NE &e) { return nestedMapper.toDto(e); });
            return out;
        }

        template<Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::vector<NE> mapNestedListReverse(
                const Mapper<NE, ND> &nestedMapper,
                const std::vector<ND> &dtos) {
            std::vector<NE> out;
            out.reserve(dtos.size());
            std::ranges::transform(dtos, std::back_inserter(out),
                                   [&](const ND &d) { return nestedMapper.toEntity(d); });
            return out;
        }

        // -----------------------------------------------------------------------
        // std::map helpers — key is copied as-is, only the value is mapped.
        //
        // Basic usage (key type is the same on both sides):
        //   std::map<std::string, Order>    -> std::map<std::string, OrderDto>
        //   mapNestedMap(orderMapper, e.ordersByRef)
        //
        // With key transformation (key type differs between entity and dto):
        //   std::map<int, Order>            -> std::map<std::string, OrderDto>
        //   mapNestedMapWithKey(orderMapper, e.ordersByRef,
        //       [](int k){ return std::to_string(k); })
        // -----------------------------------------------------------------------

        // map<K, NE> → map<K, ND>  (key unchanged)
        template<typename K, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::map<K, ND> mapNestedMap(
                const Mapper<NE, ND> &nestedMapper,
                const std::map<K, NE> &in) {
            std::map<K, ND> out;
            for (const auto &[k, v]: in)
                out.emplace(k, nestedMapper.toDto(v));
            return out;
        }

        // map<K, ND> → map<K, NE>  (key unchanged)
        template<typename K, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::map<K, NE> mapNestedMapReverse(
                const Mapper<NE, ND> &nestedMapper,
                const std::map<K, ND> &in) {
            std::map<K, NE> out;
            for (const auto &[k, v]: in)
                out.emplace(k, nestedMapper.toEntity(v));
            return out;
        }

        // map<KE, NE> → map<KD, ND>  (key and value both transformed)
        template<typename KE, typename KD, Mappable NE, Mappable ND, typename KeyFn>
        [[nodiscard]]
        static std::map<KD, ND> mapNestedMapWithKey(
                const Mapper<NE, ND> &nestedMapper,
                const std::map<KE, NE> &in,
                KeyFn &&keyFn)// callable: KE -> KD
        {
            std::map<KD, ND> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), nestedMapper.toDto(v));
            return out;
        }

        // map<KD, ND> → map<KE, NE>  (key and value both transformed)
        template<typename KD, typename KE, Mappable ND, Mappable NE, typename KeyFn>
        [[nodiscard]]
        static std::map<KE, NE> mapNestedMapWithKeyReverse(
                const Mapper<NE, ND> &nestedMapper,
                const std::map<KD, ND> &in,
                KeyFn &&keyFn)// callable: KD -> KE
        {
            std::map<KE, NE> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), nestedMapper.toEntity(v));
            return out;
        }

        // -----------------------------------------------------------------------
        // Top-level collection helpers
        // -----------------------------------------------------------------------

        [[nodiscard]]
        std::vector<Dto> toDtoList(const std::vector<Entity> &entities) const {
            std::vector<Dto> out;
            out.reserve(entities.size());
            std::ranges::transform(entities, std::back_inserter(out),
                                   [this](const Entity &e) { return toDto(e); });
            return out;
        }

        [[nodiscard]]
        std::vector<Entity> toEntityList(const std::vector<Dto> &dtos) const {
            std::vector<Entity> out;
            out.reserve(dtos.size());
            std::ranges::transform(dtos, std::back_inserter(out),
                                   [this](const Dto &d) { return toEntity(d); });
            return out;
        }

        // Accepts any input range (e.g. BsonCursorRange<Entity>)
        template<std::ranges::input_range R>
            requires std::same_as<std::ranges::range_value_t<R>, Entity>
        [[nodiscard]]
        std::vector<Dto> toDtoList(R &&range) const {
            std::vector<Dto> out;
            for (auto &&e: std::forward<R>(range))
                out.push_back(toDto(e));
            return out;
        }

        template<std::ranges::input_range R>
            requires std::same_as<std::ranges::range_value_t<R>, Dto>
        [[nodiscard]]
        std::vector<Entity> toEntityList(R &&range) const {
            std::vector<Entity> out;
            for (auto &&d: std::forward<R>(range))
                out.push_back(toEntity(d));
            return out;
        }

        [[nodiscard]]
        std::optional<Dto> toDtoOpt(const std::optional<Entity> &opt) const {
            if (!opt) return std::nullopt;
            return toDto(*opt);
        }

        [[nodiscard]]
        std::optional<Entity> toEntityOpt(const std::optional<Dto> &opt) const {
            if (!opt) return std::nullopt;
            return toEntity(*opt);
        }
    };


    // ===========================================================================
    // 2.  StaticMapper<Derived, Entity, Dto>  —  CRTP, zero virtual dispatch
    // ===========================================================================

    template<typename Derived, Mappable Entity, Mappable Dto>
    class StaticMapper {
      public:

        // Derived must provide:
        //   static Dto    toDto   (const Entity&);
        //   static Entity toEntity(const Dto&);
        //
        // For nested objects, call the nested StaticMapper directly:
        //   static UserDto toDto(const User& e) {
        //       return { e.id, AddressMapper::toDto(e.address) };
        //   }

        // -----------------------------------------------------------------------
        // Nested-object helpers  (static, no injection required)
        // -----------------------------------------------------------------------

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static ND mapNested(const NE &entity) {
            return NestedDerived::toDto(entity);
        }

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static NE mapNestedReverse(const ND &dto) {
            return NestedDerived::toEntity(dto);
        }

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::optional<ND> mapNestedOpt(const std::optional<NE> &opt) {
            if (!opt) return std::nullopt;
            return NestedDerived::toDto(*opt);
        }

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::optional<NE> mapNestedOptReverse(const std::optional<ND> &opt) {
            if (!opt) return std::nullopt;
            return NestedDerived::toEntity(*opt);
        }

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::vector<ND> mapNestedList(const std::vector<NE> &entities) {
            std::vector<ND> out;
            out.reserve(entities.size());
            std::ranges::transform(entities, std::back_inserter(out),
                                   [](const NE &e) { return NestedDerived::toDto(e); });
            return out;
        }

        template<typename NestedDerived, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::vector<NE> mapNestedListReverse(const std::vector<ND> &dtos) {
            std::vector<NE> out;
            out.reserve(dtos.size());
            std::ranges::transform(dtos, std::back_inserter(out),
                                   [](const ND &d) { return NestedDerived::toEntity(d); });
            return out;
        }

        // map<K, NE> → map<K, ND>  (key unchanged)
        template<typename NestedDerived, typename K, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::map<K, ND> mapNestedMap(const std::map<K, NE> &in) {
            std::map<K, ND> out;
            for (const auto &[k, v]: in)
                out.emplace(k, NestedDerived::toDto(v));
            return out;
        }

        // map<K, ND> → map<K, NE>  (key unchanged)
        template<typename NestedDerived, typename K, Mappable NE, Mappable ND>
        [[nodiscard]]
        static std::map<K, NE> mapNestedMapReverse(const std::map<K, ND> &in) {
            std::map<K, NE> out;
            for (const auto &[k, v]: in)
                out.emplace(k, NestedDerived::toEntity(v));
            return out;
        }

        // map<KE, NE> → map<KD, ND>  (key and value both transformed)
        template<typename NestedDerived, typename KE, typename KD,
                 Mappable NE, Mappable ND, typename KeyFn>
        [[nodiscard]]
        static std::map<KD, ND> mapNestedMapWithKey(
                const std::map<KE, NE> &in,
                KeyFn &&keyFn)// callable: KE -> KD
        {
            std::map<KD, ND> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), NestedDerived::toDto(v));
            return out;
        }

        // map<KD, ND> → map<KE, NE>  (key and value both transformed)
        template<typename NestedDerived, typename KD, typename KE,
                 Mappable ND, Mappable NE, typename KeyFn>
        [[nodiscard]]
        static std::map<KE, NE> mapNestedMapWithKeyReverse(
                const std::map<KD, ND> &in,
                KeyFn &&keyFn)// callable: KD -> KE
        {
            std::map<KE, NE> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), NestedDerived::toEntity(v));
            return out;
        }

        // -----------------------------------------------------------------------
        // Top-level collection helpers
        // -----------------------------------------------------------------------

        [[nodiscard]]
        static std::vector<Dto> toDtoList(const std::vector<Entity> &entities) {
            std::vector<Dto> out;
            out.reserve(entities.size());
            std::ranges::transform(entities, std::back_inserter(out), Derived::toDto);
            return out;
        }

        [[nodiscard]]
        static std::vector<Entity> toEntityList(const std::vector<Dto> &dtos) {
            std::vector<Entity> out;
            out.reserve(dtos.size());
            std::ranges::transform(dtos, std::back_inserter(out), Derived::toEntity);
            return out;
        }

        template<std::ranges::input_range R>
            requires std::same_as<std::ranges::range_value_t<R>, Entity>
        [[nodiscard]]
        static std::vector<Dto> toDtoList(R &&range) {
            std::vector<Dto> out;
            for (auto &&e: std::forward<R>(range))
                out.push_back(Derived::toDto(e));
            return out;
        }

        template<std::ranges::input_range R>
            requires std::same_as<std::ranges::range_value_t<R>, Dto>
        [[nodiscard]]
        static std::vector<Entity> toEntityList(R &&range) {
            std::vector<Entity> out;
            for (auto &&d: std::forward<R>(range))
                out.push_back(Derived::toEntity(d));
            return out;
        }

        [[nodiscard]]
        static std::optional<Dto> toDtoOpt(const std::optional<Entity> &opt) {
            if (!opt) return std::nullopt;
            return Derived::toDto(*opt);
        }

        [[nodiscard]]
        static std::optional<Entity> toEntityOpt(const std::optional<Dto> &opt) {
            if (!opt) return std::nullopt;
            return Derived::toEntity(*opt);
        }
        // map<K, Entity> → map<K, Dto>
        template<typename K>
        [[nodiscard]]
        static std::map<K, Dto> toDtoMap(const std::map<K, Entity> &in) {
            std::map<K, Dto> out;
            for (const auto &[k, v]: in)
                out.emplace(k, Derived::toDto(v));
            return out;
        }

        // map<K, Dto> → map<K, Entity>
        template<typename K>
        [[nodiscard]]
        static std::map<K, Entity> toEntityMap(const std::map<K, Dto> &in) {
            std::map<K, Entity> out;
            for (const auto &[k, v]: in)
                out.emplace(k, Derived::toEntity(v));
            return out;
        }

        // map<KE, Entity> → map<KD, Dto>  (key also transformed)
        template<typename KE, typename KD, typename KeyFn>
        [[nodiscard]]
        static std::map<KD, Dto> toDtoMapWithKey(const std::map<KE, Entity> &in, KeyFn &&keyFn) {
            std::map<KD, Dto> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), Derived::toDto(v));
            return out;
        }

        // map<KD, Dto> → map<KE, Entity>  (key also transformed)
        template<typename KD, typename KE, typename KeyFn>
        [[nodiscard]]
        static std::map<KE, Entity> toEntityMapWithKey(const std::map<KD, Dto> &in, KeyFn &&keyFn) {
            std::map<KE, Entity> out;
            for (const auto &[k, v]: in)
                out.emplace(std::invoke(keyFn, k), Derived::toEntity(v));
            return out;
        }

      protected:

        ~StaticMapper() = default;
    };


    // ===========================================================================
    // 3.  MapperRegistry
    //
    // Type-erased store for polymorphic Mapper<> instances.
    // Useful when a mapper needs to resolve its nested dependencies at runtime
    // (e.g. when wired together by a DI container).
    //
    // Usage:
    //   MapperRegistry registry;
    //   registry.add<Address, AddressDto>(std::make_shared<AddressMapper>());
    //   registry.add<User, UserDto>(
    //       std::make_shared<UserMapper>(registry.get<Address, AddressDto>()));
    //
    //   auto& userMapper = registry.get<User, UserDto>();
    //   UserDto dto = userMapper.toDto(user);
    // ===========================================================================

    class MapperRegistry {
      public:

        template<Mappable Entity, Mappable Dto>
        void add(std::shared_ptr<Mapper<Entity, Dto>> mapper) {
            store_[key<Entity, Dto>()] = std::move(mapper);
        }

        // Returns a reference valid for the lifetime of the registry
        template<Mappable Entity, Mappable Dto>
        Mapper<Entity, Dto> &get() const {
            auto it = store_.find(key<Entity, Dto>());
            if (it == store_.end())
                throw std::runtime_error(
                        "MapperRegistry: no mapper registered for the requested types");
            return *std::static_pointer_cast<Mapper<Entity, Dto>>(it->second);
        }

      private:

        using Key = std::pair<std::type_index, std::type_index>;

        struct KeyHash {
            std::size_t operator()(const Key &k) const noexcept {
                auto h1 = k.first.hash_code();
                auto h2 = k.second.hash_code();
                return h1 ^ (h2 * 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
            }
        };

        template<typename Entity, typename Dto>
        static Key key() {
            return {typeid(Entity), typeid(Dto)};
        }

        std::unordered_map<Key, std::shared_ptr<void>, KeyHash> store_;
    };
}// namespace AwsMock::Dto

#endif//AWSMOCK_DTO_MAPPER_H
