namespace CxxPlugins::impl {


template <typename... TArgs>
using AlignmentsOf = std::integer_sequence<std::size_t, alignof(TArgs)...>;

template <typename... TArgs>
using SizesOf = std::integer_sequence<std::size_t, sizeof(TArgs)...>;

template <std::size_t CurrentOffset, typename T>
using RequiredOffset =
    std::integral_constant<std::size_t,
                           (CurrentOffset % alignof(T) == 0
                                ? CurrentOffset
                                : (CurrentOffset / alignof(T) + 1) *
                                      alignof(T))>; // round up

/*
 * First, Second, Third
 *
 * size = required_offset<required_offset<sizeof(First), Second> +
 * sizeof(Second), Third>;
 *
 *
 *
 */

template <std::size_t current, typename T>
static constexpr std::size_t my_offset = RequiredOffset<current, T>::value +
                                         sizeof(T);

template <typename T, T... values, T to_append>
constexpr auto append(std::integer_sequence<T, values...> /*unused*/,
                      std::integral_constant<T, to_append> /*unused*/) {
  return std::integer_sequence<T, values..., to_append>{};
}

template <typename T, T... values, T... to_append>
constexpr auto append(std::integer_sequence<T, values...> /*unused*/,
                      std::integer_sequence<T, to_append...> /*unused*/) {
  return std::integer_sequence<T, values..., to_append...>{};
}

template <typename... TArgs> struct AlignedTupleStorageSize {
  using sizes = SizesOf<TArgs...>;
  using alignments = AlignmentsOf<TArgs...>;
};

template <std::size_t previous_offset, typename... TArgs> struct AlignedOffsets;

template <std::size_t previous_offset, typename CurrentType, typename... Rest>
struct AlignedOffsets<previous_offset, CurrentType, Rest...> {

  static constexpr std::size_t current_offset =
      RequiredOffset<previous_offset, CurrentType>::value;

  using Type = decltype(
      append(std::integer_sequence<std::size_t, current_offset>{},
             typename AlignedOffsets<current_offset + sizeof(CurrentType),
                                     Rest...>::Type{}));
};

template <std::size_t previous_offset> struct AlignedOffsets<previous_offset> {
  using Type = std::integer_sequence<std::size_t>;
};

template <typename... TArgs>
using AlignedOffsetsSequence = typename AlignedOffsets<0, TArgs...>::Type;

//! \attention For internal usage only!
//! \brief Wrapper for reference types
//! (we can't construct references with placement new)
template <typename U> struct PackedTupleRefWrapper {
  static_assert(std::is_reference_v<U>, "U should be a reference");
  constexpr explicit PackedTupleRefWrapper(U ref) noexcept : reference_m(ref) {}
  U reference_m;
};

template <typename U>
using PackedTupleInnerType =
    std::conditional_t<std::is_reference_v<U>, PackedTupleRefWrapper<U>, U>;

template <typename... Ts> struct PackedTupleImpl;

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &
getInner(PackedTupleImpl<Us...> const &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &
getInner(PackedTupleImpl<Us...> &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &&
getInner(PackedTupleImpl<Us...> &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &&
getInner(PackedTupleImpl<Us...> const &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> const &
get(PackedTupleImpl<Us...> const &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> &
get(PackedTupleImpl<Us...> &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> &&
get(PackedTupleImpl<Us...> &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> const &&
get(PackedTupleImpl<Us...> const &&tuple) noexcept;




template <typename... Ts>
struct PackedTupleImpl {
public:
  template <typename U> using InnerType = PackedTupleInnerType<U>;

  // Noexcept specifiers
  static constexpr bool is_nothrow_default_ctor =
      (std::is_nothrow_default_constructible_v<InnerType<Ts>> && ...);
  static constexpr bool is_nothrow_copy_ctor =
      (std::is_nothrow_copy_constructible_v<InnerType<Ts>> && ...);
  static constexpr bool is_nothrow_move_ctor =
      (std::is_nothrow_move_constructible_v<InnerType<Ts>> && ...);
  template <typename... Args>
  static constexpr bool is_nothrow_ctor =
      (std::is_nothrow_constructible_v<InnerType<Ts>, Args> && ...);
  static constexpr bool is_nothrow_dtor =
      (std::is_nothrow_destructible_v<InnerType<Ts>> && ...);
  static constexpr auto sequence = std::index_sequence_for<Ts...>{};

  // Sizes and offsets

  using Offsets = AlignedOffsetsSequence<InnerType<Ts>...>;
  using Sizes = SizesOf<InnerType<Ts>...>;

  template <bool enable = (sizeof...(Ts) > 0),
            std::enable_if_t<enable, int> = 0>
  static constexpr std::size_t calculatedSize() {
    return decltype(utility::sequenceAt<sizeof...(Ts) - 1>(Offsets{}))::value +
           decltype(utility::sequenceAt<sizeof...(Ts) - 1>(Sizes{}))::value;
  }

  template <bool enable = (sizeof...(Ts) == 0),
            std::enable_if_t<enable, unsigned> = 1>
  static constexpr std::size_t calculatedSize() {
    return 0;
  }

  // calculated_size can be equal to 0, but array size can't be 0
  static constexpr std::size_t size =
      calculatedSize() != 0 ? calculatedSize() : 1;

  static constexpr std::size_t max_alignment =
      std::max({alignof(char), alignof(Ts)...});

  // Friends
  template <std::size_t I, typename... Us>
  friend constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &
  getInner(PackedTupleImpl<Us...> const &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &
  getInner(PackedTupleImpl<Us...> &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &&
  getInner(PackedTupleImpl<Us...> &&tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &&
  getInner(PackedTupleImpl<Us...> const &&tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr utility::ElementType<I, Us...> const &
  get(PackedTupleImpl<Us...> const &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr utility::ElementType<I, Us...> &
  get(PackedTupleImpl<Us...> &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr utility::ElementType<I, Us...> &&
  get(PackedTupleImpl<Us...> &&tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr utility::ElementType<I, Us...> const &&
  get(PackedTupleImpl<Us...> const &&tuple) noexcept;

  ~PackedTupleImpl() noexcept(is_nothrow_dtor) { destructor(sequence); }

  constexpr PackedTupleImpl() noexcept(is_nothrow_default_ctor) {
    defaultCtor(sequence);
  }

  template <bool requirement = sizeof...(Ts) >= 1 &&
                               (std::is_copy_constructible_v<InnerType<Ts>> &&
                                ...),
            std::enable_if_t<requirement, bool> = true>
  constexpr explicit PackedTupleImpl(Ts const &... args) noexcept(
      is_nothrow_copy_ctor) {
    forwardCtor(sequence, args...);
  }

  template <typename... Us,
            bool requirement = (sizeof...(Us) == sizeof...(Ts) &&
                                sizeof...(Us) >= 1 &&
                                (std::is_constructible_v<Ts, Us &&> && ...)),
            std::enable_if_t<requirement, bool> = true>
  constexpr explicit PackedTupleImpl(Us &&... args) noexcept(
      is_nothrow_ctor<Us...>) {
    forwardCtor(sequence, std::forward<Us>(args)...);
  }

  template <
      typename... Us,
      bool requirements =
          (sizeof...(Ts) == sizeof...(Us) &&
           (sizeof...(Ts) != 1 ||
            ((!std::is_convertible_v<Tuple<Us...> const &, InnerType<Ts>> &&
              ...) &&
             (!std::is_constructible_v<InnerType<Ts>, Tuple<Us...> const &> &&
              ...) &&
             (!std::is_same_v<Ts, Us> && ...)))),
      std::enable_if_t<requirements, bool> = true>
  constexpr explicit PackedTupleImpl(Tuple<Us...> const &other) noexcept(
      is_nothrow_ctor<Us const &...>) {
    conversionCtor(sequence, other);
  }

  template <
      typename... Us,
      bool requirements =
          (sizeof...(Ts) == sizeof...(Us) &&
           (sizeof...(Ts) != 1 ||
            ((!std::is_convertible_v<Tuple<Us...>, InnerType<Ts>> && ...) &&
             (!std::is_constructible_v<InnerType<Ts>, Tuple<Us...>> && ...) &&
             (!std::is_same_v<Ts, Us> && ...)))),
      std::enable_if_t<requirements, bool> = true>
  constexpr explicit PackedTupleImpl(Tuple<Us...> &&other) noexcept(
      is_nothrow_ctor<Us &&...>) {
    conversionCtor(sequence, std::move(other));
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            std::enable_if_t<size_requirement, bool> = true>
  constexpr explicit PackedTupleImpl(Pair<U1, U2> const &p) noexcept(
      is_nothrow_ctor<U1 const &, U2 const &>) {
    conversionCtor(p);
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            std::enable_if_t<size_requirement, bool> = true>
  constexpr explicit PackedTupleImpl(Pair<U1, U2> &&p) noexcept(
      is_nothrow_ctor<U1 &&, U2 &&>) {
    conversionCtor(std::move(p));
  }

  constexpr explicit PackedTupleImpl(
      PackedTupleImpl<Ts...> const &other) noexcept(is_nothrow_copy_ctor) {
    copyCtor(sequence, other);
  }
  constexpr explicit PackedTupleImpl(PackedTupleImpl<Ts...> &&other) noexcept(
      is_nothrow_move_ctor) {
    moveCtor(sequence, std::move(other));
  }

private:
  template <std::size_t I> auto unsafeAt() -> void * {
    return &data_m[utility::sequenceAt<I>(Offsets{})];
  }

  template <std::size_t I>[[nodiscard]] auto unsafeAt() const -> const void * {
    return &data_m[utility::sequenceAt<I>(Offsets{})];
  }

  template <std::size_t... indices>
  constexpr void destructor(
      std::integer_sequence<std::size_t,
                            indices...> /*unused*/) noexcept(is_nothrow_dtor) {
    auto dtor = [](auto &var) {
      using type = std::decay_t<decltype(var)>;
      var.~type();
    };
    (dtor(getInner<indices>(*this)), ...);
  }

  template <std::size_t... indices>
  constexpr void
  defaultCtor(std::integer_sequence<
              std::size_t,
              indices...> /*unused*/) noexcept(is_nothrow_default_ctor) {
    (new (unsafeAt<indices>()) InnerType<Ts>(), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void copyCtor(std::index_sequence<indices...> /*unused*/,
                          PackedTupleImpl<Us...> const
                              &rhs) noexcept(is_nothrow_ctor<Us const &...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(getInner<indices>(rhs)), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  moveCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
           PackedTupleImpl<Us...> &&rhs) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>())
         InnerType<Ts>(std::forward<Us>(getInner<indices>(rhs))),
     ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  forwardCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
              Us &&... vals) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(std::forward<Us>(vals)), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void conversionCtor(
      std::integer_sequence<std::size_t, indices...> /*unused*/,
      Tuple<Us...> const &other) noexcept(is_nothrow_ctor<Us const &...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(get<indices>(other)), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  conversionCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
                 Tuple<Us...> &&other) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>()) Ts(std::forward<Us>(get<indices>(other))), ...);
  }

  template <typename U1, typename U2>
  constexpr void conversionCtor(Pair<U1, U2> const &other) noexcept(
      is_nothrow_ctor<U1 const &, U2 const &>) {
    new (unsafeAt<0>()) InnerType<utility::ElementType<0, Ts...>>(other.first);
    new (unsafeAt<1>()) InnerType<utility::ElementType<1, Ts...>>(other.second);
  }

  template <typename U1, typename U2>
  constexpr void
  conversionCtor(Pair<U1, U2> &&other) noexcept(is_nothrow_ctor<U1 &&, U2 &&>) {
    new (unsafeAt<0>()) InnerType<utility::ElementType<0, Ts...>>(
        std::forward<U1>(other.first));
    new (unsafeAt<1>()) InnerType<utility::ElementType<1, Ts...>>(
        std::forward<U2>(other.second));
  }
  alignas(max_alignment) char data_m[size];
};

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> const &
get(const PackedTupleImpl<Us...> &tuple) noexcept {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return getInner<I>(tuple).reference_m;
  } else {
    return getInner<I>(tuple);
  }
}
template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> &
get(PackedTupleImpl<Us...> &tuple) noexcept {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return getInner<I>(tuple).reference_m;
  } else {
    return getInner<I>(tuple);
  }
}

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> &&
get(PackedTupleImpl<Us...> &&tuple) noexcept {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return getInner<I>(tuple).reference_m;
  } else {
    return std::move(getInner<I>(tuple));
  }
}

template <std::size_t I, typename... Us>
constexpr utility::ElementType<I, Us...> const &&
get(PackedTupleImpl<Us...> const &&tuple) noexcept {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return getInner<I>(tuple).reference_m;
  } else {
    return std::move(getInner<I>(tuple));
  }
}

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &
getInner(PackedTupleImpl<Us...> const &tuple) noexcept {
  using PointerType =
      utility::ElementType<I, PackedTupleInnerType<Us>...> const *;
  return *static_cast<PointerType>(tuple.template unsafeAt<I>());
}

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &
getInner(PackedTupleImpl<Us...> &tuple) noexcept {
  using PointerType = utility::ElementType<I, PackedTupleInnerType<Us>...> *;
  return *static_cast<PointerType>(tuple.template unsafeAt<I>());
}

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> &&
getInner(PackedTupleImpl<Us...> &&tuple) noexcept {
  using Type = utility::ElementType<I, PackedTupleInnerType<Us>...>;
  using PointerType = Type *;
  return std::forward<Type>(
      *static_cast<PointerType>(tuple.template unsafeAt<I>()));
}

template <std::size_t I, typename... Us>
constexpr PackedTupleInnerType<utility::ElementType<I, Us...>> const &&
getInner(PackedTupleImpl<Us...> const &&tuple) noexcept {
  using Type = utility::ElementType<I, PackedTupleInnerType<Us>...>;
  using PointerType = Type *;

  return std::forward<Type>(
      *static_cast<PointerType>(tuple.template unsafeAt<I>()));
}

} // namespace CxxPlugins::impl