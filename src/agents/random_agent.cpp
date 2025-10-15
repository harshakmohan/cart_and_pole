#include "../include/agents/random_agent.h"
#include "../include/agent_factory.h"

RandomAgent::RandomAgent(double action_low, double action_high)
    : action_low_(action_low), action_high_(action_high),
      rng_(std::random_device{}()), action_dist_(action_low, action_high),
      total_actions_(0), last_action_(0.0) {
}

RandomAgent::RandomAgent(const Config& config)
    : action_low_(config.get<double>("action_low", -1.0)),
      action_high_(config.get<double>("action_high", 1.0)),
      rng_(std::random_device{}()), action_dist_(action_low_, action_high_),
      total_actions_(0), last_action_(0.0) {
}

Action RandomAgent::act(const State& state) {
    // Ignore state and choose random action
    last_action_ = action_dist_(rng_);
    total_actions_++;
    return last_action_;
}

void RandomAgent::learn(const Experience& experience) {
    // Random agent doesn't learn - this is intentionally empty
    // In a real implementation, we might still track statistics
}

std::vector<std::pair<std::string, double>> RandomAgent::getStats() const {
    return {
        {"total_actions", static_cast<double>(total_actions_)},
        {"last_action", last_action_},
        {"action_range_low", action_low_},
        {"action_range_high", action_high_}
    };
}

// Register RandomAgent with the factory
namespace {
    struct RandomAgentRegistrar {
        RandomAgentRegistrar() {
            AgentFactory::registerAgent("random", 
                [](const Config& config) -> std::unique_ptr<Agent> {
                    return std::make_unique<RandomAgent>(config);
                });
        }
    };
    static RandomAgentRegistrar random_agent_registrar;
}
