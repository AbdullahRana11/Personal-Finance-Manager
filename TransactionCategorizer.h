#pragma once
// ============================================================================
// TransactionCategorizer.h — AI-based Transaction Categorization
// ============================================================================
// SOLID: Single Responsibility — Only categorizes transactions.
// SOLID: Open/Closed — New keywords can be added without modifying core algo.
// ============================================================================

#include <string>
#include <map>
#include <vector>
#include <algorithm>

class TransactionCategorizer {
private:
    std::map<std::string, std::string> m_keywords;

    std::string toLower(const std::string& s) const {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

public:
    TransactionCategorizer() {
        // Income
        m_keywords["salary"]     = "income";
        m_keywords["paycheck"]   = "income";
        m_keywords["freelance"]  = "income";
        m_keywords["bonus"]      = "income";
        m_keywords["dividend"]   = "income";
        m_keywords["refund"]     = "income";
        // Food
        m_keywords["restaurant"] = "dining";
        m_keywords["cafe"]       = "dining";
        m_keywords["coffee"]     = "dining";
        m_keywords["pizza"]      = "dining";
        m_keywords["uber eats"]  = "dining";
        m_keywords["grocery"]    = "groceries";
        m_keywords["supermarket"]= "groceries";
        m_keywords["walmart"]    = "groceries";
        // Transport
        m_keywords["uber"]       = "transport";
        m_keywords["lyft"]       = "transport";
        m_keywords["gas"]        = "transport";
        m_keywords["fuel"]       = "transport";
        m_keywords["parking"]    = "transport";
        m_keywords["metro"]      = "transport";
        // Housing
        m_keywords["rent"]       = "rent";
        m_keywords["mortgage"]   = "rent";
        m_keywords["electric"]   = "utilities";
        m_keywords["water bill"] = "utilities";
        m_keywords["internet"]   = "utilities";
        m_keywords["phone bill"] = "utilities";
        // Entertainment
        m_keywords["netflix"]    = "entertainment";
        m_keywords["spotify"]    = "entertainment";
        m_keywords["movie"]      = "entertainment";
        m_keywords["gaming"]     = "entertainment";
        // Shopping
        m_keywords["amazon"]     = "shopping";
        m_keywords["clothing"]   = "shopping";
        m_keywords["shoes"]      = "shopping";
        m_keywords["electronics"]= "shopping";
        // Health
        m_keywords["pharmacy"]   = "healthcare";
        m_keywords["doctor"]     = "healthcare";
        m_keywords["hospital"]   = "healthcare";
        m_keywords["gym"]        = "healthcare";
        m_keywords["insurance"]  = "insurance";
        // Finance
        m_keywords["savings"]    = "savings";
        m_keywords["deposit"]    = "savings";
        m_keywords["investment"] = "investment";
        m_keywords["stock"]      = "investment";
        m_keywords["mutual fund"]= "investment";
        m_keywords["401k"]       = "investment";
        m_keywords["emergency"]  = "emergency_fund";
    }

    std::string categorize(const std::string& description) const {
        std::string lower = toLower(description);
        for (std::map<std::string, std::string>::const_iterator it = m_keywords.begin();
             it != m_keywords.end(); ++it) {
            if (lower.find(it->first) != std::string::npos) {
                return it->second;
            }
        }
        return "uncategorized";
    }

    void addKeyword(const std::string& keyword, const std::string& category) {
        m_keywords[toLower(keyword)] = category;
    }
};
