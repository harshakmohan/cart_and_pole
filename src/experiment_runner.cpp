#include "experiment_runner.h"
#include "environment_factory.h"
#include "agent_factory.h"
#include <iostream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <thread>

ExperimentRunner::ExperimentRunner(std::unique_ptr<Environment> env, std::unique_ptr<Agent> agent)
    : env_(std::move(env)), agent_(std::move(agent)) {
    if (!env_ || !agent_) {
        throw std::runtime_error("Environment and Agent must be valid");
    }
}

std::vector<ExperimentRunner::EpisodeStats> ExperimentRunner::runExperiment(const ExperimentConfig& config) {
    std::vector<EpisodeStats> all_stats;
    all_stats.reserve(config.num_episodes);
    
    std::cout << "Starting experiment with " << config.num_episodes << " episodes..." << std::endl;
    std::cout << "Environment: " << env_->getName() << std::endl;
    std::cout << "Agent: " << agent_->getName() << std::endl;
    std::cout << "================================" << std::endl;
    
    for (int episode = 0; episode < config.num_episodes; ++episode) {
        // Run episode
        bool should_render = config.render && (episode % config.render_frequency == 0);
        EpisodeStats stats = runEpisode(config.max_steps_per_episode, should_render);
        stats.episode = episode + 1;
        
        all_stats.push_back(stats);
        
        // Log progress
        if ((episode + 1) % config.log_frequency == 0) {
            printStats(stats);
            
            // Calculate moving average
            double moving_avg = calculateMovingAverage(all_stats, std::min(100, episode + 1));
            std::cout << "Moving average (last " << std::min(100, episode + 1) 
                      << " episodes): " << std::fixed << std::setprecision(2) 
                      << moving_avg << std::endl;
            std::cout << "--------------------------------" << std::endl;
        }
        
        // Reset agent for next episode
        agent_->reset();
    }
    
    // Print final summary
    printSummary(all_stats);
    
    // Save logs if requested
    if (!config.log_file.empty()) {
        logStats(all_stats, config.log_file);
    }
    
    // Save model if requested
    if (config.save_model && !config.model_save_path.empty()) {
        agent_->saveModel(config.model_save_path);
        std::cout << "Model saved to: " << config.model_save_path << std::endl;
    }
    
    return all_stats;
}

std::vector<ExperimentRunner::EpisodeStats> ExperimentRunner::runExperimentFromConfig(const std::string& config_file) {
    Config config = Config::fromJsonFile(config_file);
    
    // Create experiment config
    ExperimentConfig exp_config;
    exp_config.num_episodes = config.get<int>("num_episodes", 1000);
    exp_config.max_steps_per_episode = config.get<int>("max_steps_per_episode", 1000);
    exp_config.render = config.get<bool>("render", false);
    exp_config.render_frequency = config.get<int>("render_frequency", 10);
    exp_config.log_frequency = config.get<int>("log_frequency", 100);
    exp_config.log_file = config.get<std::string>("log_file", "experiment.log");
    exp_config.save_model = config.get<bool>("save_model", false);
    exp_config.model_save_path = config.get<std::string>("model_save_path", "model.bin");
    
    return runExperiment(exp_config);
}

ExperimentRunner::EpisodeStats ExperimentRunner::runEpisode(int max_steps, bool render) {
    EpisodeStats stats;
    stats.total_reward = 0.0;
    stats.steps = 0;
    stats.terminated = false;
    stats.termination_reason = "";
    
    // Reset environment
    State state = env_->reset();
    
    for (int step = 0; step < max_steps; ++step) {
        // Render if requested
        if (render) {
            env_->render();
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
        
        // Agent chooses action
        Action action = agent_->act(state);
        
        // Environment steps
        auto [next_state, reward, done, info] = env_->step(action);
        
        // Create experience for learning
        Experience exp(state, action, reward, next_state, done);
        agent_->learn(exp);
        
        // Update stats
        stats.total_reward += reward;
        stats.steps = step + 1;
        state = next_state;
        
        // Check if episode is done
        if (done) {
            stats.terminated = true;
            stats.termination_reason = info;
            break;
        }
    }
    
    // Get agent statistics
    stats.agent_stats = agent_->getStats();
    
    return stats;
}

void ExperimentRunner::logStats(const std::vector<EpisodeStats>& stats, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open log file: " << filename << std::endl;
        return;
    }
    
    // Write header
    file << "episode,steps,total_reward,terminated,termination_reason\n";
    
    // Write data
    for (const auto& stat : stats) {
        file << stat.episode << ","
             << stat.steps << ","
             << stat.total_reward << ","
             << (stat.terminated ? "true" : "false") << ","
             << stat.termination_reason << "\n";
    }
    
    std::cout << "Statistics logged to: " << filename << std::endl;
}

void ExperimentRunner::printStats(const EpisodeStats& stats) {
    std::cout << "Episode " << stats.episode 
              << ": Steps=" << stats.steps 
              << ", Reward=" << std::fixed << std::setprecision(2) << stats.total_reward
              << ", Terminated=" << (stats.terminated ? "Yes" : "No");
    
    if (stats.terminated && !stats.termination_reason.empty()) {
        std::cout << " (" << stats.termination_reason << ")";
    }
    
    std::cout << std::endl;
}

void ExperimentRunner::printSummary(const std::vector<EpisodeStats>& all_stats) {
    if (all_stats.empty()) return;
    
    // Calculate statistics
    double total_reward = 0.0;
    int total_steps = 0;
    int terminated_episodes = 0;
    
    for (const auto& stats : all_stats) {
        total_reward += stats.total_reward;
        total_steps += stats.steps;
        if (stats.terminated) terminated_episodes++;
    }
    
    double avg_reward = total_reward / all_stats.size();
    double avg_steps = static_cast<double>(total_steps) / all_stats.size();
    double termination_rate = static_cast<double>(terminated_episodes) / all_stats.size();
    
    std::cout << "\n======== EXPERIMENT SUMMARY ========" << std::endl;
    std::cout << "Total Episodes: " << all_stats.size() << std::endl;
    std::cout << "Average Reward: " << std::fixed << std::setprecision(2) << avg_reward << std::endl;
    std::cout << "Average Steps: " << std::fixed << std::setprecision(1) << avg_steps << std::endl;
    std::cout << "Termination Rate: " << std::fixed << std::setprecision(1) << (termination_rate * 100) << "%" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void ExperimentRunner::logToFile(const std::string& message, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
    }
}

double ExperimentRunner::calculateMovingAverage(const std::vector<EpisodeStats>& stats, int window_size) {
    if (stats.empty()) return 0.0;
    
    int start_idx = std::max(0, static_cast<int>(stats.size()) - window_size);
    double sum = 0.0;
    int count = 0;
    
    for (int i = start_idx; i < static_cast<int>(stats.size()); ++i) {
        sum += stats[i].total_reward;
        count++;
    }
    
    return count > 0 ? sum / count : 0.0;
}
