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
#include "tuple.hpp"

namespace CxxPlugins {
template <typename... Pairs> struct TupleMap;

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

template <typename... Pairs> struct TupleMap;


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
 * \tparam Pairs Pair
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
 * And now it makes a lot more sense, as we access members almost by name.
 *
 * ## Implementation details
 * + TupleMap stores Tuple inside. Index of each Tag is computed at
 * compile-time.
 * + `TupleMap<Pair<tag0,val0>,Pair<tag1,val1>>` is different from
 * `TupleMap<Pair<tag1,val1>, Pair<tag0,val0>>`, but they are
 * assignable,constructable and comparable between each other.
 *
 */
#ifdef DOXYGEN
template <typename... Pairs>
struct TupleMap
#else
template <typename... Tags, typename... TValues>
struct TupleMap<Pair<Tags, TValues>...>
#endif
{
public:
  /*!
   * \brief Gives index in the tuple for the given tag
   * \tparam Tag
   */

  static_assert(utility::are_unique_v<Tags...>, "All tags should be unique");

  using TagsTuple = Tuple<Tags...>;
  using ValuesTuple = Tuple<TValues...>;

  template<typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<Tag, TupleMap<Pair<UTags,UArgs>...>> const& get(TupleMap<Pair<UTags, UArgs>...> const& map);

  template<typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>> & get(TupleMap<Pair<UTags, UArgs>...>& map);

  template<typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>> && get(TupleMap<Pair<UTags, UArgs>...>&& map);

  template<typename Tag, typename... UTags, typename... UArgs>
  friend constexpr TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>> const&& get(TupleMap<Pair<UTags, UArgs>...> const && map);


  //! \brief Default constructor
  constexpr TupleMap() noexcept(std::is_nothrow_default_constructible_v<ValuesTuple>) = default;
  //! \overload
  constexpr TupleMap(TupleMap const &) = default;
  //! \overload
  constexpr TupleMap(TupleMap &&) noexcept = default;
  //! \brief Copy assignment operator
  constexpr auto operator=(TupleMap const &) noexcept -> TupleMap & = default;
  //! \overload
  constexpr auto operator=(TupleMap &&) noexcept -> TupleMap & = default;

  /*!
   * \brief Forward constructor for TupleMap
   * \note You should use ::makeTupleMap in most cases instead.
   */
  template <typename... UValues,
            typename = std::enable_if_t<
                sizeof...(UValues) != 0 &&
                (std::is_constructible_v<ValuesTuple, UValues...>)>>
  explicit constexpr TupleMap(UValues &&... vals) noexcept
      : values_m(std::forward<UValues>(vals)...) {}

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
  constexpr TupleMap(TupleMap<Pair<UTags, UValues>...> const &rhs)
      : values_m(get<Tags>(rhs)...) {}

  //! \overload
  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                ((!std::is_same_v<Tags, UTags> && ...) ||
                 (!std::is_same_v<TValues, UValues> && ...))>>
  constexpr TupleMap(TupleMap<Pair<UTags, UValues>...> &&rhs)
      : values_m(std::move(get<Tags>(std::move(rhs)))...) {}

  /*!
   * \brief Copy assignment operator from TupleMap with another order of
   * template parameters. Or with different value parameters.
   */
  template <typename... UTags, typename... UValues,
            typename = std::enable_if_t<
                sizeof...(Tags) == sizeof...(UTags) &&
                (utility::is_in_the_pack_v<Tags, UTags...> && ...) &&
                (!std::is_same_v<Tags, UTags> && ...)>>
  constexpr auto operator=(TupleMap<Pair<UTags, UValues>...> const &rhs)
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
  constexpr auto operator=(TupleMap<Pair<UTags, UValues>...> &&rhs)
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

private:
  Tuple<TValues...> values_m{};
};

template<typename... Pairs>
TupleMap(Pairs&&...)->TupleMap<std::decay_t<Pairs>...>;

/*!
 * \brief Equality operator for TupleMap
 * \relates TupleMap
 */
template <typename... TTags, typename... TValues, typename... UTags,
          typename... UValues>
constexpr auto operator==(TupleMap<Pair<TTags, TValues>...> const &lhs,
                          TupleMap<Pair<UTags, UValues>...> const &rhs)
    -> bool {
  static_assert(sizeof...(TTags) == sizeof...(UTags),
                "Sizes of tags should be the same.");

  static_assert((utility::is_in_the_pack_v<TTags, UTags...> && ...),
                "TTags and UTags should be same(order doesn't matter).");
  return ((get<TTags>(lhs) == get<TTags>(rhs)) && ...);
}

/*!
 * \brief Equality operator for TupleMap
 * \relates TupleMap
 */
template <typename... TTags, typename... TValues, typename... UTags,
          typename... UValues>
constexpr auto operator!=(TupleMap<Pair<TTags, TValues>...> const &lhs,
                          TupleMap<Pair<UTags, UValues>...> const &rhs)
    -> bool {
  static_assert(sizeof...(TTags) == sizeof...(UTags),
                "Sizes of tags should be the same.");

  static_assert((utility::is_in_the_pack_v<TTags, UTags...> && ...),
                "TTags and UTags should be same(order doesn't matter).");
  return ((get<TTags>(lhs) != get<TTags>(rhs)) && ...);
}


template <typename Tag, typename... Tags, typename... Values>
struct TupleMapElement<Tag, TupleMap<Pair<Tags, Values>...>> {
  using Type = utility::ElementType< utility::index_of<Tag,Tags...> , Values...>;
};


template<typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<Pair<UTags, UArgs>...> const& map) -> TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>> const& {
  return get<utility::index_of<Tag, UTags...>>(map.values_m);
}

template<typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<Pair<UTags, UArgs>...>& map) -> TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>>& {
  return get<utility::index_of<Tag, UTags...>>(map.values_m);
}

template<typename Tag, typename... UTags, typename... UArgs>
constexpr auto get(TupleMap<Pair<UTags, UArgs>...>&& map) -> TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>>&& {
  return get<utility::index_of<Tag, UTags...>>(std::move(map.values_m));
}

template<typename Tag, typename... UTags, typename... UArgs>
constexpr TupleMapElementType<Tag, TupleMap<Pair<UTags, UArgs>...>> const&& get(TupleMap<Pair<UTags, UArgs>...> const&& map) {
  get<utility::index_of<Tag, UTags...>>(std::move(map.values_m));
}



/*!
 * \brief   Creates TupleMap from list of pairs
 * \relates TupleMap
 */
template <typename... Pairs>
constexpr auto makeTupleMap(Pairs&&... pairs) {
  return TupleMap<std::decay_t<Pairs>...>(pairs.second...);
}

template <typename... Types> struct TupleSize<TupleMap<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

namespace impl {
template <std::size_t... indices, typename... AllTags, typename Maps>
constexpr auto tupleMapCatHelper(
    [[maybe_unused]] std::integer_sequence<std::size_t, indices...> indices_v,
    [[maybe_unused]] Tuple<AllTags...> tags_v, Maps &&maps) {
  static_assert(sizeof...(indices) == sizeof...(AllTags),
                "Sizeof indices should be equal to tags");
  return makeTupleMap(makePair(AllTags{}, get<AllTags>(get<indices>(maps)))...);
}
} // namespace impl

/*!
 * \brief   Concatenates multiple tupleMaps into one.
 * \attention All tags should be unique between maps.
 * \note      The order is preserved.
 * \relates TupleMap
 */
template <typename... Maps> constexpr auto tupleMapCat(Maps &&... maps) {

  using sequence = typename utility::IntegerSequenceCat<
      typename utility::IntegerSequenceRepeat<
          std::size_t, utility::index_of<Maps, Maps...>,
          tuple_size_v<std::decay_t<Maps>>>::Type...>::Type;

  return impl::tupleMapCatHelper(
      sequence(), tupleCat(typename std::decay_t<Maps>::TagsTuple()...),
      forwardAsTuple(std::forward<Maps>(maps)...));
}

/*!
 * \brief Creates new TupleMap that is a subset of given TupleMap.
 * \tparam SelectedTags Tags that will be inside new TupleMap
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... SelectedTags>
constexpr auto tupleMapSubMap(TupleMap<Pair<Tags, Values>...> const &old_map,
                              [[maybe_unused]] SelectedTags &&... tags) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<SelectedTags>, Tags...> && ...),
      "SelectedTags should be a subset of Tags");
  static_assert((!std::is_reference_v<Tags> && ...) &&
                    (!std::is_const_v<Tags> && ...) &&
                    (!std::is_volatile_v<Tags> && ...),
                "Tags should be non const non reference non volatile types");
  using OldType = std::decay_t<TupleMap<Pair<Tags, Values>...>>;
  using NewType = TupleMap<
      Pair<std::decay_t<SelectedTags>,
           TupleMapElementType<std::decay_t<SelectedTags>, OldType>>...>;
  return NewType(get<std::decay_t<SelectedTags>>(old_map)...);
}
/*!
 * \brief Creates new TupleMap that is a subset of given TupleMap.
 * \tparam SelectedTags Tags that will be inside new TupleMap
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... SelectedTags>
constexpr auto tupleMapSubMap(TupleMap<Pair<Tags, Values>...> &&old_map,
                              [[maybe_unused]] SelectedTags &&... tags) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<SelectedTags>, Tags...> && ...),
      "SelectedTags should be a subset of Tags");
  using OldType = std::decay_t<TupleMap<Pair<Tags, Values>...>>;
  using NewType = TupleMap<
      Pair<std::decay_t<SelectedTags>,
           TupleMapElementType<std::decay_t<SelectedTags>, OldType>>...>;
  return NewType(
      std::move(get<std::decay_t<SelectedTags>>(std::move(old_map)))...);
}

/*!
 * \brief  Creates new TupleMap from the given one and additional list of pairs
 * \tparam NewPairs List of pairs to insert.
 * \relates TupleMap
 */
template <typename OldMap, typename... NewPairs>
constexpr auto tupleMapInsertBack(OldMap &&old_map, NewPairs &&... pairs) {
  return tupleMapCat(std::forward<OldMap>(old_map),
                     std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)));
}

/*!
 * \brief  Creates new TupleMap from the given one and additional list of pairs
 * \tparam NewPairs List of pairs to insert.
 * \relates TupleMap
 */
template <typename OldMap, typename... NewPairs>
constexpr auto tupleMapInsertFront(OldMap &&old_map, NewPairs &&... pairs) {
  return tupleMapCat(
      std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)),
      std::forward<OldMap>(old_map));
}

/*!
 * \brief  Creates new TupleMap from the given one and additional list of pairs
 * \tparam NewPairs List of pairs to insert.
 * \relates TupleMap
 */
template <typename Position, typename OldMap, typename... NewPairs>
constexpr auto tupleMapInsert(OldMap &&old_map, NewPairs &&... pairs) {
  return tupleMapCat(
      std::move(makeTupleMap(std::forward<NewPairs>(pairs)...)),
      std::forward<OldMap>(old_map));
}


namespace impl {
template <typename NewTags, typename... Tags, typename... Values,
          std::size_t... indices>
constexpr auto tupleMapEraseHelper(
    TupleMap<Pair<Tags, Values>...> const &old_map,
    [[maybe_unused]] std::integer_sequence<std::size_t, indices...>
        indices_var) {
  return tupleMapSubMap(old_map, TupleElementType<indices, NewTags>()...);
}
template <typename NewTags, typename... Tags, typename... Values,
          std::size_t... indices>
constexpr auto tupleMapEraseHelper(
    TupleMap<Pair<Tags, Values>...> &&old_map,
    [[maybe_unused]] std::integer_sequence<std::size_t, indices...>
        indices_var) {
  return tupleMapSubMap(std::move(old_map),
                        TupleElementType<indices, NewTags>()...);
}
} // namespace impl

/*!
 * \brief  Creates new TupleMap without given tags
 * \tparam TagsToErase Tags that will be erased
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... TagsToErase>
constexpr auto tupleMapErase(TupleMap<Pair<Tags, Values>...> const &old_map,
                             [[maybe_unused]] TagsToErase &&... tagsToErase) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<TagsToErase>, Tags...> && ...),
      "TagsToErase should be in the Tags");

  using NewTagsTuple = TupleCatType<
      std::conditional_t<utility::is_in_the_pack_v<Tags, std::decay_t<TagsToErase>...>,
                         Tuple<>, Tuple<Tags>>...>;

  using Sequence =
      std::make_integer_sequence<std::size_t, tuple_size_v<NewTagsTuple>>;

  return impl::tupleMapEraseHelper<NewTagsTuple>(old_map, Sequence());
}

/*!
 * \brief  Creates new TupleMap without given tags
 * \tparam TagsToErase Tags that will be erased
 * \relates TupleMap
 */
template <typename... Tags, typename... Values, typename... TagsToErase>
constexpr auto tupleMapErase(TupleMap<Pair<Tags, Values>...> &&old_map,
                             [[maybe_unused]] TagsToErase &&... tagsToErase) {
  static_assert(
      (utility::is_in_the_pack_v<std::decay_t<TagsToErase>, Tags...> && ...),
      "TagsToErase should be in the Tags");

  using NewTagsTuple = TupleCatType<
      std::conditional_t<utility::is_in_the_pack_v<Tags, std::decay_t<TagsToErase>...>,
          Tuple<>, Tuple<Tags>>...>;

  using Sequence =
  std::make_integer_sequence<std::size_t, tuple_size_v<NewTagsTuple>>;

  return impl::tupleMapEraseHelper<NewTagsTuple>(std::move(old_map), Sequence());
}

} // namespace CxxPlugins