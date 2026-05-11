// ============================================================================
// main.cpp — Personal Finance Management System Demo
// ============================================================================
// Lab 14: Demonstrates SOLID Principles + Design Patterns
//   - Singleton  (DatabaseConnection)
//   - Composite  (Account/Transaction hierarchy)
//   - Strategy   (BudgetPlanner with swappable strategies)
//   - State      (FinanceContext with BudgetingMode/SavingsMode/InvestmentMode)
// ============================================================================

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "Database.h"
#include "FinancialComponent.h"
#include "BudgetStrategy.h"
#include "FinancialState.h"
#include "TransactionCategorizer.h"
#include "GoalTracker.h"
#include "RecommendationEngine.h"
#include "ReportGenerator.h"

// Helper to print section headers
void printHeader(const std::string& title) {
    std::cout << "\n\n";
    std::cout << "  +------------------------------------------------------------+\n";
    int pad = (60 - (int)title.size()) / 2;
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(60 - pad - (int)title.size(), ' ') << "|\n";
    std::cout << "  +------------------------------------------------------------+\n\n";
}

int main() {
    std::cout << R"(
    ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___
   |   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   |
   | P || E || R || S || O || N || A || L ||   || F || I || N |
   |___||___||___||___||___||___||___||___||___||___||___||___|
   |   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   |
   | A || N || C || E ||   || M || A || N || A || G || E || R |
   |___||___||___||___||___||___||___||___||___||___||___||___|

   Lab 14 — SOLID Principles & Design Patterns in C++
    )" << std::endl;

    // ================================================================
    // DEMO 1: SINGLETON PATTERN — Database Connection
    // ================================================================
    printHeader("DEMO 1: SINGLETON — Database Connection");

    auto& db1 = DatabaseConnection::getInstance();
    auto& db2 = DatabaseConnection::getInstance();

    std::cout << "  db1 address: " << &db1 << "\n";
    std::cout << "  db2 address: " << &db2 << "\n";
    std::cout << "  Same instance? " << ((&db1 == &db2) ? "YES" : "NO")
              << " (Singleton verified!)\n";

    // ================================================================
    // DEMO 2: AI TRANSACTION CATEGORIZATION
    // ================================================================
    printHeader("DEMO 2: AI Transaction Categorization");

    TransactionCategorizer categorizer;

    struct RawTransaction {
        std::string date, desc;
        double amount;
        std::string account;
    };

    std::vector<RawTransaction> rawData = {
        {"2025-05-01", "Monthly Salary",           5000.00,  "checking"},
        {"2025-05-02", "Freelance Web Development", 1500.00, "checking"},
        {"2025-05-03", "Walmart Grocery Shopping",  -120.50, "checking"},
        {"2025-05-04", "Uber Ride to Office",       -25.00,  "checking"},
        {"2025-05-05", "Netflix Subscription",      -15.99,  "checking"},
        {"2025-05-06", "Restaurant dinner with friends",-85.00,"checking"},
        {"2025-05-07", "Amazon Electronics Purchase",-299.99,"checking"},
        {"2025-05-08", "Gym Membership",            -50.00,  "checking"},
        {"2025-05-09", "Monthly Rent Payment",      -1200.00,"checking"},
        {"2025-05-10", "Electric Bill Utility",     -95.00,  "checking"},
        {"2025-05-11", "Savings Account Deposit",   -500.00, "savings"},
        {"2025-05-12", "Stock Investment - AAPL",   -300.00, "investment"},
        {"2025-05-13", "Mutual Fund Contribution",  -200.00, "investment"},
        {"2025-05-14", "Emergency Fund Transfer",   -400.00, "savings"},
        {"2025-05-15", "Coffee at Starbucks Cafe",  -6.50,   "checking"},
        {"2025-05-16", "Phone Bill Payment",        -70.00,  "checking"},
        {"2025-05-17", "Parking Garage Fee",        -15.00,  "checking"},
        {"2025-05-18", "Dividend Income",            120.00, "investment"},
        {"2025-05-19", "Spotify Premium",           -9.99,   "checking"},
        {"2025-05-20", "Doctor Visit Copay",        -30.00,  "checking"},
    };

    std::cout << "  Categorizing " << rawData.size() << " transactions using AI...\n\n";
    std::cout << std::left << std::setw(14) << "  Date"
              << std::setw(32) << "Description"
              << std::right << std::setw(10) << "Amount"
              << "  -> Category\n";
    std::cout << "  " << std::string(72, '-') << "\n";

    for (const auto& raw : rawData) {
        std::string category = categorizer.categorize(raw.desc);
        // Store in singleton database
        db1.addTransaction(raw.date, raw.desc, raw.amount, category, raw.account);

        std::cout << "  " << std::left << std::setw(12) << raw.date
                  << std::setw(32) << raw.desc
                  << std::right << std::setw(10) << std::fixed
                  << std::setprecision(2) << raw.amount
                  << "  -> " << category << "\n";
    }

    // ================================================================
    // DEMO 3: COMPOSITE PATTERN — Account Hierarchy
    // ================================================================
    printHeader("DEMO 3: COMPOSITE — Account Hierarchy");

    // Build a composite tree of accounts
    auto portfolio = std::make_shared<Account>("My Portfolio", "root");

    auto checking = std::make_shared<CheckingAccount>("Primary Checking");
    auto savings  = std::make_shared<SavingsAccount>("High-Yield Savings");
    auto investments = std::make_shared<InvestmentAccount>("Investment Portfolio");

    // Sub-accounts under investments
    auto stocks = std::make_shared<InvestmentAccount>("Stocks");
    auto mutualFunds = std::make_shared<InvestmentAccount>("Mutual Funds");

    // Add transactions to accounts
    checking->add(std::make_shared<Transaction>("Salary", 5000, "income", "2025-05-01"));
    checking->add(std::make_shared<Transaction>("Freelance", 1500, "income", "2025-05-02"));
    checking->add(std::make_shared<Transaction>("Groceries", -120.50, "groceries", "2025-05-03"));
    checking->add(std::make_shared<Transaction>("Rent", -1200, "rent", "2025-05-09"));
    checking->add(std::make_shared<Transaction>("Utilities", -95, "utilities", "2025-05-10"));

    savings->add(std::make_shared<Transaction>("Deposit", -500, "savings", "2025-05-11"));
    savings->add(std::make_shared<Transaction>("Emergency Fund", -400, "emergency_fund", "2025-05-14"));

    stocks->add(std::make_shared<Transaction>("AAPL Purchase", -300, "investment", "2025-05-12"));
    stocks->add(std::make_shared<Transaction>("Dividend", 120, "income", "2025-05-18"));
    mutualFunds->add(std::make_shared<Transaction>("Index Fund", -200, "investment", "2025-05-13"));

    investments->add(stocks);
    investments->add(mutualFunds);

    portfolio->add(checking);
    portfolio->add(savings);
    portfolio->add(investments);

    std::cout << "  Account hierarchy (Composite pattern):\n\n";
    portfolio->display(2);

    std::cout << "\n  Portfolio Total Balance: $" << std::fixed
              << std::setprecision(2) << portfolio->getBalance() << "\n";

    // ================================================================
    // DEMO 4: STRATEGY PATTERN — Budget Planning
    // ================================================================
    printHeader("DEMO 4: STRATEGY — Budget Planning");

    std::map<std::string, double> summary = db1.getCategorySummary();
    double totalIncome = 0;
    for (std::map<std::string, double>::const_iterator it = summary.begin();
         it != summary.end(); ++it)
        if (it->first == "income") totalIncome += it->second;
    if (totalIncome == 0) totalIncome = 6620.0; // fallback

    // Strategy 1: 50/30/20 Rule
    auto planner = std::make_shared<BudgetPlanner>(
        std::make_shared<FiftyThirtyTwentyRule>());
    planner->displayPlan(totalIncome, summary);

    // Strategy 2: Swap to Zero-Based at runtime
    std::cout << "\n  --- Swapping strategy at runtime ---\n";
    planner->setStrategy(std::make_shared<ZeroBasedBudgeting>());
    planner->displayPlan(totalIncome, summary);

    // Strategy 3: Swap to Envelope
    std::cout << "\n  --- Swapping strategy again ---\n";
    planner->setStrategy(std::make_shared<EnvelopeBudgeting>());
    planner->displayPlan(totalIncome, summary);

    // ================================================================
    // DEMO 5: GOAL TRACKING
    // ================================================================
    printHeader("DEMO 5: Goal Tracking");

    GoalTracker goalTracker;
    int g1 = goalTracker.createGoal("Emergency Fund", 10000.0, "2025-12-31");
    int g2 = goalTracker.createGoal("Vacation to Japan", 5000.0, "2026-06-01");
    int g3 = goalTracker.createGoal("New Laptop", 2000.0, "2025-09-01");
    int g4 = goalTracker.createGoal("Retirement Fund", 50000.0, "2035-01-01");

    goalTracker.contributeToGoal(g1, 400.0);
    goalTracker.contributeToGoal(g1, 600.0);
    goalTracker.contributeToGoal(g2, 1200.0);
    goalTracker.contributeToGoal(g3, 800.0);
    goalTracker.contributeToGoal(g3, 450.0);
    goalTracker.contributeToGoal(g4, 2500.0);

    goalTracker.displayGoals();

    // ================================================================
    // DEMO 6: AI RECOMMENDATIONS
    // ================================================================
    printHeader("DEMO 6: AI Recommendations");

    RecommendationEngine engine;
    engine.displayRecommendations();

    // ================================================================
    // DEMO 7: FINANCIAL REPORTS
    // ================================================================
    printHeader("DEMO 7: Financial Reports");

    ReportGenerator reporter;
    reporter.generateFullReport();

    // ================================================================
    // DEMO 8 (BONUS): STATE PATTERN — Financial Modes
    // ================================================================
    printHeader("BONUS: STATE PATTERN — Financial Modes");

    FinanceContext ctx;
    ctx.setState(std::make_shared<BudgetingMode>());
    ctx.setFinancials(6620, 5800, 900);

    std::cout << "\n  Current State: " << ctx.getCurrentState() << "\n";
    std::cout << "  Recommendations in this state:\n";
    for (const auto& r : ctx.getRecommendations())
        std::cout << "    - " << r << "\n";

    // Evaluate if transition should happen
    // expenses/income = 5800/6620 = 87.6% — too high, stays in Budgeting
    std::cout << "\n  Evaluating state transition (expenses = 87.6% of income)...\n";
    ctx.evaluateTransition();

    // Adjust financials: now expenses are under control
    ctx.setFinancials(6620, 4000, 900);
    std::cout << "\n  Updated: expenses now 60% of income.\n";
    std::cout << "  Evaluating transition...\n";
    ctx.evaluateTransition();  // Should transition to SavingsMode

    std::cout << "\n  Current State: " << ctx.getCurrentState() << "\n";
    std::cout << "  Recommendations:\n";
    for (const auto& r : ctx.getRecommendations())
        std::cout << "    - " << r << "\n";

    // Now bump savings to trigger Investment Mode
    ctx.setFinancials(6620, 4000, 50000);
    std::cout << "\n  Updated: savings now 7.5x income (above 6x threshold).\n";
    std::cout << "  Evaluating transition...\n";
    ctx.evaluateTransition();  // Should transition to InvestmentMode

    std::cout << "\n  Current State: " << ctx.getCurrentState() << "\n";
    std::cout << "  Recommendations:\n";
    for (const auto& r : ctx.getRecommendations())
        std::cout << "    - " << r << "\n";

    // ================================================================
    // SUMMARY
    // ================================================================
    printHeader("DESIGN PATTERNS SUMMARY");

    std::cout << R"(
  1. SINGLETON (Creational):
     - DatabaseConnection::getInstance() ensures ONE shared connection.
     - Verified: &db1 == &db2 (same address).

  2. COMPOSITE (Structural):
     - FinancialComponent -> Transaction (leaf) + Account (composite).
     - Accounts can contain transactions AND sub-accounts.
     - getBalance() recursively aggregates the entire tree.

  3. STRATEGY (Behavioral):
     - IBudgetStrategy interface with 3 concrete strategies:
       * FiftyThirtyTwentyRule, ZeroBasedBudgeting, EnvelopeBudgeting
     - BudgetPlanner swaps strategies at runtime via setStrategy().

  4. STATE (Behavioral - Bonus):
     - IFinancialState with 3 modes:
       * BudgetingMode -> SavingsMode -> InvestmentMode
     - Automatic transitions based on financial health metrics.
     - Different recommendations per state.

  SOLID PRINCIPLES APPLIED:
  -------------------------
  S - Single Responsibility: Each class has ONE job
      (DB manages data, Categorizer categorizes, Reporter reports, etc.)
  O - Open/Closed: New strategies/accounts/states via subclassing,
      NO modification to existing code.
  L - Liskov Substitution: Any IBudgetStrategy works in BudgetPlanner.
      Any FinancialComponent works in Account.
  I - Interface Segregation: IBudgetStrategy only defines budgeting ops.
      IFinancialState only defines state ops.
  D - Dependency Inversion: BudgetPlanner depends on IBudgetStrategy*,
      NOT concrete classes. High-level modules depend on abstractions.
)" << std::endl;

    return 0;
}
