#pragma once
// ============================================================================
// BudgetStrategy.h — Strategy Pattern
// ============================================================================
// SOLID: Open/Closed — New strategies via subclassing, no existing code changes.
// SOLID: Dependency Inversion — BudgetPlanner depends on IBudgetStrategy*.
// SOLID: Interface Segregation — IBudgetStrategy defines only budgeting ops.
//
// Design Pattern: STRATEGY
// - IBudgetStrategy: strategy interface
// - FiftyThirtyTwentyRule, ZeroBasedBudgeting, EnvelopeBudgeting: concrete
// - BudgetPlanner: context that delegates to a strategy
// ============================================================================

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <cmath>

struct BudgetAllocation {
    std::string category;
    double allocatedAmount;
    double actualSpending;
    double difference;
    std::string status;
};

class IBudgetStrategy {
public:
    virtual ~IBudgetStrategy() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<BudgetAllocation> allocate(
        double totalIncome,
        const std::map<std::string, double>& categorySpending) const = 0;

    virtual void displayPlan(double totalIncome,
                             const std::map<std::string, double>& spending) const {
        std::cout << "\n========================================\n";
        std::cout << "  Budget Strategy: " << getName() << "\n";
        std::cout << "  " << getDescription() << "\n";
        std::cout << "  Total Income: $" << std::fixed << std::setprecision(2) << totalIncome << "\n";
        std::cout << "========================================\n";
        auto allocations = allocate(totalIncome, spending);
        std::cout << std::left << std::setw(18) << "Category"
                  << std::right << std::setw(12) << "Allocated"
                  << std::setw(12) << "Actual" << std::setw(12) << "Diff" << "  Status\n";
        std::cout << std::string(60, '-') << "\n";
        for (size_t i = 0; i < allocations.size(); i++) {
            const BudgetAllocation& a = allocations[i];
            std::cout << std::left << std::setw(18) << a.category
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2) << a.allocatedAmount
                      << std::setw(12) << a.actualSpending << std::setw(12) << a.difference
                      << "  " << a.status << "\n";
        }
    }
};

// ─── Concrete Strategy: 50/30/20 Rule ───────────────────────────────────────

class FiftyThirtyTwentyRule : public IBudgetStrategy {
public:
    std::string getName() const override { return "50/30/20 Rule"; }
    std::string getDescription() const override { return "50% Needs | 30% Wants | 20% Savings"; }

    std::vector<BudgetAllocation> allocate(double totalIncome,
        const std::map<std::string, double>& categorySpending) const override {
        double needs = 0, wants = 0, savingsInv = 0;
        for (std::map<std::string, double>::const_iterator it = categorySpending.begin();
             it != categorySpending.end(); ++it) {
            std::string cat = it->first;
            double absAmt = std::abs(it->second);
            if (cat == "rent" || cat == "utilities" || cat == "groceries" ||
                cat == "healthcare" || cat == "transport" || cat == "insurance")
                needs += absAmt;
            else if (cat == "savings" || cat == "investment" || cat == "emergency_fund")
                savingsInv += absAmt;
            else if (cat != "income")
                wants += absAmt;
        }
        std::vector<BudgetAllocation> result;
        BudgetAllocation a1 = {"Needs (50%)", totalIncome*0.50, needs, totalIncome*0.50 - needs,
                                (needs <= totalIncome*0.50) ? "on-track" : "OVER-BUDGET"};
        BudgetAllocation a2 = {"Wants (30%)", totalIncome*0.30, wants, totalIncome*0.30 - wants,
                                (wants <= totalIncome*0.30) ? "on-track" : "OVER-BUDGET"};
        BudgetAllocation a3 = {"Savings (20%)", totalIncome*0.20, savingsInv, totalIncome*0.20 - savingsInv,
                                (savingsInv <= totalIncome*0.20) ? "on-track" : "OVER-BUDGET"};
        result.push_back(a1);
        result.push_back(a2);
        result.push_back(a3);
        return result;
    }
};

// ─── Concrete Strategy: Zero-Based Budgeting ────────────────────────────────

class ZeroBasedBudgeting : public IBudgetStrategy {
public:
    std::string getName() const override { return "Zero-Based Budgeting"; }
    std::string getDescription() const override { return "Every dollar assigned. Income - Expenses = 0."; }

    std::vector<BudgetAllocation> allocate(double totalIncome,
        const std::map<std::string, double>& categorySpending) const override {
        std::vector<BudgetAllocation> allocs;
        double totalAlloc = 0;
        for (std::map<std::string, double>::const_iterator it = categorySpending.begin();
             it != categorySpending.end(); ++it) {
            if (it->first == "income") continue;
            double a = std::abs(it->second);
            BudgetAllocation ba = {it->first, a, a, 0, "assigned"};
            allocs.push_back(ba);
            totalAlloc += a;
        }
        double rem = totalIncome - totalAlloc;
        BudgetAllocation remainder = {"** UNALLOCATED **", 0, 0, rem,
                          (std::abs(rem) < 0.01) ? "PERFECT" : "needs-assignment"};
        allocs.push_back(remainder);
        return allocs;
    }
};

// ─── Concrete Strategy: Envelope Budgeting ──────────────────────────────────

class EnvelopeBudgeting : public IBudgetStrategy {
    std::map<std::string, double> m_envelopes;
public:
    EnvelopeBudgeting() {
        m_envelopes["groceries"]     = 400;
        m_envelopes["dining"]        = 200;
        m_envelopes["transport"]     = 150;
        m_envelopes["entertainment"] = 100;
        m_envelopes["utilities"]     = 200;
        m_envelopes["shopping"]      = 150;
        m_envelopes["healthcare"]    = 100;
        m_envelopes["savings"]       = 500;
        m_envelopes["investment"]    = 300;
    }
    void setEnvelope(const std::string& cat, double amt) { m_envelopes[cat] = amt; }
    std::string getName() const override { return "Envelope Budgeting"; }
    std::string getDescription() const override { return "Fixed envelopes per category."; }

    std::vector<BudgetAllocation> allocate(double,
        const std::map<std::string, double>& spending) const override {
        std::vector<BudgetAllocation> allocs;
        for (std::map<std::string, double>::const_iterator it = m_envelopes.begin();
             it != m_envelopes.end(); ++it) {
            double actual = 0;
            std::map<std::string, double>::const_iterator found = spending.find(it->first);
            if (found != spending.end()) actual = std::abs(found->second);
            BudgetAllocation ba = {it->first, it->second, actual, it->second - actual,
                              (actual <= it->second) ? "within-envelope" : "EXCEEDED"};
            allocs.push_back(ba);
        }
        return allocs;
    }
};

// ─── Context: BudgetPlanner ─────────────────────────────────────────────────

class BudgetPlanner {
    std::shared_ptr<IBudgetStrategy> m_strategy;
public:
    explicit BudgetPlanner(std::shared_ptr<IBudgetStrategy> s) : m_strategy(s) {}
    void setStrategy(std::shared_ptr<IBudgetStrategy> s) {
        m_strategy = s;
        std::cout << "[BudgetPlanner] Strategy changed to: " << m_strategy->getName() << "\n";
    }
    std::string getStrategyName() const { return m_strategy ? m_strategy->getName() : "None"; }
    std::vector<BudgetAllocation> plan(double inc, const std::map<std::string, double>& sp) const {
        return m_strategy->allocate(inc, sp);
    }
    void displayPlan(double inc, const std::map<std::string, double>& sp) const {
        m_strategy->displayPlan(inc, sp);
    }
};
