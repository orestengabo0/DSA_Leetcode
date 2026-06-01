/*
 * ============================================================================
 *  Rwanda Ministry of Infrastructure — Road Network Management System
 *  Language  : C++17
 * ============================================================================
 *
 *  MENUS
 *  ─────
 *   1. Add new city(ies)
 *   2. Add road between two cities
 *   3. Add / update budget for a road
 *   4. Edit a city name  (by index)
 *   5. Search for a city (by name)
 *   6. Display city list
 *   7. Display road adjacency matrix
 *   8. Display all recorded data (cities + road matrix + budget matrix)
 *   9. Exit  →  auto-saves cities.txt & roads.txt
 *
 *  VALIDATIONS  (exhaustive list)
 *  ───────────────────────────────
 *   ✔  Menu choice: must be integer 1-9; non-numeric input re-prompts
 *   ✔  Number of cities to add: must be ≥ 1; non-numeric re-prompts
 *   ✔  City name: non-empty
 *   ✔  City name: only letters, spaces, hyphens
 *   ✔  City name: no duplicate (case-insensitive)
 *   ✔  Edit index: in valid range [1, N]
 *   ✔  Edit new name: same rules as city name + no conflict with other cities
 *   ✔  Road input: both city names must already exist
 *   ✔  Road input: source ≠ destination
 *   ✔  Road input: road must not already exist (no duplicate roads)
 *   ✔  Budget input: road must exist first
 *   ✔  Budget input: source ≠ destination
 *   ✔  Budget value: strictly positive real number; non-numeric re-prompts
 *   ✔  Context guards: roads / budgets menus require ≥ 2 cities
 *   ✔  All cin failures cleared; no silent stream-eating
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <limits>

// ════════════════════════════════════════════════════════════════════════════
//   SECTION 1 — STRING UTILITIES
// ════════════════════════════════════════════════════════════════════════════

// Remove leading / trailing whitespace.
static std::string trim(const std::string& s)
{
    static const std::string WS = " \t\r\n";
    size_t a = s.find_first_not_of(WS);
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(WS);
    return s.substr(a, b - a + 1);
}

// Return a lower-case copy.
static std::string toLower(const std::string& s)
{
    std::string out = s;
    for (auto& c : out)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

// A valid city name contains only Unicode letters, spaces, or hyphens.
// We test ASCII letters here (sufficient for Rwandan city names).
static bool isValidCityName(const std::string& name)
{
    if (name.empty()) return false;
    for (unsigned char c : name)
        if (!std::isalpha(c) && c != ' ' && c != '-') return false;
    return true;
}

// ════════════════════════════════════════════════════════════════════════════
//   SECTION 2 — SAFE INPUT HELPERS
// ════════════════════════════════════════════════════════════════════════════

/*
 * Read one non-empty line, trimmed.
 * Loops on blank/EOF.
 */
static std::string inputLine(const std::string& prompt)
{
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            // Recover from EOF / stream error.
            std::cin.clear();
            std::cout << "\n  [!] Stream error — please try again.\n";
            continue;
        }
        line = trim(line);
        if (!line.empty()) return line;
        std::cout << "  [!] Input cannot be blank. Please try again.\n";
    }
}

/*
 * Read an integer in [lo, hi].
 * Rejects: blank, non-numeric chars, out-of-range values.
 */
static int inputIntRange(const std::string& prompt, int lo, int hi)
{
    while (true) {
        std::cout << prompt;
        std::string raw;
        if (!std::getline(std::cin, raw)) {
            std::cin.clear();
            std::cout << "\n  [!] Stream error — please try again.\n";
            continue;
        }
        raw = trim(raw);
        if (raw.empty()) {
            std::cout << "  [!] Input cannot be blank.\n";
            continue;
        }
        // Check every character is a digit (allow optional leading sign)
        bool ok = true;
        size_t start = (raw[0] == '+' || raw[0] == '-') ? 1 : 0;
        if (start == raw.size()) ok = false;  // bare sign
        for (size_t i = start; i < raw.size() && ok; ++i)
            if (!std::isdigit(static_cast<unsigned char>(raw[i]))) ok = false;
        if (!ok) {
            std::cout << "  [!] \"" << raw
                      << "\" is not a valid integer. "
                         "Expected a value between "
                      << lo << " and " << hi << ".\n";
            continue;
        }
        long long val = 0;
        try { val = std::stoll(raw); }
        catch (...) {
            std::cout << "  [!] Number too large. Enter a value between "
                      << lo << " and " << hi << ".\n";
            continue;
        }
        if (val < static_cast<long long>(lo) ||
            val > static_cast<long long>(hi)) {
            std::cout << "  [!] " << val
                      << " is outside the allowed range ["
                      << lo << ", " << hi << "]. Please try again.\n";
            continue;
        }
        return static_cast<int>(val);
    }
}

/*
 * Read a strictly positive integer (no upper bound).
 * Rejects: blank, negative, zero, non-numeric.
 */
static int inputPositiveInt(const std::string& prompt)
{
    while (true) {
        std::cout << prompt;
        std::string raw;
        if (!std::getline(std::cin, raw)) { std::cin.clear(); continue; }
        raw = trim(raw);
        if (raw.empty()) {
            std::cout << "  [!] Input cannot be blank.\n";
            continue;
        }
        bool ok = true;
        for (unsigned char c : raw)
            if (!std::isdigit(c)) { ok = false; break; }
        if (!ok) {
            std::cout << "  [!] \"" << raw
                      << "\" is not a valid positive integer.\n";
            continue;
        }
        long long val = 0;
        try { val = std::stoll(raw); } catch (...) { val = 0; }
        if (val < 1) {
            std::cout << "  [!] Value must be at least 1. Try again.\n";
            continue;
        }
        return static_cast<int>(val);
    }
}

/*
 * Read a strictly positive floating-point number (budget).
 * Accepts: digits with at most one decimal point, optional leading '+'.
 * Rejects: negative sign, letters, multiple dots, zero.
 */
static double inputPositiveDouble(const std::string& prompt)
{
    while (true) {
        std::cout << prompt;
        std::string raw;
        if (!std::getline(std::cin, raw)) { std::cin.clear(); continue; }
        raw = trim(raw);
        if (raw.empty()) {
            std::cout << "  [!] Input cannot be blank.\n";
            continue;
        }
        // Strip optional leading '+'
        std::string digits = (raw[0] == '+') ? raw.substr(1) : raw;
        if (digits.empty()) {
            std::cout << "  [!] \"" << raw
                      << "\" is not a valid positive number.\n";
            continue;
        }
        // Allow digits and exactly one '.'
        int dots = 0;
        bool ok  = true;
        for (unsigned char c : digits) {
            if (c == '.') { ++dots; if (dots > 1) { ok = false; break; } }
            else if (!std::isdigit(c)) { ok = false; break; }
        }
        if (!ok || digits.empty()) {
            std::cout << "  [!] \"" << raw
                      << "\" is not a valid positive number.\n";
            continue;
        }
        double val = 0.0;
        try { val = std::stod(digits); } catch (...) { val = 0.0; }
        if (val <= 0.0) {
            std::cout << "  [!] Budget must be strictly positive (> 0). "
                         "Try again.\n";
            continue;
        }
        return val;
    }
}

// ════════════════════════════════════════════════════════════════════════════
//   SECTION 3 — DISPLAY UTILITIES
// ════════════════════════════════════════════════════════════════════════════

static void divider(char ch = '-', int w = 66)
{
    std::cout << std::string(w, ch) << '\n';
}

static void printBanner()
{
    divider('=');
    std::cout
        << "  RWANDA MINISTRY OF INFRASTRUCTURE\n"
        << "  Road Network Management System\n";
    divider('=');
}

static void printMenu()
{
    std::cout << '\n';
    divider();
    std::cout
        << "  MAIN MENU\n";
    divider();
    std::cout
        << "  1. Add new city(ies)\n"
        << "  2. Add road between cities\n"
        << "  3. Add / update budget for a road\n"
        << "  4. Edit city name\n"
        << "  5. Search for a city by name\n"
        << "  6. Display cities\n"
        << "  7. Display roads (adjacency matrix)\n"
        << "  8. Display all recorded data\n"
        << "  9. Exit\n";
    divider();
}

// ════════════════════════════════════════════════════════════════════════════
//   SECTION 4 — CORE APPLICATION
// ════════════════════════════════════════════════════════════════════════════

class InfraApp
{
    // ── Data ───────────────────────────────────────────────────────────────
    std::vector<std::string>          cities_;  // cities_[i] → name; index = i+1
    std::vector<std::vector<int>>     road_;    // 0/1 symmetric adjacency matrix
    std::vector<std::vector<double>>  budget_;  // symmetric budget matrix (BillionRWF)

    // ── Size helper ────────────────────────────────────────────────────────
    int N() const { return static_cast<int>(cities_.size()); }

    // ── Name lookup (case-insensitive) → 0-based pos, or -1 ───────────────
    int findCity(const std::string& name) const
    {
        const std::string lc = toLower(trim(name));
        for (int i = 0; i < N(); ++i)
            if (toLower(cities_[i]) == lc) return i;
        return -1;
    }

    // ── Grow both matrices to accommodate `newTotal` cities ────────────────
    void resize(int newTotal)
    {
        // Widen existing rows
        for (auto& row : road_)   row.resize(newTotal, 0);
        for (auto& row : budget_) row.resize(newTotal, 0.0);
        // Append new rows
        while (static_cast<int>(road_.size()) < newTotal) {
            road_.push_back(std::vector<int>(newTotal, 0));
            budget_.push_back(std::vector<double>(newTotal, 0.0));
        }
    }

    // ── Ask for a valid, non-duplicate city name ───────────────────────────
    std::string askNewCityName(int serial)
    {
        while (true) {
            std::string name = inputLine(
                "  Enter the name for city " + std::to_string(serial) + ": ");

            // Rule 1 — valid characters
            if (!isValidCityName(name)) {
                std::cout
                    << "  [!] City name must contain only letters, spaces, "
                       "or hyphens.\n"
                    << "      It must not be empty. Please try again.\n";
                continue;
            }
            // Rule 2 — no duplicate (case-insensitive)
            int dup = findCity(name);
            if (dup != -1) {
                std::cout
                    << "  [!] \"" << name << "\" already exists as city "
                    << (dup + 1) << " (" << cities_[dup] << ").\n"
                    << "      Each city may only be recorded once.\n";
                continue;
            }
            return name;
        }
    }

    // ── Ask for an existing city name, return its 0-based index ───────────
    int askExistingCity(const std::string& label)
    {
        while (true) {
            std::string name = inputLine(label);
            int idx = findCity(name);
            if (idx == -1) {
                std::cout
                    << "  [!] City \"" << name << "\" was not found.\n"
                    << "      Check spelling (search is case-insensitive) "
                       "and try again.\n";
            } else {
                return idx;
            }
        }
    }

    // ── Print the city list ────────────────────────────────────────────────
    void printCities() const
    {
        std::cout << "\nCities:\n";
        for (int i = 0; i < N(); ++i)
            std::cout << "  " << std::setw(3) << (i + 1)
                      << ". " << cities_[i] << '\n';
    }

    // ── Print the 0/1 road adjacency matrix ───────────────────────────────
    void printRoadMatrix() const
    {
        const int n = N();
        // Column header
        std::cout << "       ";
        for (int j = 0; j < n; ++j) std::cout << std::setw(3) << (j + 1);
        std::cout << "\n       ";
        for (int j = 0; j < n; ++j) std::cout << "---";
        std::cout << '\n';
        // Rows
        for (int i = 0; i < n; ++i) {
            std::cout << std::setw(4) << (i + 1) << " | ";
            for (int j = 0; j < n; ++j)
                std::cout << std::setw(3) << road_[i][j];
            std::cout << '\n';
        }
    }

    // ── Print the budget adjacency matrix ─────────────────────────────────
    void printBudgetMatrix() const
    {
        const int n = N();
        // Column header
        std::cout << "        ";
        for (int j = 0; j < n; ++j) std::cout << std::setw(8) << (j + 1);
        std::cout << "\n        ";
        for (int j = 0; j < n; ++j) std::cout << "--------";
        std::cout << '\n';
        // Rows
        for (int i = 0; i < n; ++i) {
            std::cout << std::setw(4) << (i + 1) << " |  ";
            for (int j = 0; j < n; ++j)
                std::cout << std::fixed << std::setprecision(1)
                          << std::setw(8) << budget_[i][j];
            std::cout << '\n';
        }
    }

    // ── File: save cities.txt ──────────────────────────────────────────────
    void saveCities(const std::string& path = "cities.txt") const
    {
        std::ofstream f(path);
        if (!f) {
            std::cerr << "  [!] Cannot open \"" << path << "\" for writing.\n";
            return;
        }
        f << std::left << std::setw(9) << "Index" << "City_Name\n";
        for (int i = 0; i < N(); ++i)
            f << std::left << std::setw(9) << (i + 1) << cities_[i] << '\n';
        std::cout << "  [+] Saved → " << path << '\n';
    }

    // ── File: save roads.txt ───────────────────────────────────────────────
    void saveRoads(const std::string& path = "roads.txt") const
    {
        std::ofstream f(path);
        if (!f) {
            std::cerr << "  [!] Cannot open \"" << path << "\" for writing.\n";
            return;
        }
        f << std::left
          << std::setw(6)  << "Nbr"
          << std::setw(30) << "Road"
          << "Budget (Billion RWF)\n";

        int nbr = 0;
        for (int i = 0; i < N(); ++i) {
            for (int j = i + 1; j < N(); ++j) {
                if (road_[i][j] == 1) {
                    ++nbr;
                    std::string label = cities_[i] + "-" + cities_[j];
                    f << std::left
                      << std::setw(6)  << (std::to_string(nbr) + ".")
                      << std::setw(30) << label
                      << std::fixed << std::setprecision(2)
                      << budget_[i][j] << '\n';
                }
            }
        }
        std::cout << "  [+] Saved → " << path << '\n';
    }

    // ════════════════════════════════════════════════════════════════════════
    //   MENU HANDLERS
    // ════════════════════════════════════════════════════════════════════════

    // ── MENU 1 ─────────────────────────────────────────────────────────────
    void menuAddCities()
    {
        std::cout << '\n'; divider();
        std::cout << "  ADD NEW CITY(IES)\n"; divider();

        int n = inputPositiveInt("  How many cities do you want to add? ");

        // Pre-grow matrices so uniqueness checks work during entry.
        resize(N() + n);

        int added = 0;
        for (int k = 1; k <= n; ++k) {
            std::string name = askNewCityName(k);
            cities_.push_back(name);
            ++added;
            std::cout << "  [+] \"" << name << "\" added  →  index "
                      << N() << ".\n";
        }

        // Trim matrices back to actual count (safety net).
        int actual = N();
        road_.resize(actual);
        budget_.resize(actual);
        for (auto& row : road_)   row.resize(actual, 0);
        for (auto& row : budget_) row.resize(actual, 0.0);

        std::cout << "  ── " << added << " city(ies) recorded. "
                     "Total cities: " << N() << " ──\n";
    }

    // ── MENU 2 ─────────────────────────────────────────────────────────────
    void menuAddRoad()
    {
        std::cout << '\n'; divider();
        std::cout << "  ADD ROAD BETWEEN CITIES\n"; divider();

        if (N() < 2) {
            std::cout << "  [!] At least 2 cities must be recorded "
                         "before roads can be added.\n";
            return;
        }

        printCities();
        std::cout << '\n';

        int i = askExistingCity("  Enter the name of the first  city: ");
        int j = askExistingCity("  Enter the name of the second city: ");

        // Validation: self-loop
        if (i == j) {
            std::cout << "  [!] A road cannot connect a city to itself.\n";
            return;
        }
        // Validation: duplicate road
        if (road_[i][j] == 1) {
            std::cout << "  [!] A road between \""
                      << cities_[i] << "\" and \""
                      << cities_[j] << "\" already exists.\n";
            return;
        }

        road_[i][j] = road_[j][i] = 1;
        std::cout << "  [+] Road added successfully between \""
                  << cities_[i] << "\" and \"" << cities_[j] << "\".\n";
    }

    // ── MENU 3 ─────────────────────────────────────────────────────────────
    void menuAddBudget()
    {
        std::cout << '\n'; divider();
        std::cout << "  ADD / UPDATE ROAD BUDGET\n"; divider();

        if (N() < 2) {
            std::cout << "  [!] At least 2 cities must be recorded.\n";
            return;
        }

        printCities();
        std::cout << '\n';

        int i = askExistingCity("  Enter the name of the first  city: ");
        int j = askExistingCity("  Enter the name of the second city: ");

        // Validation: self-loop
        if (i == j) {
            std::cout << "  [!] Source and destination must be different cities.\n";
            return;
        }
        // Validation: road must exist first
        if (road_[i][j] == 0) {
            std::cout << "  [!] No road exists between \""
                      << cities_[i] << "\" and \"" << cities_[j] << "\".\n"
                      << "      Register the road first (menu option 2).\n";
            return;
        }

        // Show existing budget if one is already set
        if (budget_[i][j] > 0.0) {
            std::cout << "  (Current budget: "
                      << std::fixed << std::setprecision(2)
                      << budget_[i][j] << " Billion RWF — will be overwritten.)\n";
        }

        double b = inputPositiveDouble(
            "  Enter the budget for the road (Billion RWF): ");

        budget_[i][j] = budget_[j][i] = b;
        std::cout << "  [+] Budget added successfully for the road between \""
                  << cities_[i] << "\" and \"" << cities_[j] << "\": "
                  << std::fixed << std::setprecision(2) << b
                  << " Billion RWF.\n";
    }

    // ── MENU 4 ─────────────────────────────────────────────────────────────
    void menuEditCity()
    {
        std::cout << '\n'; divider();
        std::cout << "  EDIT CITY NAME\n"; divider();

        if (N() == 0) {
            std::cout << "  [!] No cities have been recorded yet.\n";
            return;
        }

        printCities();

        int idx = inputIntRange(
            "\n  Enter the index of the city to edit [1-"
                + std::to_string(N()) + "]: ",
            1, N());

        std::cout << "  Current name: " << cities_[idx - 1] << '\n';

        while (true) {
            std::string newName = inputLine(
                "  Enter the new name for city " + std::to_string(idx) + ": ");

            // Rule 1 — valid characters
            if (!isValidCityName(newName)) {
                std::cout << "  [!] Name must contain only letters, "
                             "spaces, or hyphens. Try again.\n";
                continue;
            }
            // Rule 2 — no conflict with OTHER cities
            int dup = findCity(newName);
            if (dup != -1 && dup != (idx - 1)) {
                std::cout << "  [!] \"" << newName
                          << "\" is already used by city "
                          << (dup + 1) << " (" << cities_[dup]
                          << "). Choose a different name.\n";
                continue;
            }

            std::string old = cities_[idx - 1];
            cities_[idx - 1] = newName;
            std::cout << "  [+] City updated successfully: \""
                      << old << "\"  →  \"" << newName << "\".\n";
            break;
        }
    }

    // ── MENU 5 ─────────────────────────────────────────────────────────────
    void menuSearchCity()
    {
        std::cout << '\n'; divider();
        std::cout << "  SEARCH FOR A CITY BY NAME\n"; divider();

        if (N() == 0) {
            std::cout << "  [!] No cities have been recorded yet.\n";
            return;
        }

        std::string name = inputLine("  Enter the city name to search: ");

        if (!isValidCityName(name)) {
            std::cout << "  [!] \"" << name
                      << "\" contains characters not valid for a city name.\n";
            return;
        }

        int pos = findCity(name);
        if (pos == -1) {
            std::cout << "  [✗] City \"" << name << "\" was not found.\n";
            return;
        }

        std::cout << "  [✓] Found  ─  Index: " << (pos + 1)
                  << "   Name: " << cities_[pos] << '\n';

        // Bonus: show connected neighbours
        std::cout << "      Connected roads: ";
        bool any = false;
        for (int j = 0; j < N(); ++j) {
            if (road_[pos][j]) {
                std::cout << (any ? ", " : "") << cities_[j];
                any = true;
            }
        }
        std::cout << (any ? "\n" : "(none)\n");
    }

    // ── MENU 6 ─────────────────────────────────────────────────────────────
    void menuDisplayCities()
    {
        std::cout << '\n'; divider();
        std::cout << "  CITIES\n"; divider();

        if (N() == 0) {
            std::cout << "  (No cities recorded yet.)\n";
            return;
        }
        printCities();
    }

    // ── MENU 7 ─────────────────────────────────────────────────────────────
    void menuDisplayRoads()
    {
        std::cout << '\n'; divider();
        std::cout << "  ROAD ADJACENCY MATRIX\n"; divider();

        if (N() == 0) {
            std::cout << "  (No cities recorded yet.)\n";
            return;
        }
        printCities();
        std::cout << '\n';
        printRoadMatrix();
    }

    // ── MENU 8 ─────────────────────────────────────────────────────────────
    void menuDisplayAll()
    {
        std::cout << '\n'; divider();
        std::cout << "  ALL RECORDED DATA\n"; divider();

        if (N() == 0) {
            std::cout << "  (No data recorded yet.)\n";
            return;
        }
        printCities();
        std::cout << "\nRoad adjacency matrix:\n";
        printRoadMatrix();
        std::cout << "\nBudget adjacency matrix (Billion RWF):\n";
        printBudgetMatrix();
    }

    // ── MENU 9 ─────────────────────────────────────────────────────────────
    void menuExit()
    {
        std::cout << '\n';
        std::cout << "  Saving data to files...\n";
        saveCities("cities.txt");
        saveRoads("roads.txt");
        std::cout << "\n  Thank you for using the Rwanda Infrastructure Manager.\n";
        divider('=');
    }

public:

    // ════════════════════════════════════════════════════════════════════════
    //   PUBLIC ENTRY POINT
    // ════════════════════════════════════════════════════════════════════════
    void run()
    {
        printBanner();
        bool alive = true;
        while (alive) {
            printMenu();
            int choice = inputIntRange("  Enter your choice [1-9]: ", 1, 9);
            switch (choice) {
                case 1: menuAddCities();     break;
                case 2: menuAddRoad();       break;
                case 3: menuAddBudget();     break;
                case 4: menuEditCity();      break;
                case 5: menuSearchCity();    break;
                case 6: menuDisplayCities(); break;
                case 7: menuDisplayRoads();  break;
                case 8: menuDisplayAll();    break;
                case 9: menuExit(); alive = false; break;
            }
        }
    }
};

// ════════════════════════════════════════════════════════════════════════════
//   MAIN
// ════════════════════════════════════════════════════════════════════════════

int main()
{
    InfraApp app;
    app.run();
    return 0;
}