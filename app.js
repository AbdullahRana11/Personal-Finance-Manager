// ============================================================================
// app.js — Personal Finance Manager Dashboard Logic
// Mirrors the C++ backend: Singleton DB, Composite accounts, Strategy budgets,
// State pattern for financial modes, AI categorization & recommendations
// ============================================================================

// ─── SAMPLE DATA (same as main.cpp) ─────────────────────────────────────────
const transactions = [
  { date:"2025-05-01", desc:"Monthly Salary", amount:5000, account:"checking" },
  { date:"2025-05-02", desc:"Freelance Web Development", amount:1500, account:"checking" },
  { date:"2025-05-03", desc:"Walmart Grocery Shopping", amount:-120.50, account:"checking" },
  { date:"2025-05-04", desc:"Uber Ride to Office", amount:-25, account:"checking" },
  { date:"2025-05-05", desc:"Netflix Subscription", amount:-15.99, account:"checking" },
  { date:"2025-05-06", desc:"Restaurant dinner with friends", amount:-85, account:"checking" },
  { date:"2025-05-07", desc:"Amazon Electronics Purchase", amount:-299.99, account:"checking" },
  { date:"2025-05-08", desc:"Gym Membership", amount:-50, account:"checking" },
  { date:"2025-05-09", desc:"Monthly Rent Payment", amount:-1200, account:"checking" },
  { date:"2025-05-10", desc:"Electric Bill Utility", amount:-95, account:"checking" },
  { date:"2025-05-11", desc:"Savings Account Deposit", amount:-500, account:"savings" },
  { date:"2025-05-12", desc:"Stock Investment - AAPL", amount:-300, account:"investment" },
  { date:"2025-05-13", desc:"Mutual Fund Contribution", amount:-200, account:"investment" },
  { date:"2025-05-14", desc:"Emergency Fund Transfer", amount:-400, account:"savings" },
  { date:"2025-05-15", desc:"Coffee at Starbucks Cafe", amount:-6.50, account:"checking" },
  { date:"2025-05-16", desc:"Phone Bill Payment", amount:-70, account:"checking" },
  { date:"2025-05-17", desc:"Parking Garage Fee", amount:-15, account:"checking" },
  { date:"2025-05-18", desc:"Dividend Income", amount:120, account:"investment" },
  { date:"2025-05-19", desc:"Spotify Premium", amount:-9.99, account:"checking" },
  { date:"2025-05-20", desc:"Doctor Visit Copay", amount:-30, account:"checking" },
];

const goals = [
  { name:"Emergency Fund", target:10000, current:1000, deadline:"2025-12-31", icon:"🛡️", color:"#3b82f6" },
  { name:"Vacation to Japan", target:5000, current:1200, deadline:"2026-06-01", icon:"✈️", color:"#8b5cf6" },
  { name:"New Laptop", target:2000, current:1250, deadline:"2025-09-01", icon:"💻", color:"#06b6d4" },
  { name:"Retirement Fund", target:50000, current:2500, deadline:"2035-01-01", icon:"🏦", color:"#10b981" },
];

// ─── AI TRANSACTION CATEGORIZER (mirrors TransactionCategorizer.h) ──────────
const keywords = {
  salary:"income", paycheck:"income", freelance:"income", bonus:"income", dividend:"income",
  restaurant:"dining", cafe:"dining", coffee:"dining", pizza:"dining",
  grocery:"groceries", supermarket:"groceries", walmart:"groceries",
  uber:"transport", lyft:"transport", gas:"transport", parking:"transport", metro:"transport",
  rent:"rent", mortgage:"rent",
  electric:"utilities", "water bill":"utilities", internet:"utilities", "phone bill":"utilities",
  netflix:"entertainment", spotify:"entertainment", movie:"entertainment", gaming:"entertainment",
  amazon:"shopping", clothing:"shopping", electronics:"shopping",
  pharmacy:"healthcare", doctor:"healthcare", hospital:"healthcare", gym:"healthcare",
  insurance:"insurance",
  savings:"savings", deposit:"savings",
  investment:"investment", stock:"investment", "mutual fund":"investment",
  emergency:"emergency_fund"
};

function categorize(desc) {
  const lower = desc.toLowerCase();
  for (const [kw, cat] of Object.entries(keywords)) {
    if (lower.includes(kw)) return cat;
  }
  return "uncategorized";
}

// Categorize all transactions
transactions.forEach(t => { t.category = categorize(t.desc); });

// ─── CATEGORY COLORS ────────────────────────────────────────────────────────
const catColors = {
  income:"#10b981", dining:"#f59e0b", groceries:"#06b6d4", transport:"#8b5cf6",
  entertainment:"#ec4899", shopping:"#ef4444", healthcare:"#14b8a6", rent:"#6366f1",
  utilities:"#a855f7", savings:"#3b82f6", investment:"#22c55e", emergency_fund:"#0ea5e9",
  insurance:"#f97316", uncategorized:"#64748b"
};

// ─── BUDGET STRATEGIES (mirrors BudgetStrategy.h) ────────────────────────────
function getExpensesByCategory() {
  const map = {};
  transactions.forEach(t => {
    if (t.category !== "income") {
      map[t.category] = (map[t.category] || 0) + Math.abs(t.amount);
    }
  });
  return map;
}

const totalIncome = transactions.filter(t => t.amount > 0).reduce((s, t) => s + t.amount, 0);
const totalExpenses = transactions.filter(t => t.amount < 0).reduce((s, t) => s + Math.abs(t.amount), 0);

const strategies = {
  "503020": {
    name: "50/30/20 Rule",
    desc: "50% Needs | 30% Wants | 20% Savings & Investments",
    allocate() {
      const exp = getExpensesByCategory();
      const needsCats = ["rent","utilities","groceries","healthcare","transport","insurance"];
      const savCats = ["savings","investment","emergency_fund"];
      let needs=0, wants=0, sav=0;
      for (const [c,a] of Object.entries(exp)) {
        if (needsCats.includes(c)) needs += a;
        else if (savCats.includes(c)) sav += a;
        else wants += a;
      }
      return [
        { cat:"Needs (50%)", alloc:totalIncome*0.5, actual:needs },
        { cat:"Wants (30%)", alloc:totalIncome*0.3, actual:wants },
        { cat:"Savings (20%)", alloc:totalIncome*0.2, actual:sav },
      ];
    }
  },
  "zero": {
    name: "Zero-Based Budgeting",
    desc: "Every dollar is assigned a purpose. Income - Expenses = 0.",
    allocate() {
      const exp = getExpensesByCategory();
      const items = Object.entries(exp).map(([c,a]) => ({ cat:c, alloc:a, actual:a }));
      const tot = Object.values(exp).reduce((s,v) => s+v, 0);
      items.push({ cat:"UNALLOCATED", alloc:0, actual:0, diff:totalIncome - tot });
      return items;
    }
  },
  "envelope": {
    name: "Envelope Budgeting",
    desc: "Fixed budget envelopes assigned to each spending category.",
    allocate() {
      const envelopes = { groceries:400, dining:200, transport:150, entertainment:100,
        utilities:200, shopping:150, healthcare:100, savings:500, investment:300 };
      const exp = getExpensesByCategory();
      return Object.entries(envelopes).map(([c,limit]) => ({
        cat:c, alloc:limit, actual:exp[c]||0
      }));
    }
  }
};

// ─── AI RECOMMENDATIONS (mirrors RecommendationEngine.h) ─────────────────────
function generateRecommendations() {
  const recs = [];
  const savRate = ((totalIncome - totalExpenses) / totalIncome * 100);
  const exp = getExpensesByCategory();

  if (savRate < 10) recs.push({ cat:"savings", pri:"HIGH", msg:`Your savings rate is only ${savRate.toFixed(0)}%. Aim for at least 20%. Consider cutting discretionary spending immediately.`, icon:"💰" });
  else if (savRate < 20) recs.push({ cat:"savings", pri:"MEDIUM", msg:`Savings rate is ${savRate.toFixed(0)}%. Good progress, but try to reach the recommended 20% for financial security.`, icon:"💰" });
  else recs.push({ cat:"savings", pri:"LOW", msg:`Excellent savings rate of ${savRate.toFixed(0)}%! You're well above the recommended 20%. Consider investing surplus funds for higher returns.`, icon:"🎉" });

  const diningPct = ((exp.dining||0)/totalIncome*100);
  if (diningPct > 1) recs.push({ cat:"spending", pri:"MEDIUM", msg:`Dining expenses are ${diningPct.toFixed(1)}% of income ($${(exp.dining||0).toFixed(2)}). Try meal prepping 3x per week to reduce this by up to 50%.`, icon:"🍽️" });

  const shopPct = ((exp.shopping||0)/totalIncome*100);
  if (shopPct > 3) recs.push({ cat:"spending", pri:"HIGH", msg:`Shopping is ${shopPct.toFixed(1)}% of income ($${(exp.shopping||0).toFixed(2)}). Implement a 48-hour rule before non-essential purchases.`, icon:"🛒" });

  const invAmt = exp.investment||0;
  const invPct = (invAmt/totalIncome*100);
  if (invPct < 5) recs.push({ cat:"investment", pri:"HIGH", msg:"Investment allocation is very low. Start with low-cost index funds for diversified market exposure.", icon:"📈" });
  else if (invPct < 15) recs.push({ cat:"investment", pri:"MEDIUM", msg:"Consider diversifying your portfolio: mix stocks, bonds, and mutual funds. Rebalance quarterly.", icon:"📊" });
  else recs.push({ cat:"investment", pri:"LOW", msg:"Strong investment allocation! Review your portfolio quarterly for rebalancing opportunities.", icon:"✅" });

  const ef = exp.emergency_fund||0;
  if (ef < totalExpenses * 3) recs.push({ cat:"savings", pri:"HIGH", msg:"Emergency fund is below 3-month expense coverage. Financial experts recommend 3-6 months of expenses saved.", icon:"🚨" });

  recs.push({ cat:"general", pri:"MEDIUM", msg:"Set up automatic transfers to savings on payday. Automation removes the temptation to spend before saving.", icon:"🔄" });

  return recs;
}

// ─── FINANCIAL STATE (mirrors FinancialState.h) ──────────────────────────────
function getFinancialState() {
  const ratio = totalExpenses / totalIncome;
  if (ratio > 0.9) return { name:"Budgeting Mode", icon:"💡", color:"#ef4444" };
  const savings = (goals[0].current + goals[3].current);
  if (savings > totalIncome * 6) return { name:"Investment Mode", icon:"📈", color:"#10b981" };
  return { name:"Savings Mode", icon:"🏦", color:"#3b82f6" };
}

// ─── RENDER FUNCTIONS ────────────────────────────────────────────────────────

function renderDashboard() {
  document.getElementById("totalIncome").textContent = `$${totalIncome.toLocaleString("en-US",{minimumFractionDigits:2})}`;
  document.getElementById("totalExpenses").textContent = `$${totalExpenses.toLocaleString("en-US",{minimumFractionDigits:2})}`;
  const net = totalIncome - totalExpenses;
  document.getElementById("netSavings").textContent = `$${net.toLocaleString("en-US",{minimumFractionDigits:2})}`;
  document.getElementById("savingsRate").textContent = `${(net/totalIncome*100).toFixed(1)}% savings rate`;
  const avgProg = goals.reduce((s,g) => s + g.current/g.target, 0) / goals.length * 100;
  document.getElementById("goalProgress").textContent = `${avgProg.toFixed(0)}%`;

  // Income vs Expense bar chart
  new Chart(document.getElementById("incomeExpenseChart"), {
    type:"bar",
    data:{
      labels:["Income","Expenses","Net Savings"],
      datasets:[{ data:[totalIncome, totalExpenses, net],
        backgroundColor:["rgba(16,185,129,0.7)","rgba(239,68,68,0.7)","rgba(59,130,246,0.7)"],
        borderColor:["#10b981","#ef4444","#3b82f6"], borderWidth:2, borderRadius:8 }]
    },
    options:{ responsive:true, plugins:{ legend:{display:false} },
      scales:{ y:{ticks:{color:"#94a3b8"},grid:{color:"rgba(255,255,255,0.05)"}},
               x:{ticks:{color:"#94a3b8"},grid:{display:false}} } }
  });

  // Expense pie chart
  const exp = getExpensesByCategory();
  const labels = Object.keys(exp);
  const values = Object.values(exp);
  const colors = labels.map(l => catColors[l] || "#64748b");
  new Chart(document.getElementById("expensePieChart"), {
    type:"doughnut",
    data:{ labels, datasets:[{ data:values, backgroundColor:colors, borderWidth:0 }] },
    options:{ responsive:true, cutout:"60%",
      plugins:{ legend:{ position:"right", labels:{ color:"#94a3b8", padding:12, font:{size:12} } } } }
  });

  // Trend chart
  const days = transactions.map(t => t.date.slice(5));
  let cumulative = 0;
  const cumData = transactions.map(t => { cumulative += t.amount; return cumulative; });
  new Chart(document.getElementById("trendChart"), {
    type:"line",
    data:{ labels:days, datasets:[{
      label:"Cumulative Cash Flow", data:cumData,
      borderColor:"#8b5cf6", backgroundColor:"rgba(139,92,246,0.1)",
      fill:true, tension:0.4, pointRadius:4, pointBackgroundColor:"#8b5cf6" }] },
    options:{ responsive:true,
      scales:{ y:{ticks:{color:"#94a3b8"},grid:{color:"rgba(255,255,255,0.05)"}},
               x:{ticks:{color:"#94a3b8",maxRotation:45},grid:{display:false}} },
      plugins:{ legend:{ labels:{color:"#94a3b8"} } } }
  });
}

function renderTransactions() {
  const tbody = document.getElementById("txnTableBody");
  tbody.innerHTML = transactions.map(t => {
    const amtClass = t.amount >= 0 ? "amount-pos" : "amount-neg";
    const amtStr = t.amount >= 0 ? `+$${t.amount.toFixed(2)}` : `-$${Math.abs(t.amount).toFixed(2)}`;
    const color = catColors[t.category] || "#64748b";
    return `<tr>
      <td>${t.date}</td><td>${t.desc}</td>
      <td class="${amtClass}">${amtStr}</td>
      <td><span class="cat-badge" style="background:${color}22;color:${color}">${t.category}</span></td>
      <td>${t.account}</td></tr>`;
  }).join("");
}

function renderGoals() {
  document.getElementById("goalsGrid").innerHTML = goals.map(g => {
    const pct = (g.current / g.target * 100).toFixed(1);
    return `<div class="goal-card">
      <h4>${g.icon} ${g.name}</h4>
      <div class="goal-meta">Deadline: ${g.deadline}</div>
      <div class="progress-bar-bg"><div class="progress-bar-fill" style="width:${pct}%;background:${g.color}"></div></div>
      <div class="goal-amounts"><span>$${g.current.toLocaleString()} saved</span><span>${pct}% of $${g.target.toLocaleString()}</span></div>
    </div>`;
  }).join("");
}

function renderBudget(key) {
  const s = strategies[key];
  document.getElementById("budgetHeader").innerHTML = `<h3>${s.name}</h3><p>${s.desc}</p>`;
  const items = s.allocate();
  document.getElementById("budgetGrid").innerHTML = items.map(i => {
    if (i.cat === "UNALLOCATED") {
      return `<div class="budget-item"><h5>Unallocated</h5>
        <div class="budget-status ${i.diff < 1 ? 'status-ok' : 'status-warn'}">$${(i.diff||0).toFixed(2)} remaining</div></div>`;
    }
    const pct = i.alloc > 0 ? Math.min((i.actual/i.alloc)*100, 100) : 0;
    const over = i.actual > i.alloc;
    const color = over ? "#ef4444" : "#10b981";
    const status = over ? "OVER BUDGET" : "On Track";
    const statusClass = over ? "status-over" : "status-ok";
    return `<div class="budget-item"><h5>${i.cat}</h5>
      <div class="budget-bar-bg"><div class="budget-bar-fill" style="width:${pct}%;background:${color}"></div></div>
      <div class="budget-amounts"><span>$${i.actual.toFixed(2)}</span><span>$${i.alloc.toFixed(2)}</span></div>
      <div class="budget-status ${statusClass}">${status}</div></div>`;
  }).join("");

  // Budget chart
  const chartItems = items.filter(i => i.cat !== "UNALLOCATED");
  const existing = Chart.getChart("budgetChart");
  if (existing) existing.destroy();
  new Chart(document.getElementById("budgetChart"), {
    type:"bar",
    data:{ labels:chartItems.map(i=>i.cat),
      datasets:[
        { label:"Allocated", data:chartItems.map(i=>i.alloc), backgroundColor:"rgba(59,130,246,0.6)", borderRadius:6 },
        { label:"Actual", data:chartItems.map(i=>i.actual), backgroundColor:"rgba(239,68,68,0.6)", borderRadius:6 }
      ] },
    options:{ responsive:true,
      scales:{ y:{ticks:{color:"#94a3b8"},grid:{color:"rgba(255,255,255,0.05)"}},
               x:{ticks:{color:"#94a3b8",maxRotation:45},grid:{display:false}} },
      plugins:{ legend:{ labels:{color:"#94a3b8"} } } }
  });
}

function renderRecommendations() {
  const recs = generateRecommendations();
  document.getElementById("recsList").innerHTML = recs.map(r => {
    const priClass = r.pri === "HIGH" ? "high" : r.pri === "MEDIUM" ? "medium" : "low";
    const priLabel = r.pri === "HIGH" ? "priority-high" : r.pri === "MEDIUM" ? "priority-medium" : "priority-low";
    return `<div class="rec-card">
      <div class="rec-icon ${priClass}">${r.icon}</div>
      <div class="rec-content"><h4>${r.cat.charAt(0).toUpperCase()+r.cat.slice(1)}</h4>
        <p>${r.msg}</p><span class="rec-priority ${priLabel}">${r.pri} PRIORITY</span></div></div>`;
  }).join("");
}

function renderPatterns() {
  const patterns = [
    { name:"Singleton", type:"Creational Pattern", desc:"DatabaseConnection ensures exactly ONE instance exists. Verified by comparing memory addresses.", classes:["DatabaseConnection"], code:"getInstance()" },
    { name:"Composite", type:"Structural Pattern", desc:"FinancialComponent is the abstract base. Transaction is the leaf, Account is the composite containing children recursively.", classes:["FinancialComponent","Transaction","Account","SavingsAccount","CheckingAccount","InvestmentAccount"], code:"getBalance() — recursive" },
    { name:"Strategy", type:"Behavioral Pattern", desc:"IBudgetStrategy interface allows swapping budget algorithms at runtime. BudgetPlanner delegates to the active strategy.", classes:["IBudgetStrategy","FiftyThirtyTwentyRule","ZeroBasedBudgeting","EnvelopeBudgeting","BudgetPlanner"], code:"setStrategy()" },
    { name:"State (Bonus)", type:"Behavioral Pattern", desc:"IFinancialState transitions automatically between Budgeting, Savings, and Investment modes based on financial health metrics.", classes:["IFinancialState","BudgetingMode","SavingsMode","InvestmentMode","FinanceContext"], code:"evaluateTransition()" },
  ];
  document.getElementById("patternsGrid").innerHTML = patterns.map(p =>
    `<div class="pattern-card"><div class="pattern-type">${p.type}</div><h4>${p.name}</h4>
      <p>${p.desc}</p><p>Key method: <code>${p.code}</code></p>
      <div class="pattern-classes">${p.classes.map(c=>`<span>${c}</span>`).join("")}</div></div>`
  ).join("");
}

// ─── NAVIGATION ──────────────────────────────────────────────────────────────
document.querySelectorAll(".nav-links li").forEach(li => {
  li.addEventListener("click", () => {
    document.querySelectorAll(".nav-links li").forEach(l => l.classList.remove("active"));
    li.classList.add("active");
    const tab = li.dataset.tab;
    document.querySelectorAll(".tab-content").forEach(t => t.classList.remove("active"));
    document.getElementById("tab-" + tab).classList.add("active");
    const titles = { dashboard:"Dashboard", transactions:"Transactions", goals:"Financial Goals",
      budget:"Budget Planner", recommendations:"AI Insights", patterns:"Design Patterns" };
    document.getElementById("pageTitle").textContent = titles[tab] || "Dashboard";
  });
});

document.getElementById("strategySelect").addEventListener("change", e => {
  renderBudget(e.target.value);
});

// ─── STATE BADGE ─────────────────────────────────────────────────────────────
const state = getFinancialState();
document.getElementById("stateBadge").textContent = `${state.icon} ${state.name}`;
document.getElementById("stateBadge").style.color = state.color;
document.getElementById("stateBadge").style.background = state.color + "22";

// ─── INIT ────────────────────────────────────────────────────────────────────
renderDashboard();
renderTransactions();
renderGoals();
renderBudget("503020");
renderRecommendations();
renderPatterns();
