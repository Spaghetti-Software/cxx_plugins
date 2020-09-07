
namespace plugins::traits {

template <typename Return, typename... Args>
struct FunctionTraits<Return(Args...)> {
  using ReturnType = Return;
  using ArgsTuple = std::tuple<Args...>;
  static constexpr bool is_method = false;
  static constexpr bool is_const = false;
};

template <typename Return, typename... Args>
struct FunctionTraits<Return(Args...)const> {
  using ReturnType = Return;
  using ArgsTuple = std::tuple<Args...>;
  static constexpr bool is_method = false;
  static constexpr bool is_const = true;
};

template <typename Return, typename... Args>
struct FunctionTraits<Return (*)(Args...)>
    : public FunctionTraits<Return(Args...)> {};

template <typename Class, typename Return, typename... Args>
struct FunctionTraits<Return (Class::*)(Args...)> {
  using ReturnType = Return;
  using ArgsTuple = std::tuple<Args...>;
  static constexpr bool is_method = true;
  static constexpr bool is_const = false;
  using ClassType = Class;
};

template <typename Class, typename Return, typename... Args>
struct FunctionTraits<Return (Class::*)(Args...) const> {
  using ReturnType = Return;
  using ArgsTuple = std::tuple<Args...>;
  static constexpr bool is_method = true;
  static constexpr bool is_const = true;
  using ClassType = Class;
};

namespace impl {

template <typename Return, typename... Args>
struct SignatureToFunctionPointerImpl<Return(Args...), void> {
  using type = Return (*)(Args...);
};

template <typename Return, typename... Args>
struct SignatureToFunctionPointerImpl<Return (*)(Args...), void>
    : public SignatureToFunctionPointerImpl<Return(Args...)> {};

template <typename Return, typename... Args, typename Class>
struct SignatureToFunctionPointerImpl<Return(Args...), Class> {
  using type = Return (Class::*)(Args...);
};

template <typename Return, typename... Args, typename Class>
struct SignatureToFunctionPointerImpl<Return(Args...) const, Class> {
  using type = Return (Class::*)(Args...) const;
};





template <typename T, bool IsClass> struct IsCallableImpl;

template <typename T> struct IsCallableImpl<T, true> {
private:
  using yes = char (&)[1];
  using no = char (&)[2];

  struct Fallback {
    void operator()();
  };
  struct Derived : T, Fallback {};

  template <typename U, U> struct Check;

  template <typename> static yes test(...);

  template <typename C>
  static no test(Check<void (Fallback::*)(), &C::operator()> *);

public:
  static constexpr bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

template <typename Return, typename... Args>
struct IsCallableImpl<Return (*)(Args...), false> {
  static constexpr bool value = true;
};

template <typename Return, typename... Args>
struct IsCallableImpl<Return(Args...), false> {
  static constexpr bool value = true;
};

template <typename T> struct IsCallableImpl<T, false> {
  static constexpr bool value = false;
};

} // namespace impl



} // namespace CxxPlugins::utility