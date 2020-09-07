# CxxPlugins

## Emphasis

> **Note:** library is still in development. For more examples use tests.
> Documentation can be generated if you pass `CXX_PLUGINS_BUILD_DOCUMENTATION=ON` to cmake 
> or `-o enable_documentation=True` to conan 

CxxPlugins is a library that aims to simplify creation of plugin systems.
It consists of:

+ `Polymorphic` and `PolymorphicPtr` templates, 
   that allow storing polymorphic objects without inheritance.
+ `PolymorphicAllocator` that is implemented using polymorphic and has the same purpose 
   as `std::pmr::polymorphic_allocator`.
   + `Mallocator` - default polymorphic allocator memory resource. Uses malloc to allocate/deallocate objects.
   + `StackAllocator` - memory resource that allocates objects on the stack.
+ Plugin configurator
    + Customize json file configuration with constexpr function calls
    + Load configurations and dlls automatically.
    
## Requirements

+ [Conan](https://conan.io/)
+ [CMake](https://cmake.org/) 3.15 and higher
+ [doxygen](https://www.doxygen.nl/download.html) (for generating documentation)
+ C++17 and higher


## Dependencies

+ boost/1.73.0
+ rapidjson/1.1.0
+ fmt/6.2.1
+ gtest/1.10.0 (for tests only)


## Build

```sh
$ mkdir build
$ cd build
$ conan install .. -o enable_tests=True -o enable_documentaiton=True -o shared=True
$ conan build ..
```

## Polymorphic basics

Let's say that you want to implement this kind of interface:
```cpp
class Foo {
public:
  virtual void do_something() = 0;
  virtual void do_something_else() {
    puts("Foo do_something_else");
  }
  virtual ~Foo() = default;
};

class Bar : public Foo{  
public:
  void do_something() override {
    puts("Bar do_something");
  }
};

class Baz : public Foo{  
public:
  void do_something() override {
    puts("Baz do_something");
  }
  void do_something_else() override {
    puts("Baz do_something_else");
  }
};
```

And in the main you will use this interface like this:
```cpp

int main() {  
  auto foo_ptr = std::make_unique<Bar>();
  auto another_foo_ptr = std::make_unique<Baz>();
}
```
The cons of this approach are:
+ We are loosing value semantics
+ Everything is allocated on the heap.
> You can avoid it with singular objects, but if you are using containers you will deffinetely need something like unique_ptr
+ You can't make this interface work with existing libraries, as they don't inherit from your base type
+ It is hard to develop complex interfaces as it will involve multiple inheritance.

With `Polymorphic` it becomes much easier. Let's look at the example first:
```cpp

// Declare tags for our functions
struct do_something{};
struct do_something_else{};


// To define implementation for our function we need to 
// create polymoprhicExtend function which consists of following parameters:
// + Tag declared earlier(we will not use it, it's only for function search)
// + Reference to an object
// + Other parameters of the function
// Because do_something doesn't have default implementation we will require user class to provide it
template<typename T>
void polymoirphicExtend(do_something /*unused*/, T& obj) {  
  obj.do_something();
}

// Default implementation for do_something_else
template<typename T>
void polymorphicExtend(do_something_else /*unused*/, T& obj) {
  puts("Foo do_something_else");
}

// This concept will check if the class has do_something_else method
// You can use SFINAE for that if c++20 is not available for you
template<typename T>
concept HasDoSomethingElse  = requires(T a){
    {a.do_something_else()};
};
template<HasDoSomethingElse T>
void polymorphicExtend(do_something_else /*unused*/, T& obj) {
  obj.do_something_else();
}

// And finally let's declare our wrapper type
using Foo = Polymorphic<TaggedSignature<do_something, void()>, TaggedSignature<do_something_else, void()>>;
using FooPtr = PolymorphicPtr<TaggedSignature<do_something, void()>, TaggedSignature<do_something_else, void()>>;


// Now let's define our implementation classes:
class Bar {  
public:
  void do_something() {
    puts("Bar do_something");
  }
};

class Baz {  
public:
  void do_something() {
    puts("Baz do_something");
  }
  void do_something_else() {
    puts("Baz do_something_else");
  }
};
```

To use it we will do something like this:
```cpp
int main() {
  // We can do move construction of Bar here, but it's better to construct it in place
  auto foo = Foo(std::in_place_type_t<Bar>{});
  auto another_foo = Foo(std::in_place_type_t<Baz>{});
}
```

While interface looks weird and scary here are the positive sides:
+ Value semantics. `Polymorphic`s copy the content on copy construction and copy assignment. 
  + You can use `UniquePolymorphic` for non-copyable types.
+ Allocation on the stack. We use small buffer optimization and you can use `GenericPolymorphic` 
to specify the size for the objects that you want.
+ No inheritance at all. You can overload `polymorphicExtend` for any type(even for functions).
+ Any polymorphic can be 'upcasted' to `PolymorphcPtr` with less amount of functions.