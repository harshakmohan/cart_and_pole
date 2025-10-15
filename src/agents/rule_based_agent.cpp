#include "../include/rule_based_agent.h"

RuleBasedAgent::RuleBasedAgent(double max_force)
    : max_force_(max_force), total_actions_(0), last_action_(0.0),
      left_actions_(0), right_actions_(0) {
}


Action RuleBasedAgent::act(const State& state) {
    if (state.size() < 4) {
        // Invalid state, return zero action
        last_action_ = 0.0;
        return last_action_;
    }
    
    // Extract state variables
    double x = state[0];        // cart position
    double x_dot = state[1];    // cart velocity  
    double theta = state[2];    // pole angle
    double theta_dot = state[3]; // pole angular velocity
    
    // Simple rule: push cart in direction to correct pole angle
    // More sophisticated rule could consider velocity and position too
    if (theta > 0.0) {
        last_action_ = max_force_;   // push right if pole tilting right
        right_actions_++;
    } else {
        last_action_ = -max_force_;  // push left if pole tilting left  
        left_actions_++;
    }
    
    total_actions_++;
    return last_action_;
}

void RuleBasedAgent::learn(const Experience& experience) {
    // Rule-based agent doesn't learn - this is intentionally empty
    // The rules are fixed and don't change based on experience
}

std::vector<std::pair<std::string, double>> RuleBasedAgent::getStats() const {
    return {
        {"total_actions", static_cast<double>(total_actions_)},
        {"last_action", last_action_},
        {"left_actions", static_cast<double>(left_actions_)},
        {"right_actions", static_cast<double>(right_actions_)},
        {"max_force", max_force_}
    };
}

