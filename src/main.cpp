#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "mujoco/mujoco.h"
#include "GLFW/glfw3.h"

// MuJoCo visualization structures
mjvCamera cam;
mjvOption opt;
mjvScene scn;
mjrContext con;

// GLFW window
GLFWwindow* window;

// MuJoCo model and data
mjModel* m = nullptr;
mjData* d = nullptr;

// Mouse interaction
bool button_left = false;
bool button_middle = false;
bool button_right = false;
double lastx = 0;
double lasty = 0;

// Keyboard callback
void keyboard(GLFWwindow* window, int key, int scancode, int act, int mods) {
    if (act == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    // Reset simulation with 'R'
    if (act == GLFW_PRESS && key == GLFW_KEY_R) {
        mj_resetData(m, d);
        mj_forward(m, d);
    }
    
    // Apply force with arrow keys
    if (key == GLFW_KEY_LEFT) {
        d->ctrl[0] = (act == GLFW_PRESS || act == GLFW_REPEAT) ? -5.0 : 0.0;
    }
    if (key == GLFW_KEY_RIGHT) {
        d->ctrl[0] = (act == GLFW_PRESS || act == GLFW_REPEAT) ? 5.0 : 0.0;
    }
}

// Mouse button callback
void mouse_button(GLFWwindow* window, int button, int act, int mods) {
    button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    lastx = xpos;
    lasty = ypos;
}

// Mouse move callback
void mouse_move(GLFWwindow* window, double xpos, double ypos) {
    if (!button_left && !button_middle && !button_right) {
        return;
    }
    
    double dx = xpos - lastx;
    double dy = ypos - lasty;
    lastx = xpos;
    lasty = ypos;
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    
    // Camera control
    mjv_moveCamera(m, mjMOUSE_ROTATE_V + button_right, dx/height, dy/height, &scn, &cam);
}

// Initialize visualization
void init_visualization() {
    // Initialize visualization structures
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);
    
    // Create scene
    mjv_makeScene(m, &scn, 2000);
    
    // Set camera
    cam.distance = 3.0;
    cam.azimuth = 90;
    cam.elevation = -20;
    cam.lookat[0] = 0;
    cam.lookat[1] = 0;
    cam.lookat[2] = 0.3;
}

int main(int argc, char** argv) {
    // Load MuJoCo model
    char error[1000] = "Could not load model";
    m = mj_loadXML("mujoco/cartpole.xml", nullptr, error, sizeof(error));
    if (!m) {
        std::cerr << "Load model error: " << error << std::endl;
        return 1;
    }
    
    // Create data
    d = mj_makeData(m);
    
    // Initialize GLFW
    if (!glfwInit()) {
        mju_error("Could not initialize GLFW");
    }
    
    // Create window
    window = glfwCreateWindow(1200, 900, "Cart-Pole MuJoCo Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        mju_error("Could not create GLFW window");
    }
    
    // Make context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Set callbacks
    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_button);
    
    // Initialize visualization
    init_visualization();
    mjr_makeContext(m, &con, mjFONTSCALE_150);
    
    // Print instructions
    std::cout << "=== Cart-Pole Simulation ===" << std::endl;
    std::cout << "Use arrow keys to apply force to cart" << std::endl;
    std::cout << "Press 'R' to reset simulation" << std::endl;
    std::cout << "Press 'ESC' to exit" << std::endl;
    std::cout << "Mouse: drag to rotate camera" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Main simulation loop
    while (!glfwWindowShouldClose(window)) {
        // Advance simulation
        mjtNum simstart = d->time;
        while (d->time - simstart < 1.0/60.0) {
            mj_step(m, d);
        }
        
        // Get window size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // Update scene
        mjv_updateScene(m, d, &opt, nullptr, &cam, mjCAT_ALL, &scn);
        
        // Render
        mjrRect viewport = {0, 0, width, height};
        mjr_render(viewport, &scn, &con);
        
        // Show info overlay
        char info[100];
        snprintf(info, sizeof(info), 
                 "Time: %.2f\nCart Pos: %.3f\nPole Angle: %.3f rad (%.1f deg)\nCart Vel: %.3f\nPole Vel: %.3f",
                 d->time, d->qpos[0], d->qpos[1], d->qpos[1] * 180.0 / M_PI, d->qvel[0], d->qvel[1]);
        mjr_overlay(mjFONT_NORMAL, mjGRID_TOPLEFT, viewport, info, nullptr, &con);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    mjv_freeScene(&scn);
    mjr_freeContext(&con);
    mj_deleteData(d);
    mj_deleteModel(m);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
