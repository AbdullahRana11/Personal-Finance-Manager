#pragma once
// ============================================================================
// ReportGenerator.h — Financial Reporting
// ============================================================================
// SOLID: Single Responsibility — Only generates reports/visualizations.
// ============================================================================

#include "Database.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <utility>

class ReportGenerator {
public:
    void generateIncomeVsExpenseReport() const {
        DatabaseConnection& db = DatabaseConnection::getInstance();
        std::map<std::string, double> summary = db.getCategorySummary();

        double totalIncome = 0, totalExpenses = 0;
        for (std::map<std::string, double>::const_iterator it = summary.begin();
             it != summary.end(); ++it) {
            if (it->first == "income") totalIncome += it->second;
            else if (it->second < 0) totalExpenses += std::abs(it->second);
        }

        std::cout << "\n============ INCOME vs EXPENSES REPORT ============\n\n";
        std::cout << "  Total Income:   $" << std::fixed << std::setprecision(2) << totalIncome << "\n";
        std::cout << "  Total Expenses: $" << totalExpenses << "\n";
        std::cout << "  Net:            $" << (totalIncome - totalExpenses) << "\n\n";

        double maxVal = std::max(totalIncome, totalExpenses);
        int incBar = (maxVal > 0) ? static_cast<int>(totalIncome / maxVal * 40) : 0;
        int expBar = (maxVal > 0) ? static_cast<int>(totalExpenses / maxVal * 40) : 0;

        std::cout << "  Income   |" << std::string(incBar, '#') << "| $" << totalIncome << "\n";
        std::cout << "  Expenses |" << std::string(expBar, '=') << "| $" << totalExpenses << "\n";
    }

    void generateCategoryBreakdown() const {
        DatabaseConnection& db = DatabaseConnection::getInstance();
        std::map<std::string, double> summary = db.getCategorySummary();

        double totalExp = 0;
        std::vector<std::pair<std::string, double> > expenses;
        for (std::map<std::string, double>::const_iterator it = summary.begin();
             it != summary.end(); ++it) {
            if (it->first != "income" && it->second < 0) {
                expenses.push_back(std::make_pair(it->first, std::abs(it->second)));
                totalExp += std::abs(it->second);
            }
        }
        std::sort(expenses.begin(), expenses.end(),
                  [](const std::pair<std::string, double>& a,
                     const std::pair<std::string, double>& b) {
                      return a.second > b.second;
                  });

        std::cout << "\n============ EXPENSE CATEGORY BREAKDOWN ============\n\n";
        std::cout << std::left << std::setw(18) << "Category"
                  << std::right << std::setw(12) << "Amount"
                  << std::setw(10) << "Pct" << "  Chart\n";
        std::cout << std::string(60, '-') << "\n";

        for (size_t i = 0; i < expenses.size(); i++) {
            double pct = (totalExp > 0) ? (expenses[i].second / totalExp * 100) : 0;
            int barLen = static_cast<int>(pct / 2);
            std::string bar(barLen, '*');
            std::cout << std::left << std::setw(18) << expenses[i].first
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2)
                      << expenses[i].second
                      << std::setw(9) << std::setprecision(1) << pct << "%"
                      << "  " << bar << "\n";
        }
    }

    void generateFullReport() const {
        std::cout << "\n\n";
        std::cout << "  ===================================================\n";
        std::cout << "  |       PERSONAL FINANCE MANAGEMENT REPORT        |\n";
        std::cout << "  ===================================================\n";
        generateIncomeVsExpenseReport();
        generateCategoryBreakdown();
    }
};
