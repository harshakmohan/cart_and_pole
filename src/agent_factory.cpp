#include "agent_factory.h"
#include "config.h"
#include <stdexcept>

std::map<std::string, AgentFactory::AgentCreator>& AgentFactory::getRegistry() {
    static std::map<std::string, AgentCreator> registry;
    return registry;
}

void AgentFactory::registerAgent(const std::string& name, AgentCreator creator) {
    auto& registry = getRegistry();
    if (registry.find(name) != registry.end()) {
        throw std::runtime_error("Agent '" + name + "' is already registered");
    }
    registry[name] = creator;
}

std::unique_ptr<Agent> AgentFactory::create(const std::string& name, const Config& config) {
    auto& registry = getRegistry();
    auto it = registry.find(name);
    if (it == registry.end()) {
        throw std::runtime_error("Agent '" + name + "' is not registered");
    }
    return it->second(config);
}

std::vector<std::string> AgentFactory::getAvailableAgents() {
    auto& registry = getRegistry();
    std::vector<std::string> names;
    names.reserve(registry.size());
    for (const auto& pair : registry) {
        names.push_back(pair.first);
    }
    return names;
}

bool AgentFactory::isAgentRegistered(const std::string& name) {
    auto& registry = getRegistry();
    return registry.find(name) != registry.end();
}

void AgentFactory::clear() {
    getRegistry().clear();
}
