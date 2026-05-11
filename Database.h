#pragma once
// ============================================================================
// Database.h — Singleton Pattern
// ============================================================================
// SOLID: Single Responsibility — This class is ONLY responsible for managing
// the database connection and storing/retrieving raw financial records.
// It does NOT categorize, analyze, or recommend anything.
//
// Design Pattern: SINGLETON
// - Ensures exactly one database connection exists throughout the application.
// - Provides a global access point via getInstance().
// - Thread-safe lazy initialization (C++11 magic statics).
// ============================================================================

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>


// ─── Data structures stored in the "database" ───────────────────────────────

struct TransactionRecord {
    int id;
    std::string date;
    std::string description;
    double amount;           // positive = income, negative = expense
    std::string category;    // "income", "food", "transport", "investment", etc.
    std::string accountName; // which account this belongs to
};

struct GoalRecord {
    int id;
    std::string name;
    double targetAmount;
    double currentAmount;
    std::string deadline;
};

// ─── Singleton Database Connection ──────────────────────────────────────────

class DatabaseConnection {
private:
    // Private constructor — prevents external instantiation
    DatabaseConnection() : m_nextTransactionId(1), m_nextGoalId(1) {
        std::cout << "[Database] Connection established (Singleton instance created).\n";
    }

    // Delete copy/move to enforce single instance
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    DatabaseConnection(DatabaseConnection&&) = delete;
    DatabaseConnection& operator=(DatabaseConnection&&) = delete;

    // In-memory storage (simulates a real database)
    std::vector<TransactionRecord> m_transactions;
    std::vector<GoalRecord> m_goals;
    int m_nextTransactionId;
    int m_nextGoalId;


public:
    // ── Global access point ─────────────────────────────────────────────────
    // C++11 guarantees thread-safe initialization of function-local statics.
    static DatabaseConnection& getInstance() {
        static DatabaseConnection instance;
        return instance;
    }

    // ── Transaction CRUD ────────────────────────────────────────────────────
    int addTransaction(const std::string& date, const std::string& desc,
                       double amount, const std::string& category,
                       const std::string& account) {
        // Single-threaded demo — mutex omitted for MinGW compatibility
        TransactionRecord rec;
        rec.id = m_nextTransactionId++;
        rec.date = date;
        rec.description = desc;
        rec.amount = amount;
        rec.category = category;
        rec.accountName = account;
        m_transactions.push_back(rec);
        return rec.id;
    }

    std::vector<TransactionRecord> getAllTransactions() const {
        return m_transactions;
    }

    std::vector<TransactionRecord> getTransactionsByCategory(const std::string& cat) const {
        std::vector<TransactionRecord> result;
        for (const auto& t : m_transactions) {
            if (t.category == cat) result.push_back(t);
        }
        return result;
    }

    std::vector<TransactionRecord> getTransactionsByAccount(const std::string& acc) const {
        std::vector<TransactionRecord> result;
        for (const auto& t : m_transactions) {
            if (t.accountName == acc) result.push_back(t);
        }
        return result;
    }

    // ── Goal CRUD ───────────────────────────────────────────────────────────
    int addGoal(const std::string& name, double target, const std::string& deadline) {

        GoalRecord g;
        g.id = m_nextGoalId++;
        g.name = name;
        g.targetAmount = target;
        g.currentAmount = 0.0;
        g.deadline = deadline;
        m_goals.push_back(g);
        return g.id;
    }

    bool updateGoalProgress(int goalId, double amount) {

        for (auto& g : m_goals) {
            if (g.id == goalId) {
                g.currentAmount += amount;
                return true;
            }
        }
        return false;
    }

    std::vector<GoalRecord> getAllGoals() const {
        return m_goals;
    }

    // ── Aggregate helpers ───────────────────────────────────────────────────
    double getTotalByCategory(const std::string& cat) const {
        double total = 0;
        for (const auto& t : m_transactions) {
            if (t.category == cat) total += t.amount;
        }
        return total;
    }

    std::map<std::string, double> getCategorySummary() const {
        std::map<std::string, double> summary;
        for (const auto& t : m_transactions) {
            summary[t.category] += t.amount;
        }
        return summary;
    }

    void clearAll() {

        m_transactions.clear();
        m_goals.clear();
        m_nextTransactionId = 1;
        m_nextGoalId = 1;
    }
};
