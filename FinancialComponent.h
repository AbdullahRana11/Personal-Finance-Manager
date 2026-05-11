#pragma once
// ============================================================================
// FinancialComponent.h — Composite Pattern
// ============================================================================
// SOLID: Open/Closed Principle — New account types (e.g., CryptoAccount) can
// be added by simply creating a new subclass of Account. No existing code
// needs to be modified.
//
// SOLID: Liskov Substitution — Any FinancialComponent pointer can point to
// either a Transaction (leaf) or an Account (composite) interchangeably.
//
// Design Pattern: COMPOSITE
// - FinancialComponent is the abstract component.
// - Transaction is the leaf node (individual financial entry).
// - Account is the composite (contains transactions and sub-accounts).
// ============================================================================

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>

// ─── Abstract Component ─────────────────────────────────────────────────────

class FinancialComponent {
public:
    virtual ~FinancialComponent() = default;

    virtual std::string getName() const = 0;
    virtual double getBalance() const = 0;
    virtual void display(int indent = 0) const = 0;

    // Composite operations — default no-op for leaves
    virtual void add(std::shared_ptr<FinancialComponent> /*component*/) {
        throw std::runtime_error("Cannot add to a leaf component.");
    }
    virtual void remove(const std::string& /*name*/) {
        throw std::runtime_error("Cannot remove from a leaf component.");
    }
    virtual std::vector<std::shared_ptr<FinancialComponent>> getChildren() const {
        return {};
    }
};

// ─── Leaf: Transaction ──────────────────────────────────────────────────────

class Transaction : public FinancialComponent {
private:
    std::string m_description;
    double m_amount;        // positive = credit, negative = debit
    std::string m_category;
    std::string m_date;

public:
    Transaction(const std::string& desc, double amount,
                const std::string& category, const std::string& date)
        : m_description(desc), m_amount(amount),
          m_category(category), m_date(date) {}

    std::string getName() const override { return m_description; }
    double getBalance() const override { return m_amount; }
    std::string getCategory() const { return m_category; }
    std::string getDate() const { return m_date; }

    void display(int indent = 0) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "[Transaction] " << m_date << " | "
                  << std::setw(30) << std::left << m_description
                  << " | " << std::setw(10) << std::right
                  << std::fixed << std::setprecision(2) << m_amount
                  << " | " << m_category << "\n";
    }
};

// ─── Composite: Account ─────────────────────────────────────────────────────
// An Account can contain Transactions (leaves) and/or sub-Accounts (composites).
// For example, an "Investments" account might contain sub-accounts for
// "Stocks", "Bonds", and "Mutual Funds".

class Account : public FinancialComponent {
private:
    std::string m_name;
    std::string m_type; // "savings", "checking", "investment"
    std::vector<std::shared_ptr<FinancialComponent>> m_children;

public:
    Account(const std::string& name, const std::string& type)
        : m_name(name), m_type(type) {}

    std::string getName() const override { return m_name; }
    std::string getType() const { return m_type; }

    // Balance is the recursive sum of all children
    double getBalance() const override {
        double total = 0.0;
        for (const auto& child : m_children) {
            total += child->getBalance();
        }
        return total;
    }

    void add(std::shared_ptr<FinancialComponent> component) override {
        m_children.push_back(component);
    }

    void remove(const std::string& name) override {
        m_children.erase(
            std::remove_if(m_children.begin(), m_children.end(),
                [&name](const std::shared_ptr<FinancialComponent>& c) {
                    return c->getName() == name;
                }),
            m_children.end());
    }

    std::vector<std::shared_ptr<FinancialComponent>> getChildren() const override {
        return m_children;
    }

    void display(int indent = 0) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "[Account: " << m_type << "] " << m_name
                  << "  (Balance: $" << std::fixed << std::setprecision(2)
                  << getBalance() << ")\n";
        for (const auto& child : m_children) {
            child->display(indent + 1);
        }
    }

    // Convenience: get only Transaction leaves from this account
    std::vector<std::shared_ptr<Transaction>> getTransactions() const {
        std::vector<std::shared_ptr<Transaction>> txns;
        for (const auto& child : m_children) {
            auto txn = std::dynamic_pointer_cast<Transaction>(child);
            if (txn) {
                txns.push_back(txn);
            } else {
                // Recurse into sub-accounts
                auto acc = std::dynamic_pointer_cast<Account>(child);
                if (acc) {
                    auto subTxns = acc->getTransactions();
                    txns.insert(txns.end(), subTxns.begin(), subTxns.end());
                }
            }
        }
        return txns;
    }
};

// ─── Concrete Account Types ─────────────────────────────────────────────────
// Open/Closed: Adding a new account type requires NO modification to Account.

class SavingsAccount : public Account {
public:
    explicit SavingsAccount(const std::string& name)
        : Account(name, "savings") {}
};

class CheckingAccount : public Account {
public:
    explicit CheckingAccount(const std::string& name)
        : Account(name, "checking") {}
};

class InvestmentAccount : public Account {
public:
    explicit InvestmentAccount(const std::string& name)
        : Account(name, "investment") {}
};
