# Educational RL Framework - CartPole Swing-Up

This repository is a **clean, minimal framework for learning reinforcement learning (RL)** through hands-on implementation and experimentation.

The goal is to provide a solid foundation for **implementing RL algorithms from scratch**, understanding how they work, and experimenting with different approaches on control problems.

It features a **CartPole swing-up environment** implemented in C++ using MuJoCo physics, with a modular architecture that makes it easy to add and compare different RL algorithms.

---

## 🎯 Vision

This project is designed for **deep RL learning and understanding**:

**Key Principles:**  
- **Educational Focus**: Learn RL by implementing algorithms yourself
- **Modular Design**: Easy to swap in new algorithms and compare them
- **Minimal & Direct**: Clean interfaces without unnecessary complexity
- **Hands-On Learning**: Understand RL through implementation, not just theory

**Perfect for:** Students, researchers, and practitioners who want to understand RL algorithms deeply by building them from the ground up.

---

## 🚀 Setup

1. **Install dependencies:**
```bash
brew install cmake glfw
```

2. **Install MuJoCo (macOS):**
   - Download MuJoCo.app from: https://github.com/google-deepmind/mujoco/releases
   - Install to `/Applications/MuJoCo.app`

3. **Build:**
```bash
mkdir build && cd build
cmake ..
make
```

## 🎮 Run

- `./build/cartpole` - **Interactive control** (use arrow keys to swing up the pole manually)
- `./build/test_env` - **Agent demonstration** (rule-based agent attempts swing-up)

## 🧠 The Learning Environment

**CartPole Swing-Up Task:**
- **Challenge**: Swing a pole from hanging down to balanced upright
- **State**: [cart_position, cart_velocity, pole_angle, pole_angular_velocity] 
- **Action**: Continuous force applied to cart (-10 to +10 N)
- **Reward**: `cos(pole_angle) + 1` (0 when hanging down, 2 when upright)
- **Physics**: Realistic MuJoCo simulation

This is a **much more challenging** problem than standard CartPole balancing, requiring energy pumping and careful control strategies.

---

## 🏗️ Architecture

The framework uses clean inheritance for modularity:

```cpp
// Base classes for modularity
class Environment {
    virtual State reset() = 0;
    virtual StepResult step(Action action) = 0;
    // ...
};

class Agent {
    virtual Action act(const State& state) = 0;      // Policy
    virtual void learn(const Experience& exp) = 0;   // Learning
    // ...
};

// Implementations
class CartPoleEnv : public Environment { /* ... */ };
class RuleBasedAgent : public Agent { /* ... */ };
```

**Why this works for learning:**
- **Easy to add new algorithms**: Just inherit from `Agent` and implement `act()` + `learn()`
- **Easy to compare**: Same interface for all algorithms
- **Educational clarity**: Separation between policy (act) and learning (learn) is explicit

---

## 📚 Implementing Your First RL Algorithm

Here's how you'd add a simple REINFORCE agent:

```cpp
class REINFORCEAgent : public Agent {
private:
    PolicyNetwork policy_;
    std::vector<Experience> episode_buffer_;
    double learning_rate_;

public:
    Action act(const State& state) override {
        // Get action from policy network + exploration
        return policy_.sample_action(state);
    }
    
    void learn(const Experience& exp) override {
        // Store experience
        episode_buffer_.push_back(exp);
        
        // When episode ends, update policy
        if (exp.done) {
            update_policy_gradients();
            episode_buffer_.clear();
        }
    }
};

// Use it the same way as any other agent:
std::unique_ptr<Agent> agent = std::make_unique<REINFORCEAgent>(learning_rate);
```

**The beauty**: Once you implement the `Agent` interface, your algorithm works with any environment automatically!

---

## 🎓 Learning Path

**Suggested progression for learning RL:**

1. **Understand the Problem**: 
   - Run `./build/cartpole` and try to swing up the pole manually
   - Observe how difficult it is - this motivates why we need RL!

2. **Study the Rule-Based Agent**:
   - Look at `src/agents/rule_based_agent.cpp` 
   - See why simple rules fail at swing-up (energy pumping needed)

3. **Implement Your First RL Algorithm**:
   - Start with **REINFORCE** (policy gradient)
   - Simple conceptually: increase probability of good actions
   - Learn about policy networks, episode collection, gradient ascent

4. **Add Complexity Gradually**:
   - **Actor-Critic** methods (A2C, PPO)
   - **Value-based** methods (DQN for discrete version)
   - **Advanced** methods (SAC, TD3)

5. **Experiment and Compare**:
   - Compare learning curves between algorithms
   - Try different hyperparameters, network architectures
   - Understand why some algorithms work better than others

---

## 📁 Codebase Structure

```
src/main.cpp                 - Interactive manual control
src/test_env.cpp             - Agent demonstration (shows polymorphism)
src/cartpole_env.cpp         - CartPole environment implementation  
src/agents/rule_based_agent.cpp - Simple baseline controller

include/environment.h        - Environment base class
include/agent.h              - Agent base class  
include/cartpole_env.h       - CartPole environment header
include/rule_based_agent.h   - Rule-based agent header

mujoco/cartpole.xml          - Physics model definition
CMakeLists.txt               - Build system
```

**Total**: ~1,000 lines of clean, focused code (down from 9,000+ lines of over-engineered framework!)

---

## 🔬 Why This Environment?

**CartPole Swing-Up** is perfect for learning RL because:

- **Non-trivial**: Requires actual learning (simple rules fail)
- **Continuous control**: Most real-world problems are continuous
- **Fast simulation**: Quick iteration for learning experiments  
- **Intuitive**: You can visualize and understand what's happening
- **Scalable complexity**: Easy problem to start, but deep optimization possible

**Educational Value**: You'll understand energy pumping, exploration vs exploitation, credit assignment, and policy optimization in a concrete, visual way.

---

## 🚀 Ready to Start?

1. **Get familiar**: Run the interactive and agent demos
2. **Study the code**: Understand the Environment/Agent interfaces  
3. **Implement REINFORCE**: Your first learning algorithm
4. **Experiment**: Try different approaches and see what works
5. **Compare**: Implement multiple algorithms and compare their learning

The framework is designed to make this journey smooth and educational. Happy learning! 🎓