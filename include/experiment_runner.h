#ifndef EXPERIMENT_RUNNER_H
#define EXPERIMENT_RUNNER_H

#include "environment.h"
#include "agent.h"
#include "config.h"
#include <memory>
#include <vector>
#include <string>
#include <fstream>

/**
 * Manages RL experiments with standardized training loops and logging.
 */
class ExperimentRunner {
public:
    struct ExperimentConfig {
        int num_episodes = 1000;
        int max_steps_per_episode = 1000;
        bool render = false;
        int render_frequency = 10;  // Render every N episodes
        int log_frequency = 100;    // Log stats every N episodes
        std::string log_file = "experiment.log";
        bool save_model = false;
        std::string model_save_path = "model.bin";
    };
    
    struct EpisodeStats {
        int episode;
        int steps;
        double total_reward;
        bool terminated;
        std::string termination_reason;
        std::vector<std::pair<std::string, double>> agent_stats;
    };
    
    ExperimentRunner(std::unique_ptr<Environment> env, std::unique_ptr<Agent> agent);
    
    // Run experiment with given configuration
    std::vector<EpisodeStats> runExperiment(const ExperimentConfig& config);
    
    // Run experiment from config file
    std::vector<EpisodeStats> runExperimentFromConfig(const std::string& config_file);
    
    // Run single episode (useful for evaluation)
    EpisodeStats runEpisode(int max_steps = 1000, bool render = false);
    
    // Getters
    Environment* getEnvironment() const { return env_.get(); }
    Agent* getAgent() const { return agent_.get(); }
    
    // Logging
    void logStats(const std::vector<EpisodeStats>& stats, const std::string& filename);
    void printStats(const EpisodeStats& stats);
    void printSummary(const std::vector<EpisodeStats>& all_stats);

private:
    std::unique_ptr<Environment> env_;
    std::unique_ptr<Agent> agent_;
    
    void logToFile(const std::string& message, const std::string& filename);
    double calculateMovingAverage(const std::vector<EpisodeStats>& stats, int window_size = 100);
};

#endif // EXPERIMENT_RUNNER_H
