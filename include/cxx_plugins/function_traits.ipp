#include <utility>

namespace utility {

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

template <auto method> struct MethodCastImpl;

template <typename Class, typename Return, typename... Args,
          Return (Class::*ptr)(Args...)>
struct MethodCastImpl<ptr> {
  static constexpr FunctionPointer<Return(Class *, Args...)> value =
      [](Class *obj_p, Args... args) {
        (obj_p->*ptr)(std::forward<Args>(args)...);
      };
};

template <typename Class, typename Return, typename... Args,
          Return (Class::*ptr)(Args...) const>
struct MethodCastImpl<ptr> {
  static constexpr FunctionPointer<Return(const Class *, Args...)> value =
      [](const Class *obj_p, Args... args) {
        (obj_p->*ptr)(std::forward<Args>(args)...);
      };
};

} // namespace impl

template <auto method> constexpr auto castMethodToFunction() {
  return impl::MethodCastImpl<method>::value;
}

} // namespace utility