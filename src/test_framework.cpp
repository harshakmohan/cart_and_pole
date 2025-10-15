#include <iostream>
#include "environment_factory.h"
#include "agent_factory.h"
#include "experiment_runner.h"
#include "config.h"

int main() {
    try {
        // Create config for CartPole environment
        Config env_config;
        env_config.set("model_path", std::string("mujoco/cartpole.xml"));
        env_config.set("render", false);
        
        // Create config for rule-based agent
        Config agent_config;
        agent_config.set("max_force", 10.0);
        
        // Create environment and agent using factories
        auto env = EnvironmentFactory::create("cartpole", env_config);
        auto agent = AgentFactory::create("rule_based", agent_config);
        
        std::cout << "Created environment: " << env->getName() << std::endl;
        std::cout << "Created agent: " << agent->getName() << std::endl;
        
        // Create experiment runner
        ExperimentRunner runner(std::move(env), std::move(agent));
        
        // Run a short test
        ExperimentRunner::ExperimentConfig config;
        config.num_episodes = 5;
        config.max_steps_per_episode = 200;
        config.render = false;
        config.log_frequency = 1;
        
        auto stats = runner.runExperiment(config);
        
        std::cout << "\nFramework test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
