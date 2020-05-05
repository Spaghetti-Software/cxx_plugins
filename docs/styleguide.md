# Style Guide

## Files

> All rules apply ony to local files and aren't applied to external repositories

##### All files should contain no spaces and capital letters
Exceptions: readme files  

##### Project file structure [dev]
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

##### Project file structure [release] + [build folder]

```
project
|
+-- resources     // same as in [dev]
+-- tools         // subprojects (now compiled)
+-- docs
+-- all binary files
```

## Version Control

### Versioning
`MAJOR.MINOR.PATCH`
Change of version meanings:
`Patch` - change of implementation
`Minor` - no breaking changes to the interface(only new functionality)
`Major` - breaking changes to the interface were made 


### Branching

All development branches should have the following naming convention:
`dev/${domain}/${feature}`

## Code style

### Formatting 
#### Naming conventions
#### 

### Error handling
We will be using custom C++ exceptions in our code.

The base class for the error exceptions should have the ability to save information about what line and file the exception has been thrown. This will be used for writting any caught exceptions' information into a log file.

#### Types of Exceptions

### Inheritance

#### Virtual Functions
#### Virtual Inheritance
