#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>
#include <tuple>
#include <string>
#include <memory>

// Type aliases for clarity
using State = std::vector<double>;
using Action = double;  // For continuous control (can be extended to vector later)
using Reward = double;
using Done = bool;
using Info = std::string;

// Step result tuple: (next_state, reward, done, info)
using StepResult = std::tuple<State, Reward, Done, Info>;

// Experience tuple for learning: (state, action, reward, next_state, done)
struct Experience {
    State state;
    Action action;
    Reward reward;
    State next_state;
    Done done;
    
    Experience(const State& s, Action a, Reward r, const State& ns, Done d)
        : state(s), action(a), reward(r), next_state(ns), done(d) {}
};

/**
 * Abstract base class for all RL environments.
 * Provides the standard interface that all environments must implement.
 */
class Environment {
public:
    virtual ~Environment() = default;
    
    // Core environment interface (OpenAI Gym style)
    virtual State reset() = 0;
    virtual StepResult step(Action action) = 0;
    virtual void render() = 0;
    virtual void close() = 0;
    
    // Environment metadata
    virtual int getObservationSpaceSize() const = 0;
    virtual int getActionSpaceSize() const = 0;
    virtual std::vector<double> getObservationSpaceLow() const = 0;
    virtual std::vector<double> getObservationSpaceHigh() const = 0;
    virtual double getActionSpaceLow() const = 0;
    virtual double getActionSpaceHigh() const = 0;
    
    // Environment identification
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Optional: Get current state without stepping
    virtual State getCurrentState() const = 0;
    
    // Optional: Set rendering mode
    virtual void setRenderMode(bool render) {}
};

#endif // ENVIRONMENT_H
