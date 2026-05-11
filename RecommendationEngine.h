#pragma once
// ============================================================================
// RecommendationEngine.h — AI-Driven Financial Recommendations
// ============================================================================
// SOLID: Single Responsibility — Only generates recommendations.
// SOLID: High Cohesion — Focused solely on analysis and advice.
// ============================================================================

#include "Database.h"
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>

class RecommendationEngine {
public:
    struct Recommendation {
        std::string category;   // "spending", "savings", "investment"
        std::string priority;   // "HIGH", "MEDIUM", "LOW"
        std::string message;
    };

    std::vector<Recommendation> generateRecommendations() const {
        DatabaseConnection& db = DatabaseConnection::getInstance();
        std::map<std::string, double> summary = db.getCategorySummary();
        std::vector<Recommendation> recs;

        double totalIncome = 0, totalExpenses = 0;
        for (std::map<std::string, double>::const_iterator it = summary.begin();
             it != summary.end(); ++it) {
            if (it->first == "income") totalIncome += it->second;
            else totalExpenses += std::abs(it->second);
        }

        // 1. Savings rate analysis
        double savingsRate = (totalIncome > 0) ? ((totalIncome - totalExpenses) / totalIncome * 100) : 0;
        std::ostringstream oss;
        if (savingsRate < 10) {
            oss << "Your savings rate is only " << (int)savingsRate
                << "%. Aim for at least 20%. Cut discretionary spending.";
            Recommendation r = {"savings", "HIGH", oss.str()};
            recs.push_back(r);
        } else if (savingsRate < 20) {
            oss << "Savings rate is " << (int)savingsRate
                << "%. Good progress, try to reach 20%.";
            Recommendation r = {"savings", "MEDIUM", oss.str()};
            recs.push_back(r);
        } else {
            oss << "Excellent savings rate of " << (int)savingsRate
                << "%! Consider investing surplus funds.";
            Recommendation r = {"savings", "LOW", oss.str()};
            recs.push_back(r);
        }

        // 2. Category-specific analysis
        for (std::map<std::string, double>::const_iterator it = summary.begin();
             it != summary.end(); ++it) {
            if (it->first == "income") continue;
            double pct = (totalIncome > 0) ? (std::abs(it->second) / totalIncome * 100) : 0;
            if (it->first == "dining" && pct > 15) {
                std::ostringstream s;
                s << "Dining expenses are " << (int)pct << "% of income. Try meal prepping to reduce by 50%.";
                Recommendation r = {"spending", "HIGH", s.str()};
                recs.push_back(r);
            }
            if (it->first == "entertainment" && pct > 10) {
                std::ostringstream s;
                s << "Entertainment is " << (int)pct << "% of income. Look for free alternatives.";
                Recommendation r = {"spending", "MEDIUM", s.str()};
                recs.push_back(r);
            }
            if (it->first == "shopping" && pct > 12) {
                std::ostringstream s;
                s << "Shopping at " << (int)pct << "% of income. Use 48-hour rule before purchases.";
                Recommendation r = {"spending", "MEDIUM", s.str()};
                recs.push_back(r);
            }
        }

        // 3. Investment recommendations
        double investAmt = 0;
        std::map<std::string, double>::const_iterator invIt = summary.find("investment");
        if (invIt != summary.end()) investAmt = std::abs(invIt->second);
        double investPct = (totalIncome > 0) ? (investAmt / totalIncome * 100) : 0;
        if (investPct < 5) {
            Recommendation r = {"investment", "HIGH",
                "Investment allocation is very low. Start with index funds for diversified exposure."};
            recs.push_back(r);
        } else if (investPct < 15) {
            Recommendation r = {"investment", "MEDIUM",
                "Consider diversifying: mix stocks, bonds, and mutual funds."};
            recs.push_back(r);
        } else {
            Recommendation r = {"investment", "LOW",
                "Strong investment allocation! Review portfolio quarterly for rebalancing."};
            recs.push_back(r);
        }

        // 4. Emergency fund check
        double emergencyFund = 0;
        std::map<std::string, double>::const_iterator efIt = summary.find("emergency_fund");
        if (efIt != summary.end()) emergencyFund = std::abs(efIt->second);
        if (emergencyFund < totalExpenses * 3) {
            Recommendation r = {"savings", "HIGH",
                "Emergency fund below 3-month expense coverage. Prioritize building this up."};
            recs.push_back(r);
        }

        return recs;
    }

    void displayRecommendations() const {
        std::vector<Recommendation> recs = generateRecommendations();
        std::cout << "\n============ AI FINANCIAL RECOMMENDATIONS ============\n";
        for (size_t i = 0; i < recs.size(); i++) {
            std::string icon = (recs[i].priority == "HIGH") ? "!!!" :
                               (recs[i].priority == "MEDIUM") ? " ! " : " . ";
            std::cout << "  " << (i+1) << ". [" << icon << "] ["
                      << recs[i].category << "] " << recs[i].message << "\n";
        }
        std::cout << "======================================================\n";
    }
};
