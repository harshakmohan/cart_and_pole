# Reinforcement Learning Simulation Framework

This repository is a **modular sandbox for reinforcement learning (RL)** focused on **deep understanding of algorithms through simulation experiments**.

The goal is to create a clean, minimal framework that makes it easy to **implement RL algorithms, experiment with different approaches, and gain intuition about how they work** on various dynamical systems.

It currently contains a **CartPole environment** implemented in C++ using MuJoCo physics, with a plugin architecture for easily adding new algorithms and environments.

---

## Vision
This project is designed for **deep RL exploration and learning**:

Key principles:  
- **Modular Design**: Swap algorithms and environments without touching existing code
- **Minimal & Direct**: Clean interfaces with essential functionality only
- **Experimentation-Focused**: Easy to implement, test, and compare different RL approaches
- **Algorithm Understanding**: Framework designed to make RL concepts clear and intuitive

This repo serves as a foundation for implementing and understanding RL algorithms from the ground up.

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
- `./test_env` - Test RL environment wrapper (original)
- `./test_framework` - Test modular RL framework

## Modular RL Framework

The framework provides a plugin architecture for RL algorithms and environments:

### Core Components
- **Environment**: Abstract base class for all environments
- **Agent**: Abstract base class for all RL algorithms  
- **ExperimentRunner**: Standardized training and evaluation loops
- **Factories**: Registry system for dynamic creation of environments and agents

### Available Environments
- `cartpole` - Classic cart-pole control task using MuJoCo

### Available Agents
- `random` - Baseline random agent
- `rule_based` - Simple bang-bang controller for CartPole

### Usage
```cpp
// Create environment and agent from config
auto env = EnvironmentFactory::create("cartpole", config);
auto agent = AgentFactory::create("rule_based", config);

// Run experiment
ExperimentRunner runner(std::move(env), std::move(agent));
auto stats = runner.runExperiment(experiment_config);
```

## Files

- `src/main.cpp` - Interactive simulation
- `src/test_framework.cpp` - Framework integration test
- `include/environment.h` - Environment base class
- `include/agent.h` - Agent base class
- `src/cartpole_env.cpp` - CartPole environment implementation
- `src/agents/` - Agent implementations
- `config/` - Example experiment configurations

---

## Next Steps

### Phase 3: Learning Algorithms
Now that the modular framework is complete, you can implement actual RL algorithms:

**Policy Gradient Methods:**
- **REINFORCE**: Simple policy gradient with episode-based learning
- **PPO**: Proximal Policy Optimization with clipped objective
- **A2C**: Actor-Critic with advantage estimation

**Value-Based Methods:**
- **DQN**: Deep Q-Network with experience replay
- **Double DQN**: Improved target estimation
- **Dueling DQN**: Separate value and advantage streams

**Actor-Critic Methods:**
- **SAC**: Soft Actor-Critic for continuous control
- **TD3**: Twin Delayed Deep Deterministic policy gradient

### Adding New Algorithms
1. Implement the `Agent` interface in `src/agents/your_agent.cpp`
2. Register with factory: `AgentFactory::registerAgent("your_agent", creator)`
3. Test with existing environments immediately

### Adding New Environments
1. Implement the `Environment` interface 
2. Register with factory: `EnvironmentFactory::registerEnvironment("env_name", creator)`
3. Compatible with all existing agents automatically

---

## Algorithm Implementation Guide

Each RL algorithm you implement will contain:
- **Policy**: How to choose actions (neural network, lookup table, etc.)
- **Learning**: How to improve from experience (gradient updates, Q-learning, etc.)
- **Memory**: How to store/use past experiences (replay buffer, episode buffer, etc.)

The `Agent` interface handles this cleanly:
```cpp
class YourAgent : public Agent {
    Action act(const State& state) override;     // Policy
    void learn(const Experience& exp) override;  // Learning algorithm
    // Your networks, buffers, hyperparameters...
};
```

## Getting Started with Your First Algorithm

Ready to implement your first learning algorithm? Here's a suggested path:

1. **Start Simple**: Implement REINFORCE (policy gradient) - it's conceptually straightforward
2. **Test & Compare**: Run it against the rule-based agent on CartPole
3. **Iterate**: Try different network architectures, hyperparameters, reward functions
4. **Add Complexity**: Move to PPO or DQN once you understand the basics
5. **New Environments**: Add Pendulum or other control tasks to test generalization

The framework is designed to make this exploration smooth and educational!
