# Transaction Processing System — v2.0
### Mini Project – 24UCS271 Lab

---

## Objective

A fully upgraded **Bank Account Management System** written in C. The program manages up to 100 accounts stored in a random-access binary file (`credit.dat`), featuring a color terminal UI, PIN security, account types, interest, transaction history logging, sorting, and a web dashboard for visualisation.

---

## Reference Code

Original source:
🔗 https://replit.com/@ashokb/Unit5Programs#trans.c

---

## Project Files

| File | Description |
|------|-------------|
| `trans.c` | Main C source — all features, v2.0 |
| `credit.dat` | Binary random-access file (100 account slots) |
| `accounts.txt` | Text export (Menu 1) |
| `accounts.json` | JSON export for dashboard (Menu 12) |
| `history.log` | Timestamped transaction log (auto-generated) |
| `dashboard.html` | Dark-mode web dashboard — open in browser |
| `trans.exe` | Compiled executable |

---

## How to Compile and Run

```bash
gcc trans.c -o trans.exe -Wall -Wextra -std=c99
./trans.exe
```

> **Note:** `credit.dat` must be present in the same folder.
> **Note:** `credit.dat` must be re-initialized if upgrading from v1.0 (struct size changed).

---

## Menu Options

```
============== MENU =============
  1  - Export accounts to .txt
  2  - Update account balance     [PIN]
  3  - Add new account
  4  - Delete account             [PIN]
  5  - List all accounts
  6  - Search by name
  7  - Transfer funds             [PIN]
  8  - Account statistics
  9  - Apply interest (Savings)   [NEW]
  10 - Sort & display accounts    [NEW]
  11 - View transaction history   [NEW]
  12 - Export accounts to JSON    [NEW]
  0  - Exit
=================================
```

---

## Tasks Performed

1. ✅ **Compiled and ran** the original executable code.
2. ✅ **Tested** all existing menu options manually.
3. ✅ **Identified and fixed** 7 logical/safety bugs.
4. ✅ **Added 4 new features** (List, Search, Transfer, Statistics).
5. ✅ **Upgraded to v2.0** — 6 major enhancements (see below).
6. ✅ **Removed all comments** — clean production-style source.
7. ✅ **Compiled with zero warnings** using `-Wall -Wextra -std=c99`.

---

## v2.0 Upgrades

### 1. 🎨 ANSI Color Terminal UI
- Color-coded output: 🔴 errors, 🟢 success, 🟡 warnings, 🔵 headers
- ASCII art banner on startup
- Zero external libraries — pure ANSI escape codes

### 2. 🔐 PIN Security
- Each account has a **4-digit PIN** set on creation
- PIN verified before **Update / Delete / Transfer**
- Stored in the `clientData` struct inside `credit.dat`

### 3. 💰 Account Types + Interest
- Each account is **Savings (S)** or **Checking (C)**
- Savings accounts store an annual interest rate
- **Menu 9 — Apply Interest**: batch-credits all savings accounts and logs each transaction

### 4. 📋 Transaction History Log (`history.log`)
- Every transaction (deposit, withdrawal, transfer, delete, interest, create) is appended to `history.log`
- Format: `[YYYY-MM-DD HH:MM:SS] ACCT#N | TYPE | AMOUNT | BAL: X`
- **Menu 11**: view the last 20 transactions

### 5. 📊 Sort & Display
- **Menu 10**: sort all accounts by Balance ↑, Balance ↓, or Name A–Z
- Uses standard `qsort()` — no extra dependencies

### 6. 🌐 HTML Dashboard (`dashboard.html`)
- Opens directly in any browser — no server needed
- Load `accounts.json` exported via Menu 12
- Features:
  - **5 summary stat cards** (Total Accounts, Total Balance, Average, Highest, Lowest)
  - **Bar chart** — Top 10 balances (Chart.js)
  - **Doughnut chart** — Savings vs Checking split
  - **Sortable + searchable table** with color-coded balances

---

## Data Structure (v2.0)

```c
struct clientData {
    unsigned int acctNum;        // Account number (1–100)
    char         lastName[15];   // Last name
    char         firstName[10];  // First name
    double       balance;        // Account balance
    unsigned int pin;            // 4-digit PIN
    char         acctType;       // 'S' = Savings, 'C' = Checking
    double       interestRate;   // Annual interest rate (e.g. 0.035 = 3.5%)
};
```

Each record has a **fixed size** on disk. Account `N` is stored at byte offset `(N-1) × sizeof(clientData)` — **O(1) random access**.

---

## Bug Fixes Applied (v1.0)

| # | Bug | Fix |
|---|-----|-----|
| 1 | `feof()` extra iteration | Replaced with `fread()` return check |
| 2 | `sizeof()` unsigned overflow | Cast to `(long)` |
| 3 | No range validation | Added `isValidAccount()` |
| 4 | `scanf()` unchecked | Added `!= 1` checks + buffer flush |
| 5 | `exit(-1)` non-portable | Changed to `EXIT_FAILURE` |
| 6 | Silent overdraft | Overdraft warning + y/n confirm |
| 7 | Negative opening balance | Rejected in `newRecord()` |

---

## Helper Functions

| Function | Purpose |
|----------|---------|
| `isValidAccount()` | Range check — 1 to 100 |
| `readRecord()` | Read one record by account number |
| `writeRecord()` | Write one record to correct offset |
| `printHeader()` | Formatted table header (with color) |
| `printRecord()` | One account row (color-coded balance) |
| `verifyPIN()` | PIN prompt and verification |
| `logTransaction()` | Append timestamped entry to history.log |
| `printBanner()` | ASCII art startup banner |

---

## Evaluation Criteria

### 1. General (25 Points)
| Activity | Points |
|----------|--------|
| Self-effort | 5 |
| Turnaround Time | 10 |
| Project Demo | 10 |

### 2. Comprehension (15 Points)
| Activity | Points |
|----------|--------|
| Domain Knowledge | 5 |
| Added Functionality (Ideas) | 5 |
| Code Comprehension | 5 |

### 3. Modification (35 Points)
| Activity | Points |
|----------|--------|
| Code Improvement | 5 |
| Functional Decomposition | 10 |
| Refactoring for Memory Usage | 10 |
| Refactoring for Speed | 10 |

### 4. Innovation (45 Points)
| Activity | Points |
|----------|--------|
| New Features / User Stories | 5 |
| Requirement to Code Translation | 10 |
| Added Functionality (Simple – Error Handling) | 10 |
| Added Functionality (Advanced) | 20 |

**Total: 120 Marks → Scaled to 20 Marks**

---

## Rules

- Max **2 attempts** for project presentation.
- Best score out of two attempts is final.
- **Copying is strictly prohibited** — zero (0) score for plagiarism.

---

## Additional References

🔗 http://j.mp/transactionUnit5
🔗 https://htmlpreview.github.io/?https://github.com/kgisl/makesite/blob/master/content/blog/2021-06-23-unit5-mini-project.md.html
🔗 https://github.com/24UCS271-MiniProject/miniProjectSourceCode/

---

**Date Created:** 31/01/2026
**v1.0 Completed:** 08/05/2026
**v2.0 Upgraded:** 11/05/2026
