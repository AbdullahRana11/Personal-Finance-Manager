# Lab 14: Personal Finance Management System with AI Recommendations

## Build & Run

```bash
g++ -std=c++14 -o FinanceManager.exe main.cpp
./FinanceManager.exe
```

---

## Class Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        SINGLETON PATTERN                                │
│  ┌───────────────────────────────────────┐                              │
│  │      DatabaseConnection               │                              │
│  ├───────────────────────────────────────┤                              │
│  │ - m_transactions: vector<Transaction> │                              │
│  │ - m_goals: vector<GoalRecord>         │                              │
│  │ - m_nextTransactionId: int            │                              │
│  │ - m_nextGoalId: int                   │                              │
│  │ - DatabaseConnection() [private]      │                              │
│  ├───────────────────────────────────────┤                              │
│  │ + getInstance(): DatabaseConnection&  │ ◄── Static, returns single   │
│  │ + addTransaction(...)  : int          │     instance every time       │
│  │ + getAllTransactions()                 │                              │
│  │ + addGoal(...) : int                  │                              │
│  │ + updateGoalProgress(id, amt) : bool  │                              │
│  │ + getCategorySummary() : map          │                              │
│  └───────────────────────────────────────┘                              │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                        COMPOSITE PATTERN                                │
│                                                                         │
│  ┌───────────────────────────────┐                                      │
│  │  <<abstract>>                 │                                      │
│  │  FinancialComponent           │                                      │
│  ├───────────────────────────────┤                                      │
│  │ + getName() : string          │                                      │
│  │ + getBalance() : double       │                                      │
│  │ + display(indent) : void      │                                      │
│  │ + add(component) : void       │                                      │
│  │ + remove(name) : void         │                                      │
│  │ + getChildren() : vector      │                                      │
│  └──────────┬────────────────────┘                                      │
│             │                                                           │
│      ┌──────┴──────┐                                                    │
│      │             │                                                    │
│  ┌───▼───┐    ┌────▼──────────────────────┐                             │
│  │ Leaf  │    │ Composite                 │                             │
│  │       │    │                           │                             │
│  │Trans- │    │  Account                  │                             │
│  │action │    │  - m_children: vector<    │                             │
│  │       │    │      FinancialComponent>  │                             │
│  │-amount│    │  + getBalance() [recurse] │                             │
│  │-date  │    │  + add() / remove()       │                             │
│  │-categ.│    │                           │                             │
│  └───────┘    └────┬──────────────────────┘                             │
│                    │                                                    │
│          ┌─────────┼──────────┐                                         │
│          │         │          │                                          │
│    ┌─────▼──┐ ┌────▼───┐ ┌───▼────────┐                                │
│    │Checking│ │Savings │ │Investment  │                                 │
│    │Account │ │Account │ │Account     │                                 │
│    └────────┘ └────────┘ └────────────┘                                 │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                        STRATEGY PATTERN                                 │
│                                                                         │
│  ┌─────────────────────┐    uses     ┌──────────────────────────┐       │
│  │   BudgetPlanner     │───────────►│ <<interface>>             │       │
│  │   (Context)         │            │ IBudgetStrategy           │       │
│  ├─────────────────────┤            ├──────────────────────────┤       │
│  │ - m_strategy: ptr   │            │ + getName() : string     │       │
│  ├─────────────────────┤            │ + getDescription()       │       │
│  │ + setStrategy(s)    │            │ + allocate(income, map)  │       │
│  │ + displayPlan(...)  │            │ + displayPlan(...)       │       │
│  └─────────────────────┘            └──────────┬───────────────┘       │
│                                                 │                       │
│                              ┌──────────────────┼──────────────┐        │
│                              │                  │              │        │
│                     ┌────────▼───┐  ┌───────────▼┐  ┌─────────▼──┐     │
│                     │50/30/20    │  │Zero-Based  │  │Envelope    │     │
│                     │Rule        │  │Budgeting   │  │Budgeting   │     │
│                     │            │  │            │  │            │     │
│                     │Needs  50%  │  │Every $     │  │Fixed caps  │     │
│                     │Wants  30%  │  │assigned    │  │per category│     │
│                     │Save   20%  │  │to a job    │  │            │     │
│                     └────────────┘  └────────────┘  └────────────┘     │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                     STATE PATTERN (Bonus)                                │
│                                                                         │
│  ┌──────────────────┐   delegates   ┌─────────────────────────┐         │
│  │ FinanceContext    │─────────────►│ <<interface>>            │         │
│  ├──────────────────┤              │ IFinancialState          │         │
│  │ - m_state: ptr   │              ├─────────────────────────┤         │
│  │ - m_income       │              │ + getStateName()        │         │
│  │ - m_expenses     │              │ + getRecommendations()  │         │
│  │ - m_savings      │              │ + handleTransition(ctx) │         │
│  ├──────────────────┤              └──────────┬──────────────┘         │
│  │ + setState(s)    │                         │                         │
│  │ + evaluateTrans. │          ┌──────────────┼────────────┐            │
│  └──────────────────┘          │              │            │            │
│                       ┌────────▼──┐ ┌─────────▼─┐ ┌───────▼────┐       │
│                       │Budgeting  │ │Savings    │ │Investment  │       │
│                       │Mode       │ │Mode       │ │Mode        │       │
│                       │           │ │           │ │            │       │
│                       │Track every│ │Build      │ │Diversify   │       │
│                       │expense    │ │emergency  │ │portfolio   │       │
│                       │           │ │fund       │ │            │       │
│                       └─────┬─────┘ └──┬────┬──┘ └─────┬──────┘       │
│                             │          │    │          │               │
│              exp<70% income │          │    │          │ sav<2x income │
│                    ─────────┘          │    │          └──────         │
│                        sav>6x income   │    │ exp>90% income          │
│                              ──────────┘    └──────                   │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                    HIGH COHESION MODULES                                 │
│                                                                         │
│  ┌─────────────────────┐  ┌──────────────────┐  ┌──────────────────┐   │
│  │TransactionCategorizer│  │   GoalTracker    │  │RecommendEngine   │   │
│  ├─────────────────────┤  ├──────────────────┤  ├──────────────────┤   │
│  │ ONLY categorizes    │  │ ONLY tracks      │  │ ONLY generates   │   │
│  │ transactions using  │  │ goals, progress, │  │ AI-driven tips   │   │
│  │ keyword AI          │  │ and deadlines    │  │ from spending    │   │
│  └─────────────────────┘  └──────────────────┘  └──────────────────┘   │
│                                                                         │
│  ┌─────────────────────┐                                                │
│  │  ReportGenerator    │                                                │
│  ├─────────────────────┤                                                │
│  │ ONLY generates      │                                                │
│  │ ASCII reports and   │                                                │
│  │ charts              │                                                │
│  └─────────────────────┘                                                │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## SOLID Principles — Detailed Mapping

### S — Single Responsibility Principle

Each class has exactly **one reason to change**:

| Class | Single Responsibility |
|-------|----------------------|
| `DatabaseConnection` | Data storage and retrieval ONLY |
| `TransactionCategorizer` | Categorizing transactions ONLY |
| `GoalTracker` | Goal CRUD and progress tracking ONLY |
| `RecommendationEngine` | Generating financial advice ONLY |
| `ReportGenerator` | Producing formatted reports ONLY |
| `BudgetPlanner` | Delegating to a budget strategy ONLY |

**Evidence**: The `RecommendationEngine` does NOT manage transactions or accounts — it reads from the database and produces advice. Each module can be modified independently.

### O — Open/Closed Principle

The system is **open for extension** but **closed for modification**:

- **New budget strategy**: Create a new class inheriting `IBudgetStrategy` → no changes to `BudgetPlanner` or existing strategies.
- **New account type**: Create a new class inheriting `Account` (e.g., `CryptoAccount`) → no changes to `FinancialComponent` hierarchy.
- **New financial state**: Create a new class inheriting `IFinancialState` → no changes to `FinanceContext`.
- **New transaction category**: Call `categorizer.addKeyword("crypto", "investment")` → no source code changes.

### L — Liskov Substitution Principle

Anywhere a base type is expected, any subtype works correctly:

```cpp
// Any IBudgetStrategy* works in BudgetPlanner
planner->setStrategy(make_shared<FiftyThirtyTwentyRule>());   // ✓
planner->setStrategy(make_shared<ZeroBasedBudgeting>());      // ✓
planner->setStrategy(make_shared<EnvelopeBudgeting>());       // ✓

// Any FinancialComponent* works in Account::add()
account->add(make_shared<Transaction>(...));    // leaf ✓
account->add(make_shared<SavingsAccount>(...)); // composite ✓
```

### I — Interface Segregation Principle

Interfaces are small and focused — no class is forced to implement methods it doesn't use:

- `IBudgetStrategy`: Only `getName()`, `getDescription()`, `allocate()` — no reporting, no goal tracking
- `IFinancialState`: Only `getStateName()`, `getRecommendations()`, `handleTransition()` — no budgeting
- `FinancialComponent`: Only `getName()`, `getBalance()`, `display()` — composite operations (`add`/`remove`) have default implementations that throw for leaves

### D — Dependency Inversion Principle

High-level modules depend on **abstractions**, not concrete classes:

```
BudgetPlanner  ──depends on──►  IBudgetStrategy (abstract)
                                    ▲
                                    │ implements
          ┌─────────────────────────┼──────────────────┐
   FiftyThirtyTwenty       ZeroBasedBudgeting    EnvelopeBudgeting

FinanceContext ──depends on──► IFinancialState (abstract)
                                    ▲
                                    │ implements
          ┌─────────────────────────┼──────────────────┐
      BudgetingMode            SavingsMode        InvestmentMode
```

---

## Design Patterns Summary

| Pattern | Type | Problem Solved | Classes |
|---------|------|---------------|---------|
| **Singleton** | Creational | One shared database connection | `DatabaseConnection` |
| **Composite** | Structural | Uniform treatment of accounts and transactions in a tree hierarchy | `FinancialComponent`, `Transaction`, `Account` |
| **Strategy** | Behavioral | Swappable budgeting algorithms at runtime | `IBudgetStrategy`, `BudgetPlanner`, 3 strategies |
| **State** | Behavioral | Different behavior per financial mode with auto-transitions | `IFinancialState`, `FinanceContext`, 3 modes |

---

## Features Demonstrated

1. **Transaction Categorization** — AI keyword matching classifies 20 transactions into 11 categories
2. **Goal Tracking** — 4 financial goals with progress bars and contribution tracking
3. **Financial Recommendations** — Savings rate, spending pattern, and investment analysis
4. **Reporting** — ASCII bar charts for income vs expenses and category breakdowns
5. **State Transitions** — Budgeting → Savings → Investment with per-mode recommendations

---

## File Dependencies

```
main.cpp
├── Database.h                (no dependencies)
├── FinancialComponent.h      (no dependencies)
├── BudgetStrategy.h          (no dependencies)
├── FinancialState.h          (no dependencies)
├── TransactionCategorizer.h  (no dependencies)
├── GoalTracker.h             (→ Database.h)
├── RecommendationEngine.h    (→ Database.h)
└── ReportGenerator.h         (→ Database.h)
```
