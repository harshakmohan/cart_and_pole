#ifndef CARTPOLE_ENV_H
#define CARTPOLE_ENV_H

#include <vector>
#include <tuple>
#include <random>
#include <memory>
#include "mujoco/mujoco.h"

// Forward declaration to avoid including GLFW in header
struct GLFWwindow;

class CartPoleEnv {
public:
    // Constructor and destructor
    CartPoleEnv(const std::string& model_path, bool render = false);
    ~CartPoleEnv();
    
    // OpenAI Gym-style interface
    std::vector<double> reset();
    std::tuple<std::vector<double>, double, bool, std::string> step(double action);
    void render();
    void close();
    
    // Get observation and action space info
    std::vector<double> getObservationSpaceLow() const;
    std::vector<double> getObservationSpaceHigh() const;
    double getActionSpaceLow() const { return -max_force_; }
    double getActionSpaceHigh() const { return max_force_; }
    int getObservationSpaceSize() const { return 4; }  // [x, x_dot, theta, theta_dot]
    int getActionSpaceSize() const { return 1; }  // [force]
    
    // Get current state
    std::vector<double> getState() const;
    
    // Set rendering mode
    void setRenderMode(bool render) { render_enabled_ = render; }
    
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
