#include <utility>

namespace utility {

template <typename Return, typename... Args>
struct FunctionTraits<Return(Args...)> {
  using ReturnType = Return;
  using ArgsTuple = std::tuple<Args...>;
  static constexpr bool is_method = false;
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

template <typename Return, typename... Args, typename Class>
struct SignatureToFunctionPointerImpl<Return(Args...), Class> {
  using type = Return (Class::*)(Args...);
};

template <typename Return, typename... Args, typename Class>
struct SignatureToFunctionPointerImpl<Return(Args...) const, Class> {
  using type = Return (Class::*)(Args...) const;
};

template <auto method, typename InputT> struct TrampolineGeneratorImpl;

template <typename Class, typename Return, typename... Args,
          Return (Class::*ptr)(Args...), typename InputT>
struct TrampolineGeneratorImpl<ptr, InputT> {
  static constexpr FunctionPointer<Return(InputT *, Args...)> value =
      [](InputT *input_p, Args... args) -> Return {
    auto obj_p = static_cast<Class *>(input_p);
    return (obj_p->*ptr)(std::forward<Args>(args)...);
  };
};

template <typename Class, typename Return, typename... Args,
          Return (Class::*ptr)(Args...) const, typename InputT>
struct TrampolineGeneratorImpl<ptr, InputT> {
  static constexpr FunctionPointer<Return(const InputT *, Args...)> value =
      [](const InputT *input_p, Args... args) -> Return {
    const auto* obj_p = static_cast<const Class *>(input_p);
    return (obj_p->*ptr)(std::forward<Args>(args)...);
  };
};

} // namespace impl

template <auto method> constexpr auto castMethodToFunction() {
  using Traits = FunctionTraits<decltype(method)>;
  static_assert(Traits::is_method, "Variable passed in should be a method");

  return generateTrampoline<method, typename Traits::ClassType>();
}

template <typename Signature, typename Class, FunctionPointer<Signature, Class> method>
constexpr auto castMethodToFunction() {
  return generateTrampoline<method, Class>();
}

template <auto method, typename InputT> constexpr auto generateTrampoline() {
  using Traits = FunctionTraits<decltype(method)>;
  static_assert(Traits::is_method, "Variable passed in should be a method");
  return impl::TrampolineGeneratorImpl<method, InputT>::value;
}

template <typename Signature, typename Class,
    FunctionPointer<Signature, Class> method, typename InputT>
constexpr auto generateTrampoline() {
  return generateTrampoline<method, InputT>();
}

} // namespace utility