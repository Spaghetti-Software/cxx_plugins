# Style Guide

## Introduction
As a general rule llvm coding standard should be used. Unless this document contradicts with it.
https://llvm.org/docs/CodingStandards.html


## C++ Standard Versions
LLVM uses C++14 standard. We encourage usage of latest standard available at the begining of development. [C++20 on the moment of writing this document].

## C++ Standard Library

LLVM encourages use of llvm custom data structures and c++ standard data structures.

We encourage use of existing data structures and libraries as well. But as we don't have llvm code base we encourage use of boost and other libraries. But only if standard library doesn't provide required functionality.

If custom data structures should be created, then they should be all packed in separate library.

## Mechanical Source Issues
### Source Code Formatting

#### File Headers
Modified standard header(Resharper live template syntax is used):
```cpp
/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov                                 
 * This file is part of $PROJECT$ project.                                                               
 * License is available at https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE 
 *************************************************************************************************/

/*!
 * \file    $FILE$
 * \author  $AuthorName$
 * \date    $DATE$
 * \brief
 * $Abstract$
 * 
 * \description
 * $MoreInfo$
 *
 */

```

#### #include Style

1. Main module header
2. Local/Private Headers
3. Subproject headers
4. External includes
5. System includes

#### RTTI/Exceptions

LLVM uses custom form of RTTI and prohibits C++ RTTI and exceptions.

We on the other hand believe that functionality of C++ should be utilized to the full extend. Though we are waiting for proposal [P0709](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0709r2.pdf) that will allow us to avoid exceptions overhead.

##### RTTI

RTTI is usually used for polymorphism. While we allow use of RTTI, other forms of polymorphism should be preferred. But only because inheritance often creates poor design.

For example google's(not official) [polymorhic](https://github.com/google/cpp-from-the-sky-down/blob/master/metaprogrammed_polymorphism/polymorphic.hpp) can be used.

##### Exceptions/Error handling
We will be using custom C++ exceptions in our code.

The base class for the error exceptions should have the ability to save information about what line and file the exception has been thrown. This will be used for writting any caught exceptions' information into a log file.

Assertions can be added to the code as long as they can be disabled (like defining NDEBUG).

There should not be any unreachable code without a solid reason (e.g. special case bug fixing with a live debugger) and as a general rule should be avoided.

## Style Issues

### The High-Level issues
#### File Naming

All files should contain no spaces and capital letters
**Exceptions:** 
+ readme files  
+ CMakeLists.txt

[C++] file naming:
+ source - `*.cpp`
+ header - `*.hpp`
+ inlined files - `*.ipp`
[C] file naming:
+ source - `*.c`
+ header - `*.h`
+ inlined files `*.i`

### The Low-Level issues

#### Name Types, Functions, Variables, and Enumerators Properly

**Note on enums:** Prefer using `enum class` instead of regular enums. `enum class` doesn't require to have prefix.

**[C++]**

Every class, function, concept, etc. should be in nested namespaces:
```cpp
namespace ${project_name}
{
  namespace ${domain}
  {
    class foo {
      ...
    };
  }
}
```
`${domain}` namespace can be dropped if project is small and covers only one domain. Project can be considered small if it has less than 100 entries in its public interface(including class member functions).

+ Prefer small projects with one domain.
+ `impl` namespace can be used to hide template implementation details.

**[C]**
Every function, struct, union, typedef, etc. should have same structure as in C++, but naming should be used instead.
```c
struct ${project_name}_${domain}_foo {
  int a;
};
```
Macros can be used to simplify naming:
```c
// external file with helper macros
// These macros are made because a ## b ## c
// would produce 'abc' instead of '${a}${b}${c}'
#define PP_CAT_V(a,b,c,d,e) PP_CAT_V_IMPL(a,b,c,d,e)
#define PP_CAT_V_IMPL(a,b,c,d,e) a##b##c##d##e
#define CAPI_DECLARE(name) PP_CAT_V(PROJECT_NAME,_,DOMAIN_NAME,_,name)

#define PP_CAT_III(a,b,c) PP_CAT_III_IMPL(a,b,c)
#define PP_CAT_III_IMPL(a,b,c) a##b##c
#define CAPI_DECLARE_SHORT(name) PP_CAT_III(PROJECT_NAME,_,name)


// Your file
#define PROJECT_NAME MyAwesomeProject
#define DOMAIN_NAME MyAwesomeRendering

struct CAPI_DECLARE(Camera)
{
  int a;
};

void CAPI_DECLARE_SHORT(doSomething)() {

}

#undef DOMAIN_NAME
#undef PROJECT_NAME


// main file
int main() {
  MyAwesomeProject_MyAwesomeRendering_Camera camera;
  MyAwesomeProject_doSomething();
}

```

#### Assertions
See [Exceptions/Error handling](#exceptionserror-handling)


#### #include <iostream> is Forbidden

> **NEED TO FIND** `iostream` **replacement**

## Version Control

### Versioning
`MAJOR.MINOR.PATCH`
Change of version meanings:
`Major` - breaking changes to the interface were made 
`Minor` - no breaking changes to the interface(only new functionality)
`Patch` - change of implementation(bug fixing)

Version number should be synchronized between cmake(or other build systems if used) and git.
To update git version use:
`git tag -a "${MAJOR}.${MINOR}.${PATCH}"`


### Branching

All development branches should have the following naming convention:
`dev/${domain}/${feature}`


### Github Actions

Github actions should be used to check build status and test code.

`master` branch should use github servers for testing in order to make sure that no special configuration of machine is required to compile and run code.

In order to reduce used hours on github all other branches should use self-hosted servers.


## Files

> All rules apply ony to local files and aren't applied to external repositories.



### Folder structure

Folder structure `[dev]`
```
project
|
+-- src               // private files (sources and headers)
+-- include           // public(interface) headers
|   +-- ${project_name}
+-- tests             // source files for tests
+-- build
|   +-- ${platform}
|       +-- ${build_type}
+-- docs              // documentation
+-- external          // external projects and sources
+-- lib               // precompiled external libraries
|   +-- ${platform}
+-- cmake             // cmake scripts and configurations
+-- tools             // [optional] sub-projects
+-- resources         // [optional] files that can be edited by user in the end product
```

Folder structure `[install]`

```
project
|
+-- resources     // same as in [dev]
+-- tools         // subprojects (now compiled)
+-- docs
+-- bin           // binary files
```


## Testing

Should utilize CTest.

### Fuzz testing

### Unit testing

For code testing Catch2 or GoogleTest can be used.