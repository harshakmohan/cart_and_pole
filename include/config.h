#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <variant>

// Variant type for config values
using ConfigValue = std::variant<int, double, std::string, bool>;

/**
 * Simple configuration system for experiments.
 * Supports nested configuration sections and type-safe value access.
 */
struct Config {
    std::map<std::string, ConfigValue> values;
    std::map<std::string, Config> sections;
    
    // Constructor
    Config() = default;
    Config(const std::map<std::string, ConfigValue>& vals) : values(vals) {}
    
    // Value accessors with defaults
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        auto it = values.find(key);
        if (it != values.end()) {
            try {
                return std::get<T>(it->second);
            } catch (const std::bad_variant_access&) {
                return default_value;
            }
        }
        return default_value;
    }
    
    // Section accessor
    Config getSection(const std::string& section_name) const {
        auto it = sections.find(section_name);
        return (it != sections.end()) ? it->second : Config{};
    }
    
    // Setters
    template<typename T>
    void set(const std::string& key, const T& value) {
        values[key] = value;
    }
    
    void setSection(const std::string& name, const Config& config) {
        sections[name] = config;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    bool hasSection(const std::string& section_name) const {
        return sections.find(section_name) != sections.end();
    }
    
    // Load from JSON file (to be implemented)
    static Config fromJsonFile(const std::string& filepath);
    
    // Save to JSON file (to be implemented)  
    void toJsonFile(const std::string& filepath) const;
    
    // Create config from key-value pairs (helper for testing)
    static Config create(const std::map<std::string, ConfigValue>& values) {
        return Config(values);
    }
};

#endif // CONFIG_H
