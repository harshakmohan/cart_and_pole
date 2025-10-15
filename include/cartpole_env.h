#ifndef CARTPOLE_ENV_H
#define CARTPOLE_ENV_H

#include <vector>
#include <tuple>
#include <random>
#include <memory>
#include "environment.h"
#include "config.h"
#include "mujoco/mujoco.h"

// Forward declaration to avoid including GLFW in header
struct GLFWwindow;

class CartPoleEnv : public Environment {
public:
    // Constructor and destructor
    CartPoleEnv(const std::string& model_path, bool render = false);
    CartPoleEnv(const Config& config);  // New constructor for factory system
    ~CartPoleEnv() override;
    
    // Environment interface implementation
    State reset() override;
    StepResult step(Action action) override;
    void render() override;
    void close() override;
    
    // Environment metadata (implementing base class interface)
    int getObservationSpaceSize() const override { return 4; }  // [x, x_dot, theta, theta_dot]
    int getActionSpaceSize() const override { return 1; }  // [force]
    std::vector<double> getObservationSpaceLow() const override;
    std::vector<double> getObservationSpaceHigh() const override;
    double getActionSpaceLow() const override { return -max_force_; }
    double getActionSpaceHigh() const override { return max_force_; }
    
    // Environment identification
    std::string getName() const override { return "CartPole-v1"; }
    std::string getDescription() const override { 
        return "Classic cart-pole control task using MuJoCo physics"; 
    }
    
    // Get current state
    State getCurrentState() const override;
    
    // Set rendering mode
    void setRenderMode(bool render) override { render_enabled_ = render; }
    
    // Check if window should close (for proper event handling)
    bool shouldClose() const;
    
private:
    // MuJoCo model and data
    mjModel* model_;
    mjData* data_;
    
    // Rendering structures (only if render_enabled_)
    mjvCamera* cam_;
    mjvOption* opt_;
    mjvScene* scn_;
    mjrContext* con_;
    GLFWwindow* window_;
    
    // Environment parameters
    double max_force_;
    double x_threshold_;
    double theta_threshold_radians_;
    int max_episode_steps_;
    int current_step_;
    
    // Random number generation for initial states
    std::mt19937 rng_;
    std::uniform_real_distribution<double> uniform_dist_;
    
    // Rendering flag
    bool render_enabled_;
    
    // Helper functions
    void initializeRendering();
    void cleanupRendering();
    bool isDone() const;
    double computeReward(const std::vector<double>& state, bool done) const;
};

#endif // CARTPOLE_ENV_H
