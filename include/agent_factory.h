#ifndef AGENT_FACTORY_H
#define AGENT_FACTORY_H

#include "agent.h"
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <vector>

// Forward declaration for config system
struct Config;

/**
 * Factory class for creating agents by name.
 * Implements the registry pattern for pluggable RL algorithms.
 */
class AgentFactory {
public:
    // Type alias for agent creation function
    using AgentCreator = std::function<std::unique_ptr<Agent>(const Config& config)>;
    
    // Register a new agent type
    static void registerAgent(const std::string& name, AgentCreator creator);
    
    // Create an agent by name
    static std::unique_ptr<Agent> create(const std::string& name, const Config& config);
    
    // Query available agents
    static std::vector<std::string> getAvailableAgents();
    static bool isAgentRegistered(const std::string& name);
    
    // Clear all registrations (useful for testing)
    static void clear();

private:
    static std::map<std::string, AgentCreator>& getRegistry();
};

// Macro to simplify agent registration
#define REGISTER_AGENT(name, class_type) \
    namespace { \
        struct class_type##Registrar { \
            class_type##Registrar() { \
                AgentFactory::registerAgent(name, \
                    [](const Config& config) -> std::unique_ptr<Agent> { \
                        return std::make_unique<class_type>(config); \
                    }); \
            } \
        }; \
        static class_type##Registrar class_type##_registrar; \
    }

#endif // AGENT_FACTORY_H
