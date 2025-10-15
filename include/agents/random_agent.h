#ifndef RANDOM_AGENT_H
#define RANDOM_AGENT_H

#include "../agent.h"
#include "../config.h"
#include <random>

/**
 * Simple random agent for testing and baseline comparison.
 * Chooses actions uniformly at random from the action space.
 */
class RandomAgent : public Agent {
public:
    RandomAgent(double action_low, double action_high);
    RandomAgent(const Config& config);
    ~RandomAgent() override = default;
    
    // Agent interface implementation
    Action act(const State& state) override;
    void learn(const Experience& experience) override;
    
    // Agent metadata
    std::string getName() const override { return "RandomAgent"; }
    std::string getDescription() const override { 
        return "Baseline agent that chooses actions uniformly at random"; 
    }
    
    // Get learning statistics
    std::vector<std::pair<std::string, double>> getStats() const override;

private:
    double action_low_;
    double action_high_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> action_dist_;
    
    // Statistics
    int total_actions_;
    double last_action_;
};

#endif // RANDOM_AGENT_H
