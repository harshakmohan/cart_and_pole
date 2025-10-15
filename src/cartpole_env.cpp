#include "cartpole_env.h"
#include <iostream>
#include <cmath>
#include <stdexcept>
#include "GLFW/glfw3.h"
#include "environment_factory.h"

CartPoleEnv::CartPoleEnv(const std::string& model_path, bool render)
    : model_(nullptr), data_(nullptr), cam_(nullptr), opt_(nullptr),
      scn_(nullptr), con_(nullptr), window_(nullptr),
      max_force_(10.0), x_threshold_(2.4), theta_threshold_radians_(12 * M_PI / 180),
      max_episode_steps_(500), current_step_(0),
      rng_(std::random_device{}()), uniform_dist_(-0.05, 0.05),
      render_enabled_(render) {
    
    // Load MuJoCo model
    char error[1000] = "Could not load model";
    model_ = mj_loadXML(model_path.c_str(), nullptr, error, sizeof(error));
    if (!model_) {
        throw std::runtime_error(std::string("Failed to load MuJoCo model: ") + error);
    }
    
    // Create data
    data_ = mj_makeData(model_);
    
    // Initialize rendering if enabled
    if (render_enabled_) {
        initializeRendering();
    }
}

CartPoleEnv::CartPoleEnv(const Config& config)
    : model_(nullptr), data_(nullptr), cam_(nullptr), opt_(nullptr),
      scn_(nullptr), con_(nullptr), window_(nullptr),
      max_force_(config.get<double>("max_force", 10.0)), 
      x_threshold_(config.get<double>("x_threshold", 2.4)), 
      theta_threshold_radians_(config.get<double>("theta_threshold_degrees", 12.0) * M_PI / 180),
      max_episode_steps_(config.get<int>("max_episode_steps", 500)), 
      current_step_(0),
      rng_(std::random_device{}()), uniform_dist_(-0.05, 0.05),
      render_enabled_(config.get<bool>("render", false)) {
    
    // Load MuJoCo model
    std::string model_path = config.get<std::string>("model_path", "mujoco/cartpole.xml");
    char error[1000] = "Could not load model";
    model_ = mj_loadXML(model_path.c_str(), nullptr, error, sizeof(error));
    if (!model_) {
        throw std::runtime_error(std::string("Failed to load MuJoCo model: ") + error);
    }
    
    // Create data
    data_ = mj_makeData(model_);
    
    // Initialize rendering if enabled
    if (render_enabled_) {
        initializeRendering();
    }
}

CartPoleEnv::~CartPoleEnv() {
    close();
}

void CartPoleEnv::initializeRendering() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize GLFW");
    }
    
    // Create window
    window_ = glfwCreateWindow(800, 600, "CartPole Environment", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Could not create GLFW window");
    }
    
    // Make context current
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    
    // Allocate visualization structures
    cam_ = new mjvCamera();
    opt_ = new mjvOption();
    scn_ = new mjvScene();
    con_ = new mjrContext();
    
    // Initialize visualization structures
    mjv_defaultCamera(cam_);
    mjv_defaultOption(opt_);
    mjr_defaultContext(con_);
    
    // Create scene
    mjv_makeScene(model_, scn_, 2000);
    
    // Set camera position
    cam_->distance = 3.0;
    cam_->azimuth = 90;
    cam_->elevation = -20;
    cam_->lookat[0] = 0;
    cam_->lookat[1] = 0;
    cam_->lookat[2] = 0.3;
    
    // Initialize OpenGL context
    mjr_makeContext(model_, con_, mjFONTSCALE_150);
}

void CartPoleEnv::cleanupRendering() {
    if (window_) {
        // Free visualization storage
        mjv_freeScene(scn_);
        mjr_freeContext(con_);
        
        // Delete structures
        delete cam_;
        delete opt_;
        delete scn_;
        delete con_;
        
        // Destroy window and terminate GLFW
        glfwDestroyWindow(window_);
        glfwTerminate();
        
        // Reset pointers
        cam_ = nullptr;
        opt_ = nullptr;
        scn_ = nullptr;
        con_ = nullptr;
        window_ = nullptr;
    }
}

State CartPoleEnv::reset() {
    // Reset simulation data to XML defaults (pole hanging down due to ref="3.14159")
    mj_resetData(model_, data_);
    
    // Forward dynamics to compute derived quantities
    mj_forward(model_, data_);
    
    // Reset step counter
    current_step_ = 0;
    
    return getCurrentState();
}

StepResult CartPoleEnv::step(Action action) {
    // Clip action to valid range
    action = std::max(-max_force_, std::min(max_force_, action));
    
    // Apply action
    data_->ctrl[0] = action;
    
    // Step simulation
    mj_step(model_, data_);
    
    // Get new state
    State state = getCurrentState();
    
    // Check if done
    bool done = isDone() || (++current_step_ >= max_episode_steps_);
    
    // Compute reward
    double reward = computeReward(state, done);
    
    // Info string
    std::string info = done ? (current_step_ >= max_episode_steps_ ? "TimeLimit" : "Terminated") : "";
    
    return std::make_tuple(state, reward, done, info);
}

State CartPoleEnv::getCurrentState() const {
    return {
        data_->qpos[0],  // cart position
        data_->qvel[0],  // cart velocity
        data_->qpos[1],  // pole angle
        data_->qvel[1]   // pole angular velocity
    };
}

bool CartPoleEnv::isDone() const {
    double x = data_->qpos[0];
    double theta = data_->qpos[1];
    
    // Episode ends if cart goes out of bounds
    // For swing-up task, let pole swing freely (no angle limits)
    return (x < -x_threshold_ || x > x_threshold_);
}

double CartPoleEnv::computeReward(const std::vector<double>& state, bool done) const {
    double theta = state[2];  // pole angle
    
    // Reward based on how close pole is to upright (θ = 0 or θ = 2π)
    // cos(θ) = 1 when upright, -1 when hanging down
    double upright_reward = cos(theta);  // Range: [-1, 1]
    
    // Scale to [0, 2] so upright gives +2, hanging gives 0
    return upright_reward + 1.0;
}

void CartPoleEnv::render() {
    if (!render_enabled_ || !window_) {
        return;
    }
    
    // Check if window should close
    if (glfwWindowShouldClose(window_)) {
        return;
    }
    
    // Get window size
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    
    // Update scene
    mjv_updateScene(model_, data_, opt_, nullptr, cam_, mjCAT_ALL, scn_);
    
    // Render
    mjrRect viewport = {0, 0, width, height};
    mjr_render(viewport, scn_, con_);
    
    // Show state info
    char info[256];
    snprintf(info, sizeof(info), 
             "Step: %d\nCart Pos: %.3f\nPole Angle: %.3f rad (%.1f deg)\nReward: %.1f",
             current_step_, data_->qpos[0], data_->qpos[1], 
             data_->qpos[1] * 180.0 / M_PI, 
             computeReward(getCurrentState(), isDone()));
    mjr_overlay(mjFONT_NORMAL, mjGRID_TOPLEFT, viewport, info, nullptr, con_);
    
    // Swap buffers
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

void CartPoleEnv::close() {
    // Clean up rendering
    if (render_enabled_) {
        cleanupRendering();
    }
    
    // Delete MuJoCo data and model
    if (data_) {
        mj_deleteData(data_);
        data_ = nullptr;
    }
    if (model_) {
        mj_deleteModel(model_);
        model_ = nullptr;
    }
}

std::vector<double> CartPoleEnv::getObservationSpaceLow() const {
    return {-x_threshold_ * 2, -INFINITY, -theta_threshold_radians_ * 2, -INFINITY};
}

std::vector<double> CartPoleEnv::getObservationSpaceHigh() const {
    return {x_threshold_ * 2, INFINITY, theta_threshold_radians_ * 2, INFINITY};
}

bool CartPoleEnv::shouldClose() const {
    return render_enabled_ && window_ && glfwWindowShouldClose(window_);
}

// Register CartPole environment with the factory
namespace {
    struct CartPoleRegistrar {
        CartPoleRegistrar() {
            EnvironmentFactory::registerEnvironment("cartpole", 
                [](const Config& config) -> std::unique_ptr<Environment> {
                    return std::make_unique<CartPoleEnv>(config);
                });
        }
    };
    static CartPoleRegistrar cartpole_registrar;
}
