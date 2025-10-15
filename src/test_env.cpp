#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <memory>
#include "cartpole_env.h"
#include "rule_based_agent.h"

// This function is now replaced by the RuleBasedAgent class

int main() {
    try {
        // Create environment and agent using polymorphism (educational value)
        std::unique_ptr<Environment> env = std::make_unique<CartPoleEnv>("mujoco/cartpole.xml", true);
        std::unique_ptr<Agent> agent = std::make_unique<RuleBasedAgent>(10.0);
        
        std::cout << "=== CartPole Environment Test ===" << std::endl;
        std::cout << "Environment: " << env->getName() << std::endl;
        std::cout << "Agent: " << agent->getName() << std::endl;
        std::cout << "Observation space size: " << env->getObservationSpaceSize() << std::endl;
        std::cout << "Action space size: " << env->getActionSpaceSize() << std::endl;
        std::cout << "Action range: [" << env->getActionSpaceLow() << ", " << env->getActionSpaceHigh() << "]" << std::endl;
        
        // Run a few episodes
        int num_episodes = 5;
        for (int episode = 0; episode < num_episodes; ++episode) {
            std::cout << "\n--- Episode " << episode + 1 << " ---" << std::endl;
            
            // Reset environment
            State state = env->reset();
            double total_reward = 0.0;
            bool done = false;
            int steps = 0;
            
            // Cast to CartPoleEnv to access shouldClose() - in a real framework, 
            // this method would be in the Environment base class
            CartPoleEnv* cartpole_env = static_cast<CartPoleEnv*>(env.get());
            
            while (!done && !cartpole_env->shouldClose()) {
                // Render
                env->render();
                
                // Agent chooses action
                Action action = agent->act(state);
                
                // Step environment
                auto [next_state, reward, is_done, info] = env->step(action);
                
                // Agent learns from experience (rule-based agent ignores this)
                Experience exp(state, action, reward, next_state, is_done);
                agent->learn(exp);
                
                // Update state and accumulate reward
                state = next_state;
                total_reward += reward;
                done = is_done;
                steps++;
                
                // Add small delay for visualization
                std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
            }
            
            // Check if user closed window
            if (cartpole_env->shouldClose()) {
                std::cout << "Window closed by user, exiting..." << std::endl;
                break;  // Exit episode loop
            }
            
            std::cout << "Episode finished after " << steps << " steps" << std::endl;
            std::cout << "Total reward: " << total_reward << std::endl;
        }
        
        // Close environment
        env->close();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
