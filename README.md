# CxxPlugins

## Emphasis

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
    


## Polymorphic basics