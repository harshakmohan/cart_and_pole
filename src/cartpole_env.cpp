#include "cartpole_env.h"
#include <iostream>
#include <cmath>
#include <stdexcept>
#include "GLFW/glfw3.h"

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

std::vector<double> CartPoleEnv::reset() {
    // Reset simulation data
    mj_resetData(model_, data_);
    
    // Set small random initial conditions
    data_->qpos[0] = uniform_dist_(rng_);  // cart position
    data_->qpos[1] = uniform_dist_(rng_);  // pole angle
    data_->qvel[0] = uniform_dist_(rng_);  // cart velocity
    data_->qvel[1] = uniform_dist_(rng_);  // pole angular velocity
    
    // Forward dynamics to compute derived quantities
    mj_forward(model_, data_);
    
    // Reset step counter
    current_step_ = 0;
    
    return getState();
}

std::tuple<std::vector<double>, double, bool, std::string> CartPoleEnv::step(double action) {
    // Clip action to valid range
    action = std::max(-max_force_, std::min(max_force_, action));
    
    // Apply action
    data_->ctrl[0] = action;
    
    // Step simulation
    mj_step(model_, data_);
    
    // Get new state
    std::vector<double> state = getState();
    
    // Check if done
    bool done = isDone() || (++current_step_ >= max_episode_steps_);
    
    // Compute reward
    double reward = computeReward(state, done);
    
    // Info string
    std::string info = done ? (current_step_ >= max_episode_steps_ ? "TimeLimit" : "Terminated") : "";
    
    return std::make_tuple(state, reward, done, info);
}

std::vector<double> CartPoleEnv::getState() const {
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
    
    return (x < -x_threshold_ || x > x_threshold_ ||
            theta < -theta_threshold_radians_ || theta > theta_threshold_radians_);
}

double CartPoleEnv::computeReward(const std::vector<double>& state, bool done) const {
    if (!done) {
        // Standard reward: +1 for each step the pole remains upright
        return 1.0;
    } else if (current_step_ >= max_episode_steps_) {
        // Bonus for reaching max steps
        return 1.0;
    } else {
        // Penalty for falling
        return 0.0;
    }
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
             computeReward(getState(), isDone()));
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
