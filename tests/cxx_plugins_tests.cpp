#include <gtest/gtest.h>

//#include <cxx_plugins/cxx_plugins.hpp>
#include <cxx_plugins/overload_set.hpp>
#include <cxx_plugins/plugin_builder.hpp>

#include <stack>

struct functions {};

struct init_plugin {};
struct shutdown_plugin {};

template <std::size_t I> struct placeholder {};

void foo() {
  using namespace CxxPlugins;
  namespace fs = std::filesystem;
  auto descriptor = makePluginDescriptor(
      makeOptionalField<plugin_name, std::string>(),
      makeRequiredField<library_path, fs::path>([](fs::path &path) {
        if (!fs::exists(path)) {
          throw std::runtime_error("Wrong library path.");
        }
        if (!fs::is_regular_file(path)) {
          throw std::runtime_error("Library is not a regular file.");
        }
        if (!path.has_extension() ||
            (path.extension() != ".dll" && path.extension() != ".so")) {
          throw std::runtime_error("Library has wrong extension.");
        }
      }),
      makeOptionalSection<functions>(

          ));
  descriptor;
}

using PluginT = CxxPlugins::TupleMap<
    CxxPlugins::TaggedValue<plugin_name, std::__cxx11::basic_string<char>>,
    CxxPlugins::TaggedValue<library_path, std::filesystem::__cxx11::path>,
    CxxPlugins::TaggedValue<
        functions,
        CxxPlugins::TupleMap<
            CxxPlugins::TaggedValue<
                init_plugin, CxxPlugins::TupleMap<CxxPlugins::TaggedValue<
                                 declaration_name, void (*)()>>>,
            CxxPlugins::TaggedValue<
                shutdown_plugin, CxxPlugins::TupleMap<CxxPlugins::TaggedValue<
                                     declaration_name, void (*)()>>>>>>;

int main(int argc, char **argv) {

  using namespace CxxPlugins;
  namespace fs = std::filesystem;
  auto descriptor = makePluginDescriptor(
      makeOptionalField<plugin_name, std::string>(),
      makeRequiredField<library_path, fs::path>([](fs::path &path) {
        if (!fs::exists(path)) {
          throw std::runtime_error("Wrong library path.");
        }
        if (!fs::is_regular_file(path)) {
          throw std::runtime_error("Library is not a regular file.");
        }
        if (!path.has_extension() ||
            (path.extension() != ".dll" && path.extension() != ".so")) {
          throw std::runtime_error("Library has wrong extension.");
        }
      }),
      makeOptionalSection<functions>(
          makeRequiredFunction<init_plugin, void()>(),
          makeRequiredFunction<shutdown_plugin, void()>()));

  using DescriptorT = decltype(descriptor);

  std::cout << type_id<DescriptorT>().name() << '\n';
  std::cout << type_id<PluginFromLoaderT<DescriptorT>>().name() << '\n';

  char const* config =
R"json(
{
  "plugin_name" : "My first plugin",
  "library_path" : "CXX_PluginsTests",
  "functions" : {
    "init_plugin" : { "decl_name" : "MyPluginInit"},
    "shutdown_plugin" : { "decl_name" : "MyPluginShutdown" }
  }
}


)json";

  CxxPlugins::loadPluginFromString(config, descriptor);

  ::testing::InitGoogleTest(&argc, argv);
  auto result = RUN_ALL_TESTS();
  return result;
}