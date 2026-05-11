#pragma once
// ============================================================================
// GoalTracker.h — Financial Goal Tracking
// ============================================================================
// SOLID: Single Responsibility — Only tracks goals and progress.
// ============================================================================

#include "Database.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

class GoalTracker {
public:
    int createGoal(const std::string& name, double target, const std::string& deadline) {
        auto& db = DatabaseConnection::getInstance();
        int id = db.addGoal(name, target, deadline);
        std::cout << "[GoalTracker] Created goal '" << name << "' (ID: " << id
                  << ") — Target: $" << std::fixed << std::setprecision(2) << target
                  << " by " << deadline << "\n";
        return id;
    }

    bool contributeToGoal(int goalId, double amount) {
        auto& db = DatabaseConnection::getInstance();
        bool ok = db.updateGoalProgress(goalId, amount);
        if (ok)
            std::cout << "[GoalTracker] Added $" << std::fixed << std::setprecision(2)
                      << amount << " to goal ID " << goalId << "\n";
        return ok;
    }

    void displayGoals() const {
        auto& db = DatabaseConnection::getInstance();
        auto goals = db.getAllGoals();
        std::cout << "\n=============== FINANCIAL GOALS ===============\n";
        std::cout << std::left << std::setw(5) << "ID"
                  << std::setw(25) << "Goal"
                  << std::right << std::setw(12) << "Target"
                  << std::setw(12) << "Current"
                  << std::setw(10) << "Progress"
                  << "  Deadline\n";
        std::cout << std::string(72, '-') << "\n";
        for (const auto& g : goals) {
            double pct = (g.targetAmount > 0) ? (g.currentAmount / g.targetAmount * 100) : 0;
            std::string bar = "[";
            int filled = static_cast<int>(pct / 10);
            for (int i = 0; i < 10; i++) bar += (i < filled) ? "#" : ".";
            bar += "]";
            std::cout << std::left << std::setw(5) << g.id
                      << std::setw(25) << g.name
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2) << g.targetAmount
                      << std::setw(12) << g.currentAmount
                      << std::setw(10) << bar
                      << "  " << g.deadline << "\n";
        }
    }
};
