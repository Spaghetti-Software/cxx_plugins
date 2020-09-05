/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    packed_tuple_tests.cpp
 * \author  Andrey Ponomarev
 * \date    26 May 2020
 * \brief
 * Tests for PackedTuple
 *
 * \details
 * ## List of tests
 * ### Member functions
 *   + ☑ Regular constructors
 *        + ☑ Default constructor
 *        + ☑ Direct constructor
 *        + ☑ Converting constructor
 *        + ☑ Converting copy-constructor
 *        + ☑ Converting move-constructor
 *        + ☑ Pair copy constructor
 *        + ☑ Pair move Constructor
 *        + ☑ Copy constructor
 *        + ☑ Move constructor
 *   + ☐ `uses_allocator` constructors
 *        + ☐ Default constructor
 *        + ☐ Direct constructor
 *        + ☐ Converting constructor
 *        + ☐ Converting copy-constructor
 *        + ☐ Converting move-constructor
 *        + ☐ Pair copy constructor
 *        + ☐ Pair move Constructor
 *        + ☐ Copy constructor
 *        + ☐ Move constructor
 *   + ☑ Assignment operators
 *        + ☑ Copy assignment operator
 *        + ☑ Move assignment operator
 *        + ☑ Conversion copy assignment operator
 *        + ☑ Conversion move assignment operator
 *        + ☑ Pair copy assignment operator
 *        + ☑ Pair move assignment operator
 *   + ☑ swap
 * ### Non-member functions
 *   + ☑ apply
 *   + ☑ makeTuple
 *   + ☑ tie
 *   + ☑ forwardAsTuple
 *   + ☑ tupleCat
 *   + ☑ get
 *   + ☐ Comparison operators // All except three-way are kind of tested
 *        + ☐ Three-way comparison
 *        + ☐ Equality operator
 *        + ☐ Not-equality operator
 *        + ☐ Less operator
 *        + ☐ Less equal operator
 *        + ☐ Greater operator
 *        + ☐ Greater equal operator
 *   + ☑ swap
 * ### Helper classes
 *   + ☑ tuple_size (TupleSize)
 *   + ☑ tuple_element (TupleElement)
 *   + ☐ uses_allocator (UsesAllocator)
 *   + ☐ ignore (Ignore)
 * ### Non-standard extensions of tuple
 *   + ☑ tupleForEach
 *   + ☑ apply with multiple tuples
 *   + ☑ tupleForEach with multiple tuples
 *
 */

#include <tuple/tuple.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
// clang defines __GNUC__ and has same attribute
#elif __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#error "Can't create NOINLINE macro (compiler is unknown)"
#endif

class TupleChecker {
public:
  virtual void dtor() = 0;
  virtual void defaultCtor() = 0;
  virtual void copyCtor() = 0;
  virtual void moveCtor() = 0;
  virtual void conversionCopyCtor() = 0;
  virtual void conversionMoveCtor() = 0;
  virtual void copyAssignment() = 0;
  virtual void moveAssignment() = 0;
  virtual void conversionCopyAssignment() = 0;
  virtual void conversionMoveAssignment() = 0;
  virtual ~TupleChecker() = default;
};

inline void a() {
  std::tuple<> e;
  std::tuple<> e0;
  e0 = e;
}

// GCC and clang complain about google's macros, so I suppressed warnings
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

class MockTupleChecker : public TupleChecker {
public:
  MOCK_METHOD(void, dtor, (), (override));
  MOCK_METHOD(void, defaultCtor, (), (override));
  MOCK_METHOD(void, copyCtor, (), (override));
  MOCK_METHOD(void, moveCtor, (), (override));
  MOCK_METHOD(void, conversionCopyCtor, (), (override));
  MOCK_METHOD(void, conversionMoveCtor, (), (override));
  MOCK_METHOD(void, copyAssignment, (), (override));
  MOCK_METHOD(void, moveAssignment, (), (override));
  MOCK_METHOD(void, conversionCopyAssignment, (), (override));
  MOCK_METHOD(void, conversionMoveAssignment, (), (override));
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template <typename T> class ImplicitData {
public:
  ~ImplicitData() { checker_p_m->dtor(); }

  ImplicitData() { checker_p_m->defaultCtor(); }

  ImplicitData(ImplicitData const &rhs) : value_m(rhs.value_m) {
    checker_p_m->copyCtor();
  }
  ImplicitData(ImplicitData &&rhs) : value_m(rhs.value_m) {
    checker_p_m->moveCtor();
  }

  ImplicitData(T const &rhs) : value_m(rhs) {
    checker_p_m->conversionCopyCtor();
  }
  ImplicitData(T &&rhs) noexcept : value_m(rhs) {
    checker_p_m->conversionMoveCtor();
  }

  ImplicitData &operator=(ImplicitData const &rhs) {
    value_m = rhs.value_m;
    checker_p_m->copyAssignment();
    return *this;
  }
  ImplicitData &operator=(ImplicitData &&rhs) {
    value_m = rhs.value_m;
    checker_p_m->moveAssignment();
    return *this;
  }

  ImplicitData &operator=(T const &rhs) {
    value_m = rhs;
    checker_p_m->conversionCopyAssignment();
    return *this;
  }
  ImplicitData &operator=(T &&rhs) {
    value_m = rhs;
    checker_p_m->conversionMoveAssignment();
    return *this;
  }

  static void setChecker(TupleChecker *ptr) { checker_p_m = ptr; }
  static void resetChecker() { checker_p_m = nullptr; }

  T value_m = {};

#if defined(__cpp_lib_three_way_comparison)
  constexpr std::strong_ordering
  operator<=>(ImplicitData const &) const = default;
  constexpr std::strong_ordering operator<=>(T const &rhs) const {
    return value_m <=> rhs;
  }
#endif

private:
  struct Dummy {};

  static TupleChecker *checker_p_m;
};

#if !defined(__cpp_lib_three_way_comparison)
template <typename T>
constexpr bool operator==(ImplicitData<T> const &lhs,
                          ImplicitData<T> const &rhs) {
  return lhs.value_m == rhs.value_m;
}
template <typename T>
constexpr bool operator==(ImplicitData<T> const &lhs, T const &rhs) {
  return lhs.value_m == rhs;
}
template <typename T>
constexpr bool operator==(T const &lhs, ImplicitData<T> const &rhs) {
  return lhs == rhs.value_m;
}
#endif

template <typename T> class ExplicitData {
public:
  ~ExplicitData() { checker_p_m->dtor(); }

  template <bool enable = std::is_default_constructible_v<T>,
      std::enable_if_t<enable, bool> = true>
  explicit ExplicitData() : value_m() {
    checker_p_m->defaultCtor();
  }

  explicit ExplicitData(ExplicitData const &rhs) : value_m(rhs.value_m) {
    checker_p_m->copyCtor();
  }

  explicit ExplicitData(ExplicitData &&rhs) : value_m(rhs.value_m) {
    checker_p_m->moveCtor();
  }

  explicit ExplicitData(T const &rhs) noexcept : value_m(rhs) {
    checker_p_m->conversionCopyCtor();
  }

  explicit ExplicitData(T &&rhs) noexcept : value_m(rhs) {
    checker_p_m->conversionMoveCtor();
  }

  ExplicitData &operator=(ExplicitData const &rhs) {
    value_m = rhs.value_m;
    checker_p_m->copyAssignment();
    return *this;
  }
  ExplicitData &operator=(ExplicitData &&rhs) {
    value_m = rhs.value_m;
    checker_p_m->moveAssignment();
    return *this;
  }

  ExplicitData &operator=(T const &rhs) {
    value_m = rhs;
    checker_p_m->conversionCopyAssignment();
    return *this;
  }

  template <bool enable = !std::is_reference_v<T>,
      std::enable_if_t<enable, bool> = true>
  ExplicitData &operator=(T &&rhs) {
    value_m = rhs;
    checker_p_m->conversionMoveAssignment();
    return *this;
  }

  T value_m;

  static void setChecker(TupleChecker *checker) { checker_p_m = checker; }
  static void resetChecker() { checker_p_m = nullptr; }

private:
  static TupleChecker *checker_p_m;
};
template <typename T>
constexpr bool operator==(ExplicitData<T> const &lhs,
                          ExplicitData<T> const &rhs) {
  return lhs.value_m == rhs.value_m;
}
template <typename T>
constexpr bool operator==(ExplicitData<T> const &lhs, T const &rhs) {
  return lhs.value_m == rhs;
}
template <typename T>
constexpr bool operator==(T const &lhs, ExplicitData<T> const &rhs) {
  return lhs == rhs.value_m;
}

template <typename T> struct IsImplicitData : std::false_type {};
template <typename T> struct IsExplicitData : std::false_type {};
template <typename T>
struct IsImplicitData<ImplicitData<T>> : std::true_type {};
template <typename T>
struct IsExplicitData<ExplicitData<T>> : std::true_type {};

template <typename T>
static constexpr bool is_implicit_data_v = IsImplicitData<T>::value;
template <typename T>
static constexpr bool is_explicit_data_v = IsExplicitData<T>::value;

template <typename T> struct UnderlyingType;
template <typename T> struct UnderlyingType<ImplicitData<T>> {
  using Type = T;
};
template <typename T> struct UnderlyingType<ExplicitData<T>> {
  using Type = T;
};

template <typename T> using UnderlyingTypeT = typename UnderlyingType<T>::Type;
template <typename T>
using DecayedUnderlyingTypeT = std::decay_t<UnderlyingTypeT<T>>;

template <typename T> TupleChecker *ImplicitData<T>::checker_p_m = nullptr;
template <typename T> TupleChecker *ExplicitData<T>::checker_p_m = nullptr;

template <typename T> class PackedTupleTests;

struct DummyAny {
  template <typename... Ts> constexpr DummyAny(Ts &&... /*unused*/) noexcept {}
};

template <typename T, T lhs_val, T rhs_val>
constexpr auto operator+(std::integral_constant<T, lhs_val>,
                         std::integral_constant<T, rhs_val>) {
  return std::integral_constant<T, lhs_val + rhs_val>();
}

template <typename T,
    std::enable_if_t<std::is_floating_point_v<T> || std::is_integral_v<T>,
        bool> = true>
using uniform_distribution =
std::conditional_t<std::is_floating_point_v<T>,
    std::uniform_real_distribution<T>,
    std::uniform_int_distribution<T>>;

template <typename T> struct RandomGenerator {
  T operator()() { return distribution_m(random_engine_m); }

private:
  std::random_device device_m;
  std::minstd_rand random_engine_m = std::minstd_rand(device_m());

  using type = std::conditional_t<std::is_same_v<T, char>, int, T>;

  uniform_distribution<type> distribution_m = uniform_distribution<type>(
      std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
};

template <typename... Ts>
class PackedTupleTests<plugins::Tuple<Ts...>> : public ::testing::Test {
public:
  static_assert(true &&
                ((is_implicit_data_v<Ts> || is_explicit_data_v<Ts>)&&...),
                "Ts should be instances of ExplictData or ImplicitData");

  template <std::size_t i>
  using const_id = std::integral_constant<std::size_t, i>;

  static constexpr std::size_t elements_count = sizeof...(Ts);

  static constexpr std::size_t implicit_count =
      (const_id<0>() + ... +
          std::conditional_t<is_implicit_data_v<Ts>, const_id<1>, const_id<0>>());

  struct DummyAny {
    template <typename... Us>
    constexpr DummyAny(Us &&... /*unused*/) noexcept {}
  };

  static constexpr std::size_t explicit_count = elements_count - implicit_count;

  NOINLINE auto create(plugins::Tuple<Ts...> data) { return data; }

  NOINLINE void createNoReturn(plugins::Tuple<Ts...> /*unused*/) {}

  static constexpr auto sequence = std::index_sequence_for<Ts...>();

  void setStrictMock() {
    ((Ts::setChecker(&strict_mock_m)), ...);
    mock_m = &strict_mock_m;
  }
  void setNiceMock() {
    ((Ts::setChecker(&nice_mock_m)), ...);
    mock_m = &nice_mock_m;
  }

  void SetUp() override { setStrictMock(); }

  template <std::size_t index>
  void
  checkEqualityImpl(plugins::Tuple<Ts...> const &tuple,
                    std::tuple<DecayedUnderlyingTypeT<Ts>...> const &expected) {
    EXPECT_EQ(plugins::get<index>(tuple).value_m, std::get<index>(expected));
  }

  template <std::size_t... indices>
  void
  checkEquality(std::index_sequence<indices...> /*unused*/,
                plugins::Tuple<Ts...> const &tuple,
                std::tuple<DecayedUnderlyingTypeT<Ts>...> const &expected) {
    (checkEqualityImpl<indices>(tuple, expected), ...);
  }

  template <std::size_t index,
      std::enable_if_t<index >= 0 && sizeof...(Ts) >= 1, int> = 0>
  void checkEqualityImpl(plugins::Tuple<Ts...> const &tuple,
                         std::tuple<Ts...> const &expected) {
    EXPECT_EQ(plugins::get<index>(tuple).value_m,
              std::get<index>(expected).value_m);
  }

  template <std::size_t... indices,
      std::enable_if_t<sizeof...(indices) >= 1, int> = 0>
  void checkEquality(std::index_sequence<indices...> /*unused*/,
                     plugins::Tuple<Ts...> const &tuple,
                     std::tuple<Ts...> const &expected) {
    (checkEqualityImpl<indices>(tuple, expected), ...);
  }

  template <typename T> void testAlignmentImpl(T *member_p) {
    auto ptr_value = reinterpret_cast<std::uintptr_t>(member_p);
    EXPECT_EQ(ptr_value % alignof(T), 0) << "Alignment is not satisfied";
  }

  template <std::size_t... indices>
  void testAlignment(std::index_sequence<indices...> /*unused*/,
                     plugins::Tuple<Ts...> const &tuple) {
    (testAlignmentImpl(&plugins::get<indices>(tuple)), ...);
  }

  void testDefaultCtor() {
    using namespace plugins;
    ::testing::InSequence in_sequence;

    setStrictMock();

    if constexpr ((std::is_default_constructible_v<Ts> && ...)) {
      if constexpr (explicit_count == 0) {
        // Check if we can call default ctor implicitly
        EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count);
        createNoReturn({});
      }

      // Explicit ctor
      EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
      Tuple<Ts...> explicit_tuple;
      testAlignment(sequence, explicit_tuple);
      checkEquality(sequence, explicit_tuple,
                    std::make_tuple(DecayedUnderlyingTypeT<Ts>()...));
      EXPECT_CALL(*mock_m, dtor).Times(elements_count);
    } else {
      EXPECT_FALSE(std::is_default_constructible_v<Tuple<Ts...>>)
              << "PackedTuple shouldn't have default constructor.";
    }
  }

  template <std::size_t... indices>
  void testDirectCtorImplicitImpl(std::index_sequence<indices...> /*unused*/,
                                  std::tuple<Ts...> const &input_data) {
    EXPECT_CALL(*mock_m, copyCtor).Times(elements_count);
    EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
    auto data_copy = create({std::get<indices>(input_data)...});
    checkEquality(sequence, data_copy, input_data);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  template <std::size_t... indices, typename... Us>
  void
  testConvertingCtorImplicitImpl(std::index_sequence<indices...> /*unused*/,
                                 std::tuple<Us...> const &input_data) {
    EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
    EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
    auto data_copy = create({std::get<indices>(input_data)...});
    checkEquality(sequence, data_copy, input_data);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  template <std::size_t... indices>
  void testDirectCtorExplicitImpl(std::index_sequence<indices...> /*unused*/,
                                  std::tuple<Ts...> const &input_data) {
    EXPECT_CALL(*mock_m, copyCtor).Times(elements_count);
    plugins::Tuple<Ts...> data_copy(std::get<indices>(input_data)...);
    testAlignment(sequence, data_copy);
    checkEquality(sequence, data_copy, input_data);

    // Implicit tests move ctors, so let's check move here as well
    EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
    plugins::Tuple<Ts...> data_move(
        plugins::get<indices>(std::move(data_copy))...);
    checkEquality(sequence, data_copy, input_data);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  template <std::size_t... indices, typename... Us>
  void
  testConvertingCtorExplicitImpl(std::index_sequence<indices...> /*unused*/,
                                 std::tuple<Us...> const &input_data) {
    EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
    plugins::Tuple<Ts...> data_copy(std::get<indices>(input_data)...);
    testAlignment(sequence, data_copy);
    checkEquality(sequence, data_copy, input_data);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  void testDirectCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) >= 1 &&
                  (std::is_copy_constructible_v<Ts> && ...)) {

      // need 2 separate inputs, as UnderlyingType<Ts>... can contain references
      std::tuple<DecayedUnderlyingTypeT<Ts>...> input_data{
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>()()...};
      EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count); // tmp
      std::tuple<Ts...> tmp(input_data);

      if constexpr (explicit_count == 0) {
        testDirectCtorImplicitImpl(sequence, tmp);
      }

      // explicit ctor should work in implicit case as well
      testDirectCtorExplicitImpl(sequence, tmp);

      EXPECT_CALL(*mock_m, dtor).Times(elements_count); // tmp
    }
      // Need to check for >= 1, as otherwise default ctor will be selected and
      // true will be returned
    else if constexpr (sizeof...(Ts) >= 1) {
      bool is_constructible =
          std::is_constructible_v<Tuple<Ts...>, Ts const &...>;
      EXPECT_FALSE(is_constructible)
              << "PackedTuple should have no direct constructor";
    }
  }
  void testConvertingCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) >= 1 &&
                  (std::is_copy_constructible_v<Ts> && ...)) {

      // need 2 separate inputs, as UnderlyingType<Ts>... can contain references
      std::tuple<DecayedUnderlyingTypeT<Ts>...> input_data{
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>()()...};

      if constexpr (explicit_count == 0) {
        testConvertingCtorImplicitImpl(sequence, input_data);
      }

      // explicit ctor should work in implicit case as well
      testConvertingCtorExplicitImpl(sequence, input_data);
    }
      // Need to check for >= 1, as otherwise default ctor will be selected and
      // true will be returned
    else if constexpr (sizeof...(Ts) >= 1) {
      bool is_constructible =
          std::is_constructible_v<Tuple<Ts...>,
              DecayedUnderlyingTypeT<Ts> const &...>;
      EXPECT_FALSE(is_constructible)
              << "PackedTuple should have no conversion constructor";
    }
  }
  void testConvertingCopyCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) >= 1) {

      // need 2 separate inputs, as UnderlyingType<Ts>... can contain references
      Tuple<DecayedUnderlyingTypeT<Ts>...> tmp(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

      // implicit construction
      if constexpr (explicit_count == 0) {
        EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count);
        auto result = create(tmp);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count); // result
        EXPECT_EQ(result, tmp);
      }

      EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
      Tuple<Ts...> explicit_data(tmp);
      EXPECT_EQ(explicit_data, tmp);
      EXPECT_CALL(*mock_m, dtor).Times(elements_count);
    }
      // Need to check for >= 1, as otherwise default ctor will be selected and
      // true will be returned
    else if constexpr (sizeof...(Ts) >= 1) {
      bool is_constructible =
          std::is_constructible_v<Tuple<Ts...>,
              Tuple<DecayedUnderlyingTypeT<Ts>...> const &>;
      EXPECT_FALSE(is_constructible)
              << "PackedTuple should have no conversion copy constructor";
    }
  }
  void testConvertingMoveCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) >= 1) {

      Tuple<DecayedUnderlyingTypeT<Ts>...> expected(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);
      Tuple<DecayedUnderlyingTypeT<Ts>...> tmp0(expected);
      Tuple<DecayedUnderlyingTypeT<Ts>...> tmp1(expected);

      // implicit construction
      if constexpr (explicit_count == 0) {
        EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count);
        auto result = create(std::move(tmp0));
        EXPECT_EQ(result, expected);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count); // result
      }

      EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
      EXPECT_CALL(*mock_m, dtor).Times(elements_count);
      Tuple<Ts...> explicit_data(std::move(tmp1));
      EXPECT_EQ(explicit_data, expected);

    }
      // Need to check for >= 1, as otherwise default ctor will be selected and
      // true will be returned
    else if constexpr (sizeof...(Ts) >= 1) {
      bool is_constructible =
          std::is_constructible_v<Tuple<Ts...>,
              Tuple<DecayedUnderlyingTypeT<Ts>...> const &>;
      EXPECT_FALSE(is_constructible)
              << "PackedTuple should have no conversion move constructor";
    }
  }
  void testPairCopyCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) == 2) {

      std::pair<DecayedUnderlyingTypeT<Ts>...> expected(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

      std::pair<DecayedUnderlyingTypeT<Ts>...> tmp0(expected);
      std::pair<DecayedUnderlyingTypeT<Ts>...> tmp1(expected);

      // implicit construction
      if constexpr (explicit_count == 0) {
        EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count);
        auto result = create(tmp0);
        EXPECT_EQ(get<0>(result), expected.first);
        EXPECT_EQ(get<1>(result), expected.second);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count); // result
      }

      EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
      EXPECT_CALL(*mock_m, dtor).Times(elements_count);
      Tuple<Ts...> explicit_data(tmp1);
      EXPECT_EQ(get<0>(explicit_data), expected.first);
      EXPECT_EQ(get<1>(explicit_data), expected.second);
    }
  }
  void testPairMoveCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    if constexpr (sizeof...(Ts) == 2) {

      std::pair<DecayedUnderlyingTypeT<Ts>...> expected(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

      std::pair<DecayedUnderlyingTypeT<Ts>...> tmp0(expected);
      std::pair<DecayedUnderlyingTypeT<Ts>...> tmp1(expected);

      // implicit construction
      if constexpr (explicit_count == 0) {
        EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count);
        auto result = create(std::move(tmp0));
        EXPECT_EQ(get<0>(result), expected.first);
        EXPECT_EQ(get<1>(result), expected.second);
        EXPECT_CALL(*mock_m, dtor).Times(elements_count); // result
      }

      EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
      EXPECT_CALL(*mock_m, dtor).Times(elements_count);
      Tuple<Ts...> explicit_data(std::move(tmp1));
      EXPECT_EQ(get<0>(explicit_data), expected.first);
      EXPECT_EQ(get<1>(explicit_data), expected.second);
    }
  }
  void testCopyCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> initial{RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};

    EXPECT_CALL(*mock_m, copyCtor).Times(elements_count);
    auto copy = initial;
    EXPECT_EQ(copy, initial);

    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 2);
  }
  void testMoveCtor() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    Tuple<DecayedUnderlyingTypeT<Ts>...> input_data{
        RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};
    EXPECT_CALL(*mock_m, conversionCopyCtor).Times(elements_count);
    Tuple<Ts...> tmp(input_data);

    EXPECT_CALL(*mock_m, moveCtor).Times(elements_count);
    auto copy = std::move(tmp);
    EXPECT_EQ(copy, input_data);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 2);
  }
  void testCopyAssignment() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> initial{RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> another;
    EXPECT_CALL(*mock_m, copyAssignment).Times(elements_count);
    another = initial;
    EXPECT_EQ(another, initial);

    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 2);
  }
  void testMoveAssignment() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> initial{RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};
    EXPECT_CALL(*mock_m, copyCtor).Times(elements_count);
    Tuple<Ts...> tmp(initial);

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> another;
    EXPECT_CALL(*mock_m, moveAssignment).Times(elements_count);
    another = std::move(tmp);
    EXPECT_EQ(another, initial);

    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 3);
  }
  void testConversionCopyAssignment() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    Tuple<DecayedUnderlyingTypeT<Ts>...> initial{
        RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> another;
    EXPECT_CALL(*mock_m, conversionCopyAssignment).Times(elements_count);
    another = initial;
    EXPECT_EQ(another, initial);

    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }
  void testConversionMoveAssignment() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setStrictMock();

    Tuple<DecayedUnderlyingTypeT<Ts>...> initial{
        RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...};
    auto tmp = initial;

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> another;
    EXPECT_CALL(*mock_m, conversionMoveAssignment).Times(elements_count);
    another = std::move(tmp);
    EXPECT_EQ(another, initial);

    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  void testMemberSwap() {
    ::testing::InSequence in_sequence;
    using namespace plugins;

    setNiceMock();

    // ExplicitData has explicit move/copy constructors, which breaks swap
    if constexpr (explicit_count == 0) {
      Tuple<Ts...> expected0(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);
      Tuple<Ts...> expected1(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

      Tuple<Ts...> val0(expected0);
      Tuple<Ts...> val1(expected1);

      val0.swap(val1);

      EXPECT_EQ(val0, expected1);
      EXPECT_EQ(val1, expected0);
    }
  }

  void testAdlSwap() {
    ::testing::InSequence in_sequence;

    setNiceMock();

    // ExplicitData has explicit move/copy constructors, which breaks swap
    if constexpr (explicit_count == 0) {
      plugins::Tuple<Ts...> expected0(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);
      plugins::Tuple<Ts...> expected1(
          RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

      plugins::Tuple<Ts...> val0(expected0);
      plugins::Tuple<Ts...> val1(expected1);

      swap(val0, val1);

      EXPECT_EQ(val0, expected1);
      EXPECT_EQ(val1, expected0);
    }
  }

  void testMakeTuple() {
    using namespace plugins;

    bool is_same = std::is_same_v<Tuple<Ts...>,
        decltype(makeTuple(std::declval<Ts>()...))>;

    EXPECT_TRUE(is_same)
            << "makeTuple(Ts()...) is different from std::tuple<Ts...>";
  }

  void testApply() {
    using namespace plugins;

    //    if constexpr (elements_count >= 1) {

    setStrictMock();

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value;

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> expected(
        (std::numeric_limits<DecayedUnderlyingTypeT<Ts>>::max())...);

    // Empty tuples are always equal
    if constexpr (elements_count >= 1) {
      EXPECT_NE(value, expected);
    }
    auto fn = [this](auto &&... val) {
      EXPECT_CALL(*mock_m, conversionMoveAssignment).Times(elements_count);
      ((val = std::numeric_limits<
          DecayedUnderlyingTypeT<std::decay_t<decltype(val)>>>::max()),
          ...);
    };
    apply(fn, value);

    EXPECT_EQ(value, expected);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 2);
    //    }
  }

  void testMultiArgumentApply() {
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value0;
    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value1;

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> expected(
        (std::numeric_limits<DecayedUnderlyingTypeT<Ts>>::max())...);

    // Empty tuples are always equal
    if constexpr (elements_count >= 1) {
      EXPECT_NE(value0, expected);
      EXPECT_NE(value1, expected);
    }

    auto fn = [this](auto &&... val) {
      EXPECT_CALL(*mock_m, conversionMoveAssignment).Times(elements_count * 2);
      ((val = std::numeric_limits<
          DecayedUnderlyingTypeT<std::decay_t<decltype(val)>>>::max()),
          ...);
    };
    apply(fn, value0, value1);

    EXPECT_EQ(value0, expected);
    EXPECT_EQ(value1, expected);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 3);
  }

  void testForEach() {
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value;

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> expected(
        (std::numeric_limits<DecayedUnderlyingTypeT<Ts>>::max())...);

    if constexpr (elements_count >= 1) {
      EXPECT_NE(value, expected);
    }
    auto fn = [this](auto &&val) {
      EXPECT_CALL(*mock_m, conversionMoveAssignment);
      val = std::numeric_limits<
          DecayedUnderlyingTypeT<std::decay_t<decltype(val)>>>::max();
    };

    tupleForEach(fn, value);

    EXPECT_EQ(value, expected);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 2);
  }

  void testForEachMultiArgument() {
    using namespace plugins;

    setStrictMock();

    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value0;
    EXPECT_CALL(*mock_m, defaultCtor).Times(elements_count);
    Tuple<Ts...> value1;

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> expected(
        (std::numeric_limits<DecayedUnderlyingTypeT<Ts>>::max())...);

    if constexpr (elements_count >= 1) {
      EXPECT_NE(value0, expected);
      EXPECT_NE(value1, expected);
    }
    auto fn = [this](auto && val0, auto&& val1) {
      using type = std::decay_t<decltype(val0)>;
      bool are_same = std::is_same_v<decltype(val0), decltype(val1)>;

      EXPECT_TRUE(are_same) << "Tuples passed into for each have same types. "
                               "ForEach is implemented incorrectly";

      EXPECT_CALL(*mock_m, conversionMoveAssignment).Times(2);
      val0 = std::numeric_limits<DecayedUnderlyingTypeT<type>>::max();
      val1 = std::numeric_limits<DecayedUnderlyingTypeT<type>>::max();
    };

    tupleForEach(fn, value0, value1);

    EXPECT_EQ(value0, expected);
    EXPECT_EQ(value1, expected);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count * 3);
  }

  template <std::size_t... indices>
  auto tieImpl(plugins::Tuple<Ts...> &tuple,
               std::index_sequence<indices...> /*unused*/) {
    using namespace plugins;
    return tie(get<indices>(tuple)...);
  }

  void testTie() {
    using namespace plugins;

    setStrictMock();

    bool is_same = std::is_same_v<Tuple<Ts const &...>,
        decltype(tie(std::declval<Ts const &>()...))>;
    EXPECT_TRUE(is_same)
            << "tie(Ts()...) is different from std::tuple<Ts const &... > ";

    EXPECT_CALL(*mock_m, conversionMoveCtor).Times(elements_count);
    Tuple<Ts...> my_tuple(RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);

    auto tie_tuple = tieImpl(my_tuple, std::index_sequence_for<Ts...>());
    auto check_addresses = [](auto &lhs, auto &rhs) {
      EXPECT_EQ(&lhs, &rhs) << "Addresses are different.";
    };
    tupleForEach(check_addresses, my_tuple, tie_tuple);
    EXPECT_CALL(*mock_m, dtor).Times(elements_count);
  }

  void testTypeDeductionGuides() {
    using namespace plugins;
    setNiceMock();
    Tuple tuple0(Ts{}...);
    using type0 = decltype(tuple0);
    bool is_correct0 = std::is_same_v<type0, Tuple<Ts...>>;
    EXPECT_TRUE(is_correct0)
            << "Deduction guide Tuple tuple0(Ts{}...) didn't create Tuple<Ts...>";

    Tuple tuple1(RandomGenerator<DecayedUnderlyingTypeT<Ts>>{}()...);
    using type1 = decltype(tuple1);
    bool is_correct1 =
        std::is_same_v<type1, Tuple<DecayedUnderlyingTypeT<Ts>...>>;
    EXPECT_TRUE(is_correct1)
            << "Deduction guide Tuple tuple1(DecayedUnderlyingTypeT<Ts>...) didn't "
               "create Tuple<DecayedUnderlyingTypeT<Ts>...>";
  }

  void testStructuredBinding() {
    setNiceMock();
    plugins::Tuple tuple(Ts{}...);
    if constexpr (elements_count == 1) {
      auto &[member_ref] = tuple;
      EXPECT_EQ(&member_ref, &plugins::get<0>(tuple));
      auto [member_val] = tuple;
      EXPECT_NE(&member_val, &plugins::get<0>(tuple));
      EXPECT_EQ(member_val, plugins::get<0>(tuple));
    }
    if constexpr (elements_count == 2) {
      auto &[member_ref0, member_ref1] = tuple;
      EXPECT_EQ(&member_ref0, &plugins::get<0>(tuple));
      EXPECT_EQ(&member_ref1, &plugins::get<1>(tuple));

      auto [member_val0, member_val1] = tuple;

      EXPECT_NE(&member_val0, &plugins::get<0>(tuple));
      EXPECT_NE(&member_val1, &plugins::get<1>(tuple));

      EXPECT_EQ(member_val0, plugins::get<0>(tuple));
      EXPECT_EQ(member_val1, plugins::get<1>(tuple));

    }
    if constexpr (elements_count == 3) {
      auto &[member_ref0, member_ref1, member_ref2] = tuple;
      EXPECT_EQ(&member_ref0, &plugins::get<0>(tuple));
      EXPECT_EQ(&member_ref1, &plugins::get<1>(tuple));
      EXPECT_EQ(&member_ref2, &plugins::get<2>(tuple));


      auto [member_val0, member_val1, member_val2] = tuple;

      EXPECT_NE(&member_val0, &plugins::get<0>(tuple));
      EXPECT_NE(&member_val1, &plugins::get<1>(tuple));
      EXPECT_NE(&member_val2, &plugins::get<2>(tuple));


      EXPECT_EQ(member_val0, plugins::get<0>(tuple));
      EXPECT_EQ(member_val1, plugins::get<1>(tuple));
      EXPECT_EQ(member_val2, plugins::get<2>(tuple));
    }
  }

  void testTupleCat() {
    using namespace plugins;
    setNiceMock();
    Tuple<Ts...> t0;
    Tuple<Ts...> t1;
    Tuple<Ts...> t2;
    Tuple<Ts...> t3;

    auto cat0 = tupleCat(t0,t1);
    bool result = std::is_same_v<decltype(cat0), Tuple<Ts...,Ts...>>;
    EXPECT_TRUE(result);
    auto cat1 = tupleCat(t0,t1,t2);
    result = std::is_same_v<decltype(cat1), Tuple<Ts...,Ts...,Ts...>>;
    EXPECT_TRUE(result);
    result = std::is_same_v<decltype(cat1), decltype(tupleCat(cat0, std::declval<Tuple<Ts...>>()))>;
    EXPECT_TRUE(result);
    auto cat2 = tupleCat(t0,t1,t2,t3);
    result = std::is_same_v<decltype(cat2), Tuple<Ts...,Ts...,Ts...,Ts...>>;
    EXPECT_TRUE(result);
    result = std::is_same_v<decltype(cat2), decltype(tupleCat(cat0,cat0))>;
    EXPECT_TRUE(result);



  }

  void TearDown() override { (Ts::resetChecker(), ...); }

  MockTupleChecker *mock_m = nullptr;
  ::testing::StrictMock<MockTupleChecker> strict_mock_m = {};
  ::testing::NiceMock<MockTupleChecker> nice_mock_m = {};
};

TYPED_TEST_SUITE_P(PackedTupleTests);

TYPED_TEST_P(PackedTupleTests, DefaultConstructor) {
  TestFixture::testDefaultCtor();
}
TYPED_TEST_P(PackedTupleTests, DirectConstructor) {
  TestFixture::testDirectCtor();
}
TYPED_TEST_P(PackedTupleTests, ConvertingConstructor) {
  TestFixture::testConvertingCtor();
}
TYPED_TEST_P(PackedTupleTests, ConvertingCopyConstructor) {
  TestFixture::testConvertingCopyCtor();
}
TYPED_TEST_P(PackedTupleTests, ConvertingMoveConstructor) {
  TestFixture::testConvertingMoveCtor();
}
TYPED_TEST_P(PackedTupleTests, PairCopyConstructor) {
  TestFixture::testPairCopyCtor();
}
TYPED_TEST_P(PackedTupleTests, PairMoveConstructor) {
  TestFixture::testPairMoveCtor();
}
TYPED_TEST_P(PackedTupleTests, CopyConstructor) { TestFixture::testCopyCtor(); }
TYPED_TEST_P(PackedTupleTests, MoveConstructor) { TestFixture::testMoveCtor(); }

TYPED_TEST_P(PackedTupleTests, CopyAssignment) {
  TestFixture::testCopyAssignment();
}
TYPED_TEST_P(PackedTupleTests, MoveAssignment) {
  TestFixture::testMoveAssignment();
}
TYPED_TEST_P(PackedTupleTests, ConversionCopyAssignment) {
  TestFixture::testConversionCopyAssignment();
}
TYPED_TEST_P(PackedTupleTests, ConversionMoveAssignment) {
  TestFixture::testConversionMoveAssignment();
}

TYPED_TEST_P(PackedTupleTests, MemberSwap) { TestFixture::testMemberSwap(); }

TYPED_TEST_P(PackedTupleTests, MakeTuple) { TestFixture::testMakeTuple(); }

TYPED_TEST_P(PackedTupleTests, AdlSwap) {
  TestFixture::testAdlSwap();
}

TYPED_TEST_P(PackedTupleTests, Apply) {
  TestFixture::testApply();
}

TYPED_TEST_P(PackedTupleTests, ApplyMulti) {
  TestFixture::testMultiArgumentApply();
}

TYPED_TEST_P(PackedTupleTests, ForEach) {
  TestFixture::testForEach();
}

TYPED_TEST_P(PackedTupleTests, ForEachMulti) {
  TestFixture::testForEachMultiArgument();
}
TYPED_TEST_P(PackedTupleTests, Tie) {
  TestFixture::testTie();
}
TYPED_TEST_P(PackedTupleTests, Cat) {
  TestFixture::testTupleCat();
}

TYPED_TEST_P(PackedTupleTests, DeductionGuides) {
  TestFixture::testTypeDeductionGuides();
}

TYPED_TEST_P(PackedTupleTests, StructuredBinding) {
  TestFixture::testStructuredBinding();
}



REGISTER_TYPED_TEST_SUITE_P(
    PackedTupleTests, DefaultConstructor, DirectConstructor,
    ConvertingConstructor, ConvertingCopyConstructor, ConvertingMoveConstructor,
    PairCopyConstructor, PairMoveConstructor, CopyConstructor, MoveConstructor,
    CopyAssignment, MoveAssignment, ConversionCopyAssignment,
    ConversionMoveAssignment, MemberSwap, MakeTuple, AdlSwap, Apply, ApplyMulti,
    ForEach, ForEachMulti, Tie, Cat, DeductionGuides, StructuredBinding);

// clang-format off
using EmptyType = plugins::Tuple<>;

using SimpleSingleTypes = ::testing::Types<
    plugins::Tuple<ImplicitData<int>>,
    plugins::Tuple<ExplicitData<int>>
>;

using SimpleMultiTypes = ::testing::Types<
    plugins::Tuple<ImplicitData<int>, ImplicitData<int>>,
    plugins::Tuple<ImplicitData<int>, ImplicitData<int>, ImplicitData<int>>,
    plugins::Tuple<ImplicitData<int>, ImplicitData<int>, ImplicitData<int>, ImplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int>, ExplicitData<int>, ExplicitData<int>>
>;

using MixedMultiTypes = ::testing::Types<
    plugins::Tuple<ImplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ImplicitData<int>, ExplicitData<int>, ImplicitData<int>>,
    plugins::Tuple<ImplicitData<int>, ExplicitData<int>, ImplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ImplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ImplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ImplicitData<int>, ExplicitData<int>, ImplicitData<int>>
>;

// Implicit and explicit Constructors are tested in types above
// All types bellow use Explicit constructors, as they have less overhead
// (implicit ctors use explicit ctors too)

using PaddedTypes = :: testing::Types<
    plugins::Tuple<ExplicitData<int>, ExplicitData<char>>,
    plugins::Tuple<ExplicitData<char>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<double>, ExplicitData<int>, ExplicitData<char>>,
    plugins::Tuple<ExplicitData<char>, ExplicitData<int>, ExplicitData<double>>,
    plugins::Tuple<ExplicitData<char>, ExplicitData<double>, ExplicitData<char>, ExplicitData<double>>,
    plugins::Tuple<ExplicitData<double>, ExplicitData<double>, ExplicitData<char>, ExplicitData<char>>
>;

// No Default constroctors
// No Copy/Move assignment
using ReferenceTypes = ::testing::Types<
    plugins::Tuple<ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int&>, ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int&>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int>, ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int&>, ExplicitData<int&>>,
    plugins::Tuple<ExplicitData<int&>, ExplicitData<int>, ExplicitData<int>>,
    plugins::Tuple<ExplicitData<int>, ExplicitData<int>, ExplicitData<int&>>
>;

// clang-format on
template <std::size_t i> struct PaddedStruct;

template <> struct PaddedStruct<0> {
  int val0_m;
  char val1_m;
};

static_assert(sizeof(plugins::Tuple<int, char>) == sizeof(PaddedStruct<0>),
              "Size is wrong");

template <> struct PaddedStruct<1> {
  char val0_m;
  int val1_m;
};

static_assert(sizeof(plugins::Tuple<char, int>) == sizeof(PaddedStruct<1>),
              "Size is wrong");

template <> struct PaddedStruct<2> {
  double val0_m;
  int val1_m;
  char val2_m;
};

static_assert(sizeof(plugins::Tuple<double, int, char>) ==
              sizeof(PaddedStruct<2>),
              "Size is wrong");

template <> struct PaddedStruct<3> {
  char val0_m;
  int val1_m;
  double val2_m;
};

static_assert(sizeof(plugins::Tuple<char, int, double>) ==
              sizeof(PaddedStruct<3>),
              "Size is wrong");

template <> struct PaddedStruct<4> {
  char val0_m;
  double val1_m;
  char val2_m;
  double val_3_m;
};

static_assert(sizeof(plugins::Tuple<char, double, char, double>) ==
              sizeof(PaddedStruct<4>),
              "Size is wrong");

template <> struct PaddedStruct<5> {
  double val0_m;
  double val1_m;
  char val2_m;
  char val_3_m;
};

static_assert(sizeof(plugins::Tuple<double, double, char, char>) ==
              sizeof(PaddedStruct<5>),
              "Size is wrong");




