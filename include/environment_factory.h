#ifndef ENVIRONMENT_FACTORY_H
#define ENVIRONMENT_FACTORY_H

#include "environment.h"
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <vector>

// Forward declaration for config system
struct Config;

/**
 * Factory class for creating environments by name.
 * Implements the registry pattern for pluggable environments.
 */
class EnvironmentFactory {
public:
    // Type alias for environment creation function
    using EnvironmentCreator = std::function<std::unique_ptr<Environment>(const Config& config)>;
    
    // Register a new environment type
    static void registerEnvironment(const std::string& name, EnvironmentCreator creator);
    
    // Create an environment by name
    static std::unique_ptr<Environment> create(const std::string& name, const Config& config);
    
    // Query available environments
    static std::vector<std::string> getAvailableEnvironments();
    static bool isEnvironmentRegistered(const std::string& name);
    
    // Clear all registrations (useful for testing)
    static void clear();

private:
    static std::map<std::string, EnvironmentCreator>& getRegistry();
};

// Macro to simplify environment registration
#define REGISTER_ENVIRONMENT(name, class_type) \
    namespace { \
        struct class_type##Registrar { \
            class_type##Registrar() { \
                EnvironmentFactory::registerEnvironment(name, \
                    [](const Config& config) -> std::unique_ptr<Environment> { \
                        return std::make_unique<class_type>(config); \
                    }); \
            } \
        }; \
        static class_type##Registrar class_type##_registrar; \
    }

#endif // ENVIRONMENT_FACTORY_H
