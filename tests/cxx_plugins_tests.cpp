#include <gtest/gtest.h>

//#include <cxx_plugins/cxx_plugins.hpp>
#include <api.hpp>
#include <cxx_plugins/overload_set.hpp>
#include <cxx_plugins/plugin_builder.hpp>

#include <stack>

struct plugin_name {};
struct plugin_version {};
struct plugin_api {};
struct plugin_api_version {};

struct init_plugin {};
struct shutdown_plugin {};

struct systems_count {};

struct system_getters {};

struct log_callback {};

TEST(Plugins, BasicPluginFromString) {
  using namespace plugins;

  //! \attention no need to specify lib prefix or .so/.dll,
  //! boost::dll handles this for us

  constexpr char const *json_config =
      R"json(
{
  "shared_library" : "../lib/example_plugin",
  "configuration" : {
    "plugin_name" : "Example Plugin",
    "plugin_version" : [0,0,1],
    "plugin_api" : "ExampleApi",
    "plugin_api_version" : [0,0,1]
  },
  "members" : {
    "init_plugin" : "init_plugin",
    "shutdown_plugin" : "shutdown_plugin",
    "systems_count" : null,
    "log_callback" : "callback_ptr",
    "system_getters" : [
      "get_graphics_engine", "get_debug_renderer"
    ]
  }
}
)json";


  auto plugin = makePlugin(
      makeConfig(makeField<plugin_name, std::string>(),
                 makeField<plugin_version, std::array<int, 3>>(),
                 makeField<plugin_api, std::string>(),
                 makeField<plugin_api_version, std::array<int, 3>>()),
      makeMembers(
          // Adding variable of function type is same
          makeVariable<init_plugin, void()>(),
          // as adding function of function pointer type
          makeFunction<shutdown_plugin, void (*const)()>(),
          makeVariable<systems_count, int>(),
          makeVariable<log_callback, void (*)(char const *)>(),
          makeFunctionList<system_getters, SystemRef()>()));

  loadPluginFromString(json_config, plugin);

  auto log = [](char const *message_p) {
        std::cout << message_p;
  };

  *get<Members, log_callback>(plugin) = log;

  auto init = get<Members, init_plugin>(plugin).value_m;
  auto shutdown = get<Members, shutdown_plugin>(plugin).value_m;
  (*init)();

  auto &system_list = get<Members, system_getters>(plugin).value_m;

  for (auto &system_getter : system_list) {
    (*system_getter)().call<System::init>();
    (*system_getter)().call<System::update>();
    (*system_getter)().call<System::shutdown>();
  }

  (*shutdown)();
}

TEST(Plugins, BasicPluginFromFile) {
  using namespace plugins;

  //! \attention no need to specify lib prefix or .so/.dll,
  //! boost::dll handles this for us

  static const std::filesystem::path json_config_path =
      "../lib/example_plugin.json";
  namespace dll = boost::dll;

  auto plugin = makePlugin(
      makeConfig(makeField<plugin_name, std::string>(),
                 makeField<plugin_version, std::array<int, 3>>(),
                 makeField<plugin_api, std::string>(),
                 makeField<plugin_api_version, std::array<int, 3>>()),
      makeMembers(
          // Adding variable of function type is same
          makeVariable<init_plugin, void()>(),
          // as adding function of function pointer type
          makeFunction<shutdown_plugin, void (*const)()>(),
          makeVariable<systems_count, int>(),
          makeVariable<log_callback, void (*)(char const *)>(),
          makeFunctionList<system_getters, SystemRef()>()));

  loadPluginFromFile(json_config_path, plugin);

  auto log = [](char const *message_p) {
        std::cout << message_p;
  };

  *get<Members, log_callback>(plugin) = log;

  auto init = get<Members, init_plugin>(plugin).value_m;
  auto shutdown = get<Members, shutdown_plugin>(plugin).value_m;
  (*init)();

  auto &system_list = get<Members, system_getters>(plugin).value_m;

  for (auto &system_getter : system_list) {
    (*system_getter)().call<System::init>();
    (*system_getter)().call<System::update>();
    (*system_getter)().call<System::shutdown>();
  }

  (*shutdown)();
}

int main(int argc, char **argv) {

  ::testing::InitGoogleTest(&argc, argv);
  auto result = RUN_ALL_TESTS();
  return result;
}