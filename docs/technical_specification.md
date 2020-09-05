# Technical Specification

## Introduction

Managing dlls can be a difficult task. This library should simplify the proccess of loading dlls by automating function loading procedures.

## Objects

### `Polymorhic<StoragePolicy, pair<TagType, FunctionSignature>...>`

Polymorphic object represents object with custom vtable. Essential idea is that Polymorphic is a map of the form:
```json
{
  "tag_name" : "pointer to function",
  "tag_name2" : "pointer to another function" 
}
```

`StoragePolicy` should specify if objects of the same type store copy of vtable or share pointer to the same vtable.

**Required functionality:**
+ Function calls and function pointer extraction.
+ Associativity: `Polymorphic<pair<tag0,fn0>,pair<tag1,fn1>>` is same as `Polymorphic<pair<tag1,fn1>, pair<tag0,fn0>>`
+ Composition: We should be able compose polymorphic objects.
  For example we have polymorphic interface `Renderable` which has associated function `void render(void*)` and `Printable` with function `void print(void*)`. We should be able to create object that both `Renderable` and `Printable`.
  > **How?!**

> Effective container should be discussed and benchmarked. Requirements to the container should include:
> + Fast search.(Should be as fast as possible)(Preferrably compile time)
> + Deletion and insertion are not important as deletion will never be called and insertion will be called only on startup.
> + Conversion from runtime tag to compile time tag should be implemented, so interface can be used in C or other languages.

### `Plugin`

Plugin is a class that should represent library. This library can be shared(should be more common) or static.

**Required functionality:**
+ Function search
+ Class search
+ Class creation (should be wrapped with some class that provides automatic deletion on destruction)(only if user provides factories)
+ Creation of polymorphic objects (Same idea as class)


### `PluginManager`

**Required functionality:**
+ API declaration
+ Loading and managing plugins with specified API
+ Managing plugins
+ Search of the functionality in different plugins


## Types of APIs

C++ can import different types of APIs from external libraries. This section describes types of APIs.

### C API

Support of C API is very important as it can be a middle layer between different versions of C++, different compilers(if vtable layout is different) or 
even different languages.

#### Simple/Functional API

Functional API should not include object creation.
Suppose we want to have the following function to be loaded from external source:
```cpp
void* allocate(size_t size);
```

> Continue later with simple example of pure functiononal/procedural API.


