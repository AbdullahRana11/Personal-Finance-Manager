#pragma once
// ============================================================================
// FinancialState.h — State Pattern (Bonus)
// ============================================================================
// The user's financial context can be in one of several modes:
//   BudgetingMode, SavingsMode, InvestmentMode
// Each state changes how recommendations and behaviors work.
// ============================================================================

#include <string>
#include <vector>
#include <iostream>
#include <memory>

class FinanceContext; // Forward declaration

class IFinancialState {
public:
    virtual ~IFinancialState() = default;
    virtual std::string getStateName() const = 0;
    virtual std::vector<std::string> getRecommendations(double income, double expenses, double savings) const = 0;
    virtual void handleTransition(FinanceContext& ctx) const = 0;
};

class FinanceContext {
    std::shared_ptr<IFinancialState> m_state;
    double m_income, m_expenses, m_savings;
public:
    FinanceContext() : m_income(0), m_expenses(0), m_savings(0) {}
    void setState(std::shared_ptr<IFinancialState> s) {
        m_state = std::move(s);
        std::cout << "[State] Transitioned to: " << m_state->getStateName() << "\n";
    }
    void setFinancials(double inc, double exp, double sav) {
        m_income = inc; m_expenses = exp; m_savings = sav;
    }
    std::string getCurrentState() const { return m_state ? m_state->getStateName() : "None"; }
    std::vector<std::string> getRecommendations() const {
        return m_state->getRecommendations(m_income, m_expenses, m_savings);
    }
    void evaluateTransition() { m_state->handleTransition(*this); }
    double getIncome() const { return m_income; }
    double getExpenses() const { return m_expenses; }
    double getSavings() const { return m_savings; }
};

// ─── Concrete States ────────────────────────────────────────────────────────

class BudgetingMode : public IFinancialState {
public:
    std::string getStateName() const override { return "Budgeting Mode"; }
    std::vector<std::string> getRecommendations(double income, double expenses, double) const override {
        std::vector<std::string> recs;
        recs.push_back("Focus on tracking every expense to understand spending patterns.");
        if (expenses > income * 0.9)
            recs.push_back("WARNING: Spending is >90% of income. Cut discretionary expenses immediately.");
        recs.push_back("Consider using the 50/30/20 rule to structure your budget.");
        recs.push_back("Set up automatic bill payments to avoid late fees.");
        return recs;
    }
    void handleTransition(FinanceContext& ctx) const override;
};

class SavingsMode : public IFinancialState {
public:
    std::string getStateName() const override { return "Savings Mode"; }
    std::vector<std::string> getRecommendations(double income, double, double savings) const override {
        std::vector<std::string> recs;
        recs.push_back("Build an emergency fund covering 3-6 months of expenses.");
        if (savings < income * 3)
            recs.push_back("Your emergency fund is below target. Prioritize saving.");
        recs.push_back("Consider a high-yield savings account for better returns.");
        recs.push_back("Automate transfers to savings on payday.");
        return recs;
    }
    void handleTransition(FinanceContext& ctx) const override;
};

class InvestmentMode : public IFinancialState {
public:
    std::string getStateName() const override { return "Investment Mode"; }
    std::vector<std::string> getRecommendations(double income, double, double savings) const override {
        std::vector<std::string> recs;
        recs.push_back("Diversify investments across stocks, bonds, and mutual funds.");
        recs.push_back("Consider index funds for low-cost, broad market exposure.");
        if (savings > income * 6)
            recs.push_back("Strong savings position! Explore higher-return investments.");
        recs.push_back("Rebalance your portfolio quarterly.");
        return recs;
    }
    void handleTransition(FinanceContext& ctx) const override;
};

// ─── Transition logic (defined after all classes) ───────────────────────────

inline void BudgetingMode::handleTransition(FinanceContext& ctx) const {
    double ratio = ctx.getExpenses() / (ctx.getIncome() > 0 ? ctx.getIncome() : 1);
    if (ratio < 0.7) {
        std::cout << "[State] Expenses under control. Transitioning to Savings Mode.\n";
        ctx.setState(std::make_shared<SavingsMode>());
    }
}

inline void SavingsMode::handleTransition(FinanceContext& ctx) const {
    if (ctx.getSavings() > ctx.getIncome() * 6) {
        std::cout << "[State] Strong savings. Transitioning to Investment Mode.\n";
        ctx.setState(std::make_shared<InvestmentMode>());
    } else if (ctx.getExpenses() > ctx.getIncome() * 0.9) {
        std::cout << "[State] Spending too high. Back to Budgeting Mode.\n";
        ctx.setState(std::make_shared<BudgetingMode>());
    }
}

inline void InvestmentMode::handleTransition(FinanceContext& ctx) const {
    if (ctx.getSavings() < ctx.getIncome() * 2) {
        std::cout << "[State] Savings depleted. Back to Savings Mode.\n";
        ctx.setState(std::make_shared<SavingsMode>());
    }
}
