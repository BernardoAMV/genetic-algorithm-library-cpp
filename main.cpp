// ============================================================================
// Genetic Algorithm Library — Usage Example
// ============================================================================
//
// This file demonstrates how to use the library to solve an optimization
// problem with mixed variable types (real, integer, and boolean), subject
// to general constraints (not just per-variable box limits).
//
// Example problem: investment allocation
//   - x1 (real):    fraction of capital in stocks          [0, 100]
//   - x2 (integer): number of fixed-income lots             [0, 50]
//   - x3 (bool):    whether to use leverage
//
// Objective: maximize estimated return, subject to risk constraints.
// ============================================================================

#include <iostream>
#include <vector>
#include <cmath>

#include "headers/GeneticAlgorithm.h"
#include "headers/GeneticTypes.h"
#include "headers/GeneFactory.h"

// ----------------------------------------------------------------------------
// Objective function
// ----------------------------------------------------------------------------
// Always receives a std::vector<double>, in the same order the GeneBounds
// were declared. int and bool arrive already converted to double (bool
// becomes 0.0/1.0). It's up to the caller to cast back if the original
// type is needed.
double investment_return(const std::vector<double>& x) {
    double stock_fraction = x[0];        // x1: real
    double fixed_income_lots = x[1];     // x2: integer (arrives as double)
    bool leverage = x[2] != 0.0;         // x3: bool (arrives as 0.0 or 1.0)

    double base_return = 0.08 * stock_fraction + 0.03 * fixed_income_lots;

    // Leverage increases expected return, but that's balanced by the
    // risk constraints below.
    if (leverage) {
        base_return *= 1.4;
    }

    return base_return;
}

// ----------------------------------------------------------------------------
// Constraints
// ----------------------------------------------------------------------------
// Convention: the function must return <= 0 when the constraint is
// satisfied, and a value > 0 (the size of the violation) when it's broken.
//
// To rewrite any inequality in this form:
//   A <= B   becomes   A - B <= 0   ->  return A - B;
//   A >= B   becomes   B - A <= 0   ->  return B - A;

// Constraint 1: stock allocation cannot exceed 80 (exposure cap)
double constraint_stock_limit(const std::vector<double>& x) {
    double stock_fraction = x[0];
    return stock_fraction - 80.0;   // stock_fraction <= 80
}

// Constraint 2: if leverage is used, stocks + 2x fixed income cannot
// exceed 120 (fictional combined-risk rule)
double constraint_leverage_risk(const std::vector<double>& x) {
    double stock_fraction = x[0];
    double fixed_income_lots = x[1];
    bool leverage = x[2] != 0.0;

    if (!leverage) {
        return -1.0;   // no leverage, constraint always satisfied
    }
    return (stock_fraction + 2 * fixed_income_lots) - 120.0;
}

// Constraint 3: minimum of 5 fixed-income lots (mandatory diversification)
// A >= B  ->  B - A <= 0
double constraint_min_diversification(const std::vector<double>& x) {
    double fixed_income_lots = x[1];
    return 5.0 - fixed_income_lots;   // fixed_income_lots >= 5
}

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
int main() {
    // 1. Search space: one GeneBounds per variable, in the order the
    //    objective function expects to find them in `x`.
    std::vector<GeneBounds> bounds = {
        std::pair<double, double>{0.0, 100.0},   // x1: stock fraction
        std::pair<int, int>{0, 50},               // x2: fixed-income lots
        std::monostate{}                            // x3: leverage (bool)
    };

    // 2. Constraints — as many as needed, each its own function.
    std::vector<GeneticAlgorithm::ConstraintFunction> constraints = {
        constraint_stock_limit,
        constraint_leverage_risk,
        constraint_min_diversification
    };

    // 3. Algorithm configuration.
    //    - Maximize: we want the highest possible return.
    //    - uniform_crossover: the library's default strategy.
    //    - constraints: defined above.
    //    - penalty_weight: the higher it is, the more "expensive"
    //      violating a constraint becomes.
    GeneticAlgorithm ga(
        GeneticAlgorithm::OptimizationGoal::Maximize,
        GeneticAlgorithm::uniform_crossover,
        constraints,
        1000.0
    );

    // 4. Run the evolution.
    //    Parameters: generations, population size, mutation rate,
    //    tournament size, bounds, objective function.
    auto result = ga.evolve(
        /*max_generations=*/ 300,
        /*population_size=*/ 100,
        /*mutation_rate=*/   0.1,
        /*tournament_size=*/ 3,
        bounds,
        investment_return
    );

    const Chromosome& best = result.first;
    double best_fitness = result.second;

    // 5. Report the result.
    auto values = GeneFactory::chromosome_to_doubles(best.get_genes());

    std::cout << "=== Best solution found ===\n";
    std::cout << "Stock fraction (x1):          " << values[0] << "\n";
    std::cout << "Fixed-income lots (x2):       " << values[1] << "\n";
    std::cout << "Uses leverage (x3):           " << (values[2] != 0.0 ? "yes" : "no") << "\n";
    std::cout << "Adjusted fitness:             " << best_fitness << "\n";
    std::cout << "Raw return (no penalty):      " << investment_return(values) << "\n\n";

    // 6. Manually check whether the constraints were respected.
    std::cout << "=== Constraint check ===\n";
    std::cout << "x1 <= 80:                     " << values[0]
              << (values[0] <= 80.0 ? " (ok)" : " (VIOLATED)") << "\n";

    double leverage_check = values[0] + 2 * values[1];
    bool leverage_on = values[2] != 0.0;
    std::cout << "x1 + 2*x2 <= 120 (if leverage): " << leverage_check
              << (!leverage_on || leverage_check <= 120.0 ? " (ok)" : " (VIOLATED)") << "\n";

    std::cout << "x2 >= 5:                      " << values[1]
              << (values[1] >= 5.0 ? " (ok)" : " (VIOLATED)") << "\n";

    return 0;
}