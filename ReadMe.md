# Reinforcement Learning Simulation Sandbox

This repository is a **modular sandbox for reinforcement learning (RL)** with an emphasis on **deploying agents onto embedded robotic systems with limited compute resources**.  

The aim is not just to train agents in simulation, but to create a lightweight, reproducible framework that makes it easy to **prototype algorithms, benchmark them across environments, and eventually run them on real robotic hardware**.

It currently contains a minimal implementation of a **CartPole dynamical system** in C++ using the MuJoCo physics engine.

---

## Long-Term Vision
This project is guided by the goal of bridging **simulation → reproducible experiments → embedded deployment**.  

Key principles:  
- **Swappable environments** (CartPole → Pendulum → custom robotics tasks).  
- **Swappable agents** (RL algorithms) without modifying environment code.  
- **Reproducible experiments** via config files and consistent logging.  
- **Comparable results** through standardized visualization (learning curves, benchmarks, policy rollouts).  
- **Deployability**: agents designed to run efficiently on embedded devices (e.g., NVIDIA Jetson) using C++ or ONNX/TensorRT inference.  

This repo will serve as a framework to accelerate RL research by making it easy to apply new algorithms and rigorously benchmark them — both in simulation and on real robotic systems.

---


## Setup

1. Install dependencies:
```bash
brew install cmake glfw
```

2. Install MuJoCo (macOS):
   - Download and install MuJoCo.app from: https://github.com/google-deepmind/mujoco/releases
   - The build system will automatically find it at `/Applications/MuJoCo.app`

3. Build:
```bash
mkdir build && cd build
cmake ..
make
```

## Run

- `./cartpole` - Interactive demo (use arrow keys)
- `./test_env` - Test RL environment wrapper

## Files

- `src/main.cpp` - Interactive simulation
- `src/cartpole_env.cpp` - RL environment interface
- `mujoco/cartpole.xml` - MuJoCo model definition

---

## Future Work

This project is still in its early stages. The roadmap below outlines the milestones that will guide development:

1. **Baseline Algorithms**
   - Implement a shared `Agent` interface
   - Implement PPO and DQN agents with the shared `Agent` interface
   - Provide modular config-driven training (YAML for hyperparams, env properties, etc.)

2. **Expanded Environments**
   - Add additional baseline MuJoCo environments (Pendulum, MountainCar, etc.)
   - Add simple robotics manipulation tasks (e.g. basic grasping, pushing an object, etc.)  
   - Build an interface to enable integration with real robots (method and architecture TBD)

3. **Experiment Management**
   - Implement consistent logging and visualization (episode rewards, training curves, etc.) to benchmark algorithms across environments

4. **Embedded Deployment**
   - Export trained agents to portable format for embedded deployment
   - Run optimized inference on embedded device (e.g. Nvidia Jetson)
   - Benchmark runtime performance and efficiency on embedded hardware.

5. **Open Source Release**
   - Polish documentation and examples.
   - Publish reproducible comparison of results/benchmarks.
   - Position the repo as a minimal, modular RL framework designed for **simulation → reproducibility → embedded deployment**.
