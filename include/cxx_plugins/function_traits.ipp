
namespace plugins::utility {

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

template <auto function, typename InputT> struct TrampolineGeneratorImpl;

template <typename Class, typename Return, typename... Args,
          Return (Class::*method)(Args...), typename InputT>
struct TrampolineGeneratorImpl<method, InputT> {

  static constexpr Return fnImpl(InputT *input_p, Args... args) {
    auto obj_p = static_cast<Class *>(input_p);
    return (obj_p->*method)(std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
          Return (Class::*method)(Args...) const, typename InputT>
struct TrampolineGeneratorImpl<method, InputT> {

  static constexpr Return fnImpl(const InputT *input_p, Args... args) {
    const auto *obj_p = static_cast<const Class *>(input_p);
    return (obj_p->*method)(std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
    Return(*fn)(Class*, Args...), typename InputT>
struct TrampolineGeneratorImpl<fn, InputT> {

  static constexpr Return fnImpl(InputT *input_p, Args... args) {
    auto obj_p = static_cast<Class *>(input_p);
    return fn(obj_p, std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
    Return (*fn)(Class const*, Args...), typename InputT>
struct TrampolineGeneratorImpl<fn, InputT> {

  static constexpr Return fnImpl(const InputT *input_p, Args... args) {
    auto obj_p = static_cast<const Class *>(input_p);
    return fn(obj_p, std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
    Return(*fn)(Class*, Args...)>
struct TrampolineGeneratorImpl<fn, Class> {

  static constexpr auto value = fn;
};

template <typename Class, typename Return, typename... Args,
    Return (*fn)(Class const*, Args...)>
struct TrampolineGeneratorImpl<fn, Class> {
  static constexpr auto value = fn;
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

template <auto method> constexpr auto castMethodToFunction() {
  using Traits = FunctionTraits<decltype(method)>;
  static_assert(Traits::is_method, "Variable passed in should be a method");

  return generateTrampoline<method, typename Traits::ClassType>();
}

template <typename Signature, typename Class,
          FunctionPointer<Signature, Class> method>
constexpr auto castMethodToFunction() {
  return generateTrampoline<method, Class>();
}

template <auto function, typename InputT> constexpr auto generateTrampoline() {
  return impl::TrampolineGeneratorImpl<function, InputT>::value;
}

//template <typename Signature, typename Class,
//          FunctionPointer<Signature, Class> method, typename InputT>
//constexpr auto generateTrampoline() {
//  return generateTrampoline<method, InputT>();
//}

template <typename Signature,
    FunctionPointer<Signature,void> function, typename InputT>
constexpr auto generateTrampoline() {
  return generateTrampoline<function, InputT>();
}

} // namespace CxxPlugins::utility