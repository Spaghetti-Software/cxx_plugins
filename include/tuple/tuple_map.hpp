/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_map.hpp
 * \author  Andrey Ponomarev
 * \date    21 May 2020
 * \brief
 * $BRIEF$
 */
#pragma once
#include "tuple/tuple_impl.hpp"
#include "tuple/tuple_cat.hpp"

namespace CxxPlugins {

template <typename T> struct Tag {};

/*!
 * \brief Simplifies declaring tags
 * ```cpp
 * struct Foo {};
 * ...
 * my_map[tag<Foo>] = ...;
 * ```
 * \tparam T User defined tag type
 */
template <typename T> constexpr Tag<T> tag = {};

template <typename... TaggedValues> struct TupleMap;

template <typename TagT, typename T> struct TaggedValue {
  TaggedValue() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
  template <typename U>
  TaggedValue(TagT && /*unused*/,
              U &&val) noexcept(std::is_nothrow_constructible_v<T, U &&>)
      : value_m(std::forward<U>(val)) {}
  template <typename U>
  TaggedValue(TagT const & /*unused*/,
              U &&val) noexcept(std::is_nothrow_constructible_v<T, U &&>)
      : value_m(std::forward<U>(val)) {}
  explicit TaggedValue(T const &val) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : value_m(val) {}

  explicit TaggedValue(T &&val) noexcept(
      std::is_nothrow_move_constructible_v<T>)
      : value_m(std::move(val)) {}

  template <typename U, std::enable_if_t<!std::is_same_v<U, T>, int> = 0>
  TaggedValue &operator=(TaggedValue<TagT, U> const &rhs) noexcept(
      std::is_nothrow_assignable_v<T, const U &>) {
    value_m = rhs.value_m;
    return *this;
  }

  template <typename U, std::enable_if_t<!std::is_same_v<U, T>, int> = 0>
  TaggedValue &operator=(TaggedValue<TagT, U> &&rhs) noexcept(
      std::is_nothrow_assignable_v<T, U &&>) {
    value_m = std::move(rhs.value_m);
    return *this;
  }

  using Tag = TagT;
  T value_m;
};
template <typename Tag, typename T>
TaggedValue(Tag, T &&) -> TaggedValue<Tag, std::decay_t<T>>;

template <typename Tag, typename T> auto makeTaggedValue(T &&val) {
  return TaggedValue<Tag, std::decay_t<T>>(std::forward<T>(val));
}

template <typename Tag, typename T>
auto makeTaggedValue(Tag && /*unused*/, T &&val) {
  return TaggedValue<std::decay_t<Tag>, std::decay_t<T>>(std::forward<T>(val));
}

/*!
 * \brief  Gets TupleMap value type(Use TupleMapElementType for simplicity)
 * \tparam Tag  Tag to get the value type of
 * \tparam T    TupleMap instantiation
 */
template <typename Tag, typename T> struct TupleMapElement;
/*!
 * \brief Alias for TupleMapElement::Type
 */
template <typename Tag, typename T>
using TupleMapElementType = typename TupleMapElement<Tag, T>::Type;

/*!
 * \brief Tuple that can be accessed by type identifier, rather than index.
 *
 * \tparam TaggedValues
 * Value can be any type. Use simple types as tag type. For example you can use
 * `Tag` type or `tag` variable.(Or create your own empty types and use
 * them directly). **All Tags should be unique.**
 *
 *
 * \details
 * ## Motivation
 * TupleMap allows creating different kind of tables that would be more
 * meaningful than regular tuple. For example you have `tuple<void(*)(),
 * void(*)()>`. There is no meaning for function pointer 0 or 1. But you can
 * write something like this:
 * ```cpp
 * struct MeaningfulFn {};
 * struct AnotherMeaningfulFn {};
 * ...
 * void foo() {
 *  auto function_map = makeTupleMap(
 *      makePair(tag<MeaningfulFn>, some_fn_pointer),
 *      makePair(tag<AnotherMeaningfulFn>, another_fn_pointer)
 *  );
 *  function_map[tag<MeaningfulFn>]();
 *  function_map[tag<AnotherMeaningfulFn>]();
 *  ...
 * }
 * ```
 * And now it makes a lot more sense, as we can access members almost by name.
 *
 * ## Implementation details
 * + TupleMap is implemented in terms of Tuple
 * + TupleMap is explicitly convertible to Tuple(in order to avoid miss-usage)
 * + `TupleMap<Pair<tag0,val0>,Pair<tag1,val1>>` is different from
 * `TupleMap<Pair<tag1,val1>, Pair<tag0,val0>>`, but they are
 * assignable,constructable and comparable between each other.
 * + TupleMap is comparable(has operators ==,!=) if
 *   + Both TupleMaps have same tags. The order of tags is not important.
 *   + Value types of associated tags should be comparable.
 * + TupleMap is orderable (has operators <,<=,>,>=) if
 *   + Both TupleMaps has same tags in the same order.
 *   + Value types of associated tags should be orderable.
 *
 */
#ifdef DOXYGEN
template <typename... TaggedValues>
struct TupleMap
#else
template <typename... Tags, typename... TValues>
struct TupleMap<TaggedValue<Tags, TValues>...> : public Tuple<TValues...>
#endif
{

private:
  using Parent = Tuple<TValues...>;

public:
  using Parent::Parent;
  using Parent::operator=;

  /*!
   * \brief Gives index in the tuple for the given tag
   * \tparam Tag
   */

  static_assert(utility::are_unique_v<Tags...>, "All tags should be unique");

  //! \brief Tuple of all tags
  using TagsTuple = Tuple<Tags...>;
  //! \brief Tuple of all values
  using ValuesTuple = Tuple<TValues...>;

  template <typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<
      Tag, TupleMap<TaggedValue<UTags, UArgs>...>> const &
  get(TupleMap<TaggedValue<UTags, UArgs>...> const &map);

  template <typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<Tag,
                                       TupleMap<TaggedValue<UTags, UArgs>...>> &
  get(TupleMap<TaggedValue<UTags, UArgs>...> &map);

  template <typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<
      Tag, TupleMap<TaggedValue<UTags, UArgs>...>> &&
  get(TupleMap<TaggedValue<UTags, UArgs>...> &&map);

  template <typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<
      Tag, TupleMap<TaggedValue<UTags, UArgs>...>> const &&
  get(TupleMap<TaggedValue<UTags, UArgs>...> const &&map);

  //! \brief Default constructor
  constexpr TupleMap() noexcept(
      std::is_nothrow_default_constructible_v<ValuesTuple>) = default;
  //! \overload
  constexpr TupleMap(TupleMap const &) = default;
  //! \overload
  constexpr TupleMap(TupleMap &&) noexcept = default;
  //! \brief Copy assignment operator
  constexpr auto operator=(TupleMap const &) noexcept -> TupleMap & = default;
  //! \overload
  constexpr auto operator=(TupleMap &&) noexcept -> TupleMap & = default;

  //  /*!
  //   * \brief Forward constructor for TupleMap
  //   * \note You should use ::makeTupleMap in most cases instead.
  //   */
  //  template <typename... UValues,
  //            typename = std::enable_if_t<
  //                sizeof...(UValues) != 0 &&
  //                (std::is_constructible_v<ValuesTuple, UValues...>)>>
  //  explicit constexpr TupleMap(UValues &&... vals) noexcept
  //      : Parent(std::forward<UValues>(vals)...) {}

  /*!
   * \brief Copy constructor from TupleMap with another order of template
   * parameters. Or with different value parameters.
   */
  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                ((!std::is_same_v<Tags, UTags> && ...) ||
                 (!std::is_same_v<TValues, UValues> && ...))>>
  explicit constexpr TupleMap(
      TupleMap<TaggedValue<UTags, UValues>...> const &rhs)
      : Parent(get<Tags>(rhs)...) {}

  //! \overload
  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                ((!std::is_same_v<Tags, UTags> && ...) ||
                 (!std::is_same_v<TValues, UValues> && ...))>>
  explicit constexpr TupleMap(TupleMap<TaggedValue<UTags, UValues>...> &&rhs)
      : Parent(std::move(get<Tags>(std::move(rhs)))...) {}

  /*!
   * \brief Copy assignment operator from TupleMap with another order of
   * template parameters. Or with different value parameters.
   */
  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                (!std::is_same_v<Tags, UTags> && ...)>>
  constexpr auto operator=(TupleMap<TaggedValue<UTags, UValues>...> const &rhs)
      -> TupleMap & {
    ((get<Tags>(*this) = get<Tags>(rhs)), ...);
    return *this;
  }

  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                ((!std::is_same_v<Tags, UTags> && ...) ||
                 (!std::is_same_v<TValues, UValues> && ...))>>
  constexpr auto operator=(TupleMap<TaggedValue<UTags, UValues>...> &&rhs)
      -> TupleMap & {
    ((get<Tags>(*this) = std::move(get<Tags>(std::move(rhs)))), ...);
    return *this;
  }

  /*!
   * \brief Subscript operator, provided for convenience. You can use it or
   * get<Tag>(TupleMap const&).
   */
  template <typename Tag>
  constexpr auto operator[]([[maybe_unused]] Tag &&tag) const
      -> decltype(get<Tag>(*this)) {
    return get<std::decay_t<Tag>>(*this);
  }
  //! \overload
  template <typename Tag>
  constexpr auto operator[]([[maybe_unused]] Tag &&tag)
      -> decltype(get<Tag>(*this)) {
    return get<std::decay_t<Tag>>(*this);
  }

  explicit operator Tuple<TValues...> &() {
    return static_cast<Tuple<TValues...> &>(*this);
  }

  explicit operator Tuple<TValues...> const &() const {
    return static_cast<Tuple<TValues...> const &>(*this);
  }
};

template <typename... TaggedValues>
TupleMap(TaggedValues...) -> TupleMap<TaggedValues...>;

// CxxPlugins::impl::Compare<
//    CxxPlugins::TupleMap<CxxPlugins::TaggedValue<CxxPlugins::Tag<Foo>, float>,
//                         CxxPlugins::TaggedValue<CxxPlugins::Tag<Bar>,
//                         float>>,
//    CxxPlugins::TupleMap<CxxPlugins::TaggedValue<CxxPlugins::Tag<Foo>,
//    double>,
//                         CxxPlugins::TaggedValue<CxxPlugins::Tag<Bar>,
//                         double>>>

namespace impl {
template <typename... TTags, typename... TValues, typename... UTags,
          typename... UValues>
struct Compare<TupleMap<TaggedValue<TTags, TValues>...>,
               TupleMap<TaggedValue<UTags, UValues>...>> {
  static_assert(sizeof...(TValues) == sizeof...(UValues),
                "Sizes should be same");
  static_assert((utility::is_in_the_pack_v<TTags, UTags...> && ...),
                "TTags and UTags should be same(order doesn't matter).");

  using LeftT = TupleMap<TaggedValue<TTags, TValues>...>;
  using RighT = TupleMap<TaggedValue<UTags, UValues>...>;

  static constexpr auto equal(LeftT const &lhs, RighT const &rhs) -> bool {
    return ((get<TTags>(lhs) == get<TTags>(rhs)) && ...);
  }

  template <std::size_t i = 0>
  static constexpr auto less(LeftT const &lhs, RighT const &rhs) -> bool {
    static_assert(
        (std::is_same_v<TTags, UTags> && ...),
        "For order comparison TTags and UTags should have the same order.");

    using LeftBase = Tuple<TValues...>;
    using RightBase = Tuple<UValues...>;

    return static_cast<LeftBase const &>(lhs) <
           static_cast<RightBase const &>(rhs);
  }
};

template <> struct Compare<TupleMap<>, TupleMap<>> {
  template <std::size_t i = 0>
  static constexpr bool equal(TupleMap<> const & /*unused*/,
                              TupleMap<> const & /*unused*/) {
    return true;
  }

  template <std::size_t i = 0>
  static constexpr bool less(TupleMap<> const & /*unused*/,
                             TupleMap<> const & /*unused*/) {
    return false;
  }
};
} // namespace impl

template <typename... Ts, typename... Us>
constexpr auto operator==(TupleMap<Ts...> const &lhs,
                          TupleMap<Us...> const &rhs) {
  return impl::Compare<TupleMap<Ts...>, TupleMap<Us...>>::equal(lhs, rhs);
}

template <typename... Ts, typename... Us>
constexpr auto operator!=(TupleMap<Ts...> const &lhs,
                          TupleMap<Us...> const &rhs) {
  return !(lhs == rhs);
}

template <typename... Ts, typename... Us>
constexpr auto operator<(TupleMap<Ts...> const &lhs,
                         TupleMap<Us...> const &rhs) {
  return impl::Compare<TupleMap<Ts...>, TupleMap<Us...>>::less(lhs, rhs);
}

template <typename... Ts, typename... Us>
constexpr auto operator<=(TupleMap<Ts...> const &lhs,
                          TupleMap<Us...> const &rhs) {
  return !(rhs < lhs);
}

template <typename... Ts, typename... Us>
constexpr auto operator>(TupleMap<Ts...> const &lhs,
                         TupleMap<Us...> const &rhs) {
  return rhs < lhs;
}

template <typename... Ts, typename... Us>
constexpr auto operator>=(TupleMap<Ts...> const &lhs,
                          TupleMap<Us...> const &rhs) {
  return !(lhs < rhs);
}

template <typename Tag, typename... Tags, typename... Values>
struct TupleMapElement<Tag, TupleMap<TaggedValue<Tags, Values>...>> {
  using Type = utility::ElementType<utility::index_of<Tag, Tags...>, Values...>;
};

template <typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<TaggedValue<UTags, UArgs>...> const &map)
    -> TupleMapElementType<Tag, TupleMap<TaggedValue<UTags, UArgs>...>> const
        & {
  return get<utility::index_of<Tag, UTags...>>(map);
}

template <typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<TaggedValue<UTags, UArgs>...> &map)
    -> TupleMapElementType<Tag, TupleMap<TaggedValue<UTags, UArgs>...>> & {
  return get<utility::index_of<Tag, UTags...>>(map);
}

template <typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<TaggedValue<UTags, UArgs>...> &&map)
    -> TupleMapElementType<Tag, TupleMap<TaggedValue<UTags, UArgs>...>> && {
  return get<utility::index_of<Tag, UTags...>>(std::move(map));
}

template <typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<TaggedValue<UTags, UArgs>...> const &&map)
    -> TupleMapElementType<Tag, TupleMap<TaggedValue<UTags, UArgs>...>> const
        && {
  get<utility::index_of<Tag, UTags...>>(std::move(map));
}



/*!
 * \brief   Creates TupleMap from list of tagged values
 * \relates TupleMap
 */
template <typename... TaggedValues>
constexpr auto makeTupleMap(TaggedValues &&... vals) {
  return TupleMap<std::decay_t<TaggedValues>...>(vals.value_m...);
}


template<std::size_t I, typename... Tags, typename... Values>
struct TupleCatElement<I, TupleMap<TaggedValue<Tags,Values>...>> {
  using Type = utility::ElementType<I,TaggedValue<Tags,Values>...>;
};


/*!
 * \brief Creates new TupleMap that is a subset of given TupleMap.
 * \tparam SelectedTags Tags that will be inside new TupleMap
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... SelectedTags>
constexpr auto
tupleMapSubMap(TupleMap<TaggedValue<Tags, Values>...> const &old_map,
               [[maybe_unused]] SelectedTags &&... tags) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<SelectedTags>, Tags...> && ...),
      "SelectedTags should be a subset of Tags");
  static_assert((!std::is_reference_v<Tags> && ...) &&
                    (!std::is_const_v<Tags> && ...) &&
                    (!std::is_volatile_v<Tags> && ...),
                "Tags should be non const non reference non volatile types");
  using OldType = std::decay_t<TupleMap<TaggedValue<Tags, Values>...>>;
  using NewType = TupleMap<
      TaggedValue<std::decay_t<SelectedTags>,
                  TupleMapElementType<std::decay_t<SelectedTags>, OldType>>...>;
  return NewType(get<std::decay_t<SelectedTags>>(old_map)...);
}
/*!
 * \brief Creates new TupleMap that is a subset of given TupleMap.
 * \tparam SelectedTags Tags that will be inside new TupleMap
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... SelectedTags>
constexpr auto tupleMapSubMap(TupleMap<TaggedValue<Tags, Values>...> &&old_map,
                              [[maybe_unused]] SelectedTags &&... tags) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<SelectedTags>, Tags...> && ...),
      "SelectedTags should be a subset of Tags");
  using OldType = std::decay_t<TupleMap<TaggedValue<Tags, Values>...>>;
  using NewType = TupleMap<
      TaggedValue<std::decay_t<SelectedTags>,
                  TupleMapElementType<std::decay_t<SelectedTags>, OldType>>...>;
  return NewType(
      std::move(get<std::decay_t<SelectedTags>>(std::move(old_map)))...);
}

///*!
// * \brief  Creates new TupleMap from the given one and additional list of
// pairs
// * \tparam NewPairs List of pairs to insert.
// * \relates TupleMap
// */
// template <typename OldMap, typename... NewPairs>
// constexpr auto tupleMapInsertBack(OldMap &&old_map, NewPairs &&... pairs) {
//  return tupleCat(std::forward<OldMap>(old_map),
//                  std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)));
//}

///*!
// * \brief  Creates new TupleMap from the given one and additional list of
// pairs
// * \tparam NewPairs List of pairs to insert.
// * \relates TupleMap
// */
// template <typename OldMap, typename... NewPairs>
// constexpr auto tupleMapInsertFront(OldMap &&old_map, NewPairs &&... pairs) {
//  return
//  tupleMapCat(std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)),
//                     std::forward<OldMap>(old_map));
//}
//
///*!
// * \brief  Creates new TupleMap from the given one and additional list of
// pairs
// * \tparam NewPairs List of pairs to insert.
// * \relates TupleMap
// */
// template <typename Position, typename OldMap, typename... NewPairs>
// constexpr auto tupleMapInsert(OldMap &&old_map, NewPairs &&... pairs) {
//  return
//  tupleMapCat(std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)),
//                     std::forward<OldMap>(old_map));
//}

// namespace impl {
// template <typename NewTags, typename... Tags, typename... Values,
//          std::size_t... indices>
// constexpr auto tupleMapEraseHelper(
//    TupleMap<Pair<Tags, Values>...> const &old_map,
//    [[maybe_unused]] std::integer_sequence<std::size_t, indices...>
//        indices_var) {
//  return tupleMapSubMap(old_map, TupleElementType<indices, NewTags>()...);
//}
// template <typename NewTags, typename... Tags, typename... Values,
//          std::size_t... indices>
// constexpr auto tupleMapEraseHelper(
//    TupleMap<Pair<Tags, Values>...> &&old_map,
//    [[maybe_unused]] std::integer_sequence<std::size_t, indices...>
//        indices_var) {
//  return tupleMapSubMap(std::move(old_map),
//                        TupleElementType<indices, NewTags>()...);
//}
//} // namespace impl
//
///*!
// * \brief  Creates new TupleMap without given tags
// * \tparam TagsToErase Tags that will be erased
// * \relates TupleMap
// */
// template <typename... Tags, typename... Values, typename... TagsToErase>
// constexpr auto tupleMapErase(TupleMap<Pair<Tags, Values>...> const &old_map,
//                             [[maybe_unused]] TagsToErase &&... tagsToErase) {
//  static_assert(
//      (utility::is_in_the_pack_v<std::decay_t<TagsToErase>, Tags...> && ...),
//      "TagsToErase should be in the Tags");
//
//  using NewTagsTuple = TupleCatType<std::conditional_t<
//      utility::is_in_the_pack_v<Tags, std::decay_t<TagsToErase>...>, Tuple<>,
//      Tuple<Tags>>...>;
//
//  using Sequence =
//      std::make_integer_sequence<std::size_t, tuple_size_v<NewTagsTuple>>;
//
//  return impl::tupleMapEraseHelper<NewTagsTuple>(old_map, Sequence());
//}
//
///*!
// * \brief  Creates new TupleMap without given tags
// * \tparam TagsToErase Tags that will be erased
// * \relates TupleMap
// */
// template <typename... Tags, typename... Values, typename... TagsToErase>
// constexpr auto tupleMapErase(TupleMap<Pair<Tags, Values>...> &&old_map,
//                             [[maybe_unused]] TagsToErase &&... tagsToErase) {
//  static_assert(
//      (utility::is_in_the_pack_v<std::decay_t<TagsToErase>, Tags...> && ...),
//      "TagsToErase should be in the Tags");
//
//  using NewTagsTuple = TupleCatType<std::conditional_t<
//      utility::is_in_the_pack_v<Tags, std::decay_t<TagsToErase>...>, Tuple<>,
//      Tuple<Tags>>...>;
//
//  using Sequence =
//      std::make_integer_sequence<std::size_t, tuple_size_v<NewTagsTuple>>;
//
//  return impl::tupleMapEraseHelper<NewTagsTuple>(std::move(old_map),
//                                                 Sequence());
//}

} // namespace CxxPlugins