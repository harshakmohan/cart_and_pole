#include "environment_factory.h"
#include "config.h"
#include <stdexcept>

std::map<std::string, EnvironmentFactory::EnvironmentCreator>& EnvironmentFactory::getRegistry() {
    static std::map<std::string, EnvironmentCreator> registry;
    return registry;
}

void EnvironmentFactory::registerEnvironment(const std::string& name, EnvironmentCreator creator) {
    auto& registry = getRegistry();
    if (registry.find(name) != registry.end()) {
        throw std::runtime_error("Environment '" + name + "' is already registered");
    }
    registry[name] = creator;
}

std::unique_ptr<Environment> EnvironmentFactory::create(const std::string& name, const Config& config) {
    auto& registry = getRegistry();
    auto it = registry.find(name);
    if (it == registry.end()) {
        throw std::runtime_error("Environment '" + name + "' is not registered");
    }
    return it->second(config);
}

std::vector<std::string> EnvironmentFactory::getAvailableEnvironments() {
    auto& registry = getRegistry();
    std::vector<std::string> names;
    names.reserve(registry.size());
    for (const auto& pair : registry) {
        names.push_back(pair.first);
    }
    return names;
}

bool EnvironmentFactory::isEnvironmentRegistered(const std::string& name) {
    auto& registry = getRegistry();
    return registry.find(name) != registry.end();
}

void EnvironmentFactory::clear() {
    getRegistry().clear();
}
