#ifndef RULE_BASED_AGENT_H
#define RULE_BASED_AGENT_H

#include "../agent.h"
#include "../config.h"

/**
 * Simple rule-based agent for CartPole.
 * Uses a bang-bang controller based on pole angle.
 */
class RuleBasedAgent : public Agent {
public:
    RuleBasedAgent(double max_force = 10.0);
    RuleBasedAgent(const Config& config);
    ~RuleBasedAgent() override = default;
    
    // Agent interface implementation
    Action act(const State& state) override;
    void learn(const Experience& experience) override;
    
    // Agent metadata
    std::string getName() const override { return "RuleBasedAgent"; }
    std::string getDescription() const override { 
        return "Simple bang-bang controller for CartPole based on pole angle"; 
    }
    
    // Get learning statistics
    std::vector<std::pair<std::string, double>> getStats() const override;

private:
    double max_force_;
    
    // Statistics
    int total_actions_;
    double last_action_;
    int left_actions_;
    int right_actions_;
};

#endif // RULE_BASED_AGENT_H
