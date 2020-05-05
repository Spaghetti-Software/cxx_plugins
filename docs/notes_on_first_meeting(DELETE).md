# What are we doing

### Plugin manager

Templated plugin manager. 

```cpp
class System {
  virtual void Update(Scene* scene, ...);
}

extern "C" {
  void SystemUpdate(void* system, Scene* scene);
}
```

```cpp
template<typename... Targs>
class PluginManager {

  template<typename T>
  PluginManager<T,Targs...> add_class();
};

int main() {
  PluginCreator creator;
  auto plugin_manager = creator
    .add_class<System, FactoryMethods, ....>
    .add_c_class<SystemUpdate, ...>
}
```

config files
static plugins
dynamic plugins

vizualization tool


### ECS engine

HeapOverflow engine v2

1. Essage/Event Bus
2. Task scheduling
3. Parallelization


### DLL

Linking on startup

Pros:
+ as easy as static

Cons:
+ can't unload


Runtime linking:
```cpp

int main() {

  PLuginManager ....;

  for (config_file : plugins_dir) {
    plugin_manager.load(config_file);
  }


  auto lib = load_lib("foo.dll");
  void(*bar_ptr)() = lib.load<void()>("bar");


  bar_ptr();
}
```



# How will we work

1. Version Control
  GitHub

2. Automated builds
  Github
  Our own server

3. Build system 
   CMake 
  
4. Platforms
   Win/Linux(WSL)

5. Languages
  C/C++



# Schedule

25.04.2020 - First meeting

04.05.2020 - Start of work

Week 1
naming conventions
code style
repo creation
design start

Week 2 - Week 5
Design


Week 6-8
Implement + tests



25.08.2020 - Summer is gone









