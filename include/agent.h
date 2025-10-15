#ifndef AGENT_H
#define AGENT_H

#include "environment.h"
#include <vector>
#include <string>
#include <memory>

/**
 * Abstract base class for all RL agents.
 * Defines the interface that all learning algorithms must implement.
 */
class Agent {
public:
    virtual ~Agent() = default;
    
    // Core agent interface
    virtual Action act(const State& state) = 0;
    virtual void learn(const Experience& experience) = 0;
    
    // Episode-based learning (for algorithms that need full trajectories)
    virtual void learn(const std::vector<Experience>& trajectory) {
        // Default implementation: learn from each experience individually
        for (const auto& exp : trajectory) {
            learn(exp);
        }
    }
    
    // Training mode control
    virtual void setTrainingMode(bool training) { training_mode_ = training; }
    virtual bool isTraining() const { return training_mode_; }
    
    // Model persistence
    virtual void saveModel(const std::string& filepath) {}
    virtual void loadModel(const std::string& filepath) {}
    
    // Agent metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Reset agent state (for episodic algorithms)
    virtual void reset() {}
    
    // Get learning statistics
    virtual std::vector<std::pair<std::string, double>> getStats() const {
        return {};
    }

protected:
    bool training_mode_ = true;
};

#endif // AGENT_H
