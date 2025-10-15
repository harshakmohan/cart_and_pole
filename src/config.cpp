#include "config.h"
#include <fstream>
#include <stdexcept>
#include <sstream>

// Simple JSON parser for basic config support
// Note: This is a minimal implementation. For production, consider using a proper JSON library.

namespace {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    ConfigValue parseValue(const std::string& value) {
        std::string trimmed = trim(value);
        
        // Remove quotes if present
        if (trimmed.front() == '"' && trimmed.back() == '"') {
            return trimmed.substr(1, trimmed.length() - 2);
        }
        
        // Check for boolean
        if (trimmed == "true") return true;
        if (trimmed == "false") return false;
        
        // Try to parse as number
        try {
            if (trimmed.find('.') != std::string::npos) {
                return std::stod(trimmed);
            } else {
                return std::stoi(trimmed);
            }
        } catch (...) {
            // If parsing fails, treat as string
            return trimmed;
        }
    }
}

Config Config::fromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filepath);
    }
    
    Config config;
    std::string line;
    std::string current_section = "";
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line.substr(0, 2) == "//") {
            continue;
        }
        
        // Skip JSON braces for now (simple parser)
        if (line == "{" || line == "}") {
            continue;
        }
        
        // Look for key-value pairs
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = trim(line.substr(0, colon_pos));
            std::string value = trim(line.substr(colon_pos + 1));
            
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value.pop_back();
                value = trim(value);
            }
            
            // Remove quotes from key
            if (key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.length() - 2);
            }
            
            config.set(key, parseValue(value));
        }
    }
    
    return config;
}

void Config::toJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not create config file: " + filepath);
    }
    
    file << "{\n";
    bool first = true;
    
    for (const auto& pair : values) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << pair.first << "\": ";
        
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else {
                file << value;
            }
        }, pair.second);
    }
    
    file << "\n}\n";
}
