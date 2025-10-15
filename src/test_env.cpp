#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include "cartpole_env.h"

// Simple bang-bang controller for testing
double simpleController(const std::vector<double>& state) {
    double x = state[0];
    double x_dot = state[1];
    double theta = state[2];
    double theta_dot = state[3];
    
    // Simple rule: push cart in direction to correct pole angle
    double force = 10.0;  // max force
    if (theta > 0.0) {
        return force;  // push right if pole tilting right
    } else {
        return -force;  // push left if pole tilting left
    }
}

int main() {
    try {
        // Create environment with rendering enabled
        CartPoleEnv env("mujoco/cartpole.xml", true);
        
        std::cout << "=== CartPole Environment Test ===" << std::endl;
        std::cout << "Observation space size: " << env.getObservationSpaceSize() << std::endl;
        std::cout << "Action space size: " << env.getActionSpaceSize() << std::endl;
        std::cout << "Action range: [" << env.getActionSpaceLow() << ", " << env.getActionSpaceHigh() << "]" << std::endl;
        
        // Run a few episodes
        int num_episodes = 5;
        for (int episode = 0; episode < num_episodes; ++episode) {
            std::cout << "\n--- Episode " << episode + 1 << " ---" << std::endl;
            
            // Reset environment
            std::vector<double> state = env.reset();
            double total_reward = 0.0;
            bool done = false;
            int steps = 0;
            
            while (!done && !env.shouldClose()) {
                // Render
                env.render();
                
                // Choose action using simple controller
                double action = simpleController(state);
                
                // Step environment
                auto [next_state, reward, is_done, info] = env.step(action);
                
                // Update state and accumulate reward
                state = next_state;
                total_reward += reward;
                done = is_done;
                steps++;
                
                // Add small delay for visualization
                std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
            }
            
            // Check if user closed window
            if (env.shouldClose()) {
                std::cout << "Window closed by user, exiting..." << std::endl;
                break;  // Exit episode loop
            }
            
            std::cout << "Episode finished after " << steps << " steps" << std::endl;
            std::cout << "Total reward: " << total_reward << std::endl;
        }
        
        // Close environment
        env.close();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
